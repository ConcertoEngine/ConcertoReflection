//
// Clang LibTooling based parser for Concerto Package Generator
//

#include "Concerto/PackageGenerator/ClangParser.hpp"
#include <Concerto/Core/Logger.hpp>
#include <toml11/parser.hpp>
#include <algorithm>
#include <optional>
#include <sstream>

#include <clang/AST/ASTContext.h>
#include <clang/AST/Attr.h>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/DeclTemplate.h>
#include <clang/AST/Type.h>
#include <clang/Frontend/ASTUnit.h>
#include <clang/Lex/Lexer.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/CompilationDatabase.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/ADT/SmallString.h>

using namespace clang;
using namespace clang::tooling;

namespace
{

	TomlAttributes GetAttributesOr(TomlAttributes attributes, TomlAttributes defaultValue)
	{
		if (attributes.is_table() == false)
			return defaultValue;
		auto& attributesTable = attributes.as_table();
		auto it = attributesTable.find("Attributes");
		if (it == attributesTable.end())
			return defaultValue;
		return it->second;
	}

	std::string QualTypeToString(const QualType& qt, const ASTContext& ctx)
	{
		PrintingPolicy policy(ctx.getLangOpts());
		policy.SuppressTagKeyword = true;
		policy.Bool = true;
		policy.SuppressUnwrittenScope = true;
		policy.AnonymousTagLocations = false;
		return qt.getAsString(policy);
	}

	std::vector<std::string> GetNamespaceChain(const DeclContext* DC)
	{
		std::vector<std::string> chain;
		while (DC)
		{
			if (const auto* ND = llvm::dyn_cast<NamespaceDecl>(DC))
			{
				if (!ND->isAnonymousNamespace())
					chain.emplace_back(ND->getNameAsString());
			}
			DC = DC->getParent();
		}
		std::ranges::reverse(chain);
		return chain;
	}

	Namespace* EnsureNamespace(Package& pkg, const std::vector<std::string>& chain)
	{
		Namespace* current = nullptr;
		auto* level = &pkg.namepsaces;
		for (const auto& name : chain)
		{
			auto it = std::ranges::find_if(*level, [&](const Namespace& ns) { return ns.name == name; });
			if (it == level->end())
			{
				Namespace ns;
				ns.name = name;
				level->push_back(std::move(ns));
				current = &level->back();
			}
			else
			{
				current = &*it;
			}
			level = &current->namespaces;
		}
		return current; // nullptr if chain empty
	}

	bool ExtractCctAttribute(const Attr* A, ASTContext& astContext, const SourceManager& SM, const LangOptions& LO,
		std::string& outScope, TomlAttributes& outAttrs)
	{
		const IdentifierInfo* identifierInfo = A->getAttrName();
		auto attributeName = identifierInfo ? identifierInfo->getName() : StringRef();
		const auto* AnnotateAttribute = dyn_cast<AnnotateAttr>(A);
		if (AnnotateAttribute == nullptr)
			return false;

		using namespace std::string_view_literals;

		constexpr std::array knownAnnotations = {
			"cct::Package"sv,
			"cct::Class"sv,
			"cct::Member"sv,
			"cct::Method"sv,
			"cct::Enum"sv,
			"cct::EnumValue"sv
		};

		std::string_view annotation(AnnotateAttribute->getAnnotation().begin(), AnnotateAttribute->getAnnotation().end());
		bool known = false;
		for (auto& annotationName : knownAnnotations)
		{
			if (annotationName == annotation)
			{
				known = true;
				break;
			}
		}

		if (known == false)
			return false;

		outScope = annotation;

		std::string tomlStr;
		for (auto& args : AnnotateAttribute->args())
		{
			auto str = args->tryEvaluateString(astContext);
			if (str)
				tomlStr += *str + '\n';
		}
		tomlStr += '\n';
		auto result = toml::try_parse_str(tomlStr);
		if (result.is_err())
		{
			for (auto& err : result.as_err())
				cct::Logger::Error("{}\n\t{}", err.title(), err.suffix());
			return {};
		}

		outAttrs = result.as_ok();
		return true;
	}

	void InsertClassIntoPackage(Package& pkg, const std::vector<std::string>& nsChain, const Class& cls)
	{
		if (nsChain.empty())
		{
			pkg.classes.push_back(cls);
			return;
		}
		Namespace* leaf = EnsureNamespace(pkg, nsChain);
		if (leaf)
			leaf->classes.push_back(cls);
	}

	void InsertEnumIntoPackage(Package& pkg, const std::vector<std::string>& nsChain, const Enum& enm)
	{
		if (nsChain.empty())
		{
			pkg.enums.push_back(enm);
			return;
		}
		Namespace* leaf = EnsureNamespace(pkg, nsChain);
		if (leaf)
			leaf->enums.push_back(enm);
	}
}

namespace cct
{
	Package ClangParser::Parse(const std::vector<std::string>& includeDirs,
		const std::vector<std::string>& defines,
		const std::vector<std::string>& sources)
	{
		std::string code;

		for (auto& src : sources)
			code += std::format("#include \"{}\"\n", src);

		std::vector<std::string> args;
		args.reserve(defines.size() + includeDirs.size() + 1);

		args.emplace_back("-std=c++20");
		args.emplace_back("-xc++");

		for (auto& define : defines)
			args.emplace_back("-D " + define);

		for (auto& include : includeDirs)
			args.emplace_back("-I" + include);

		std::unique_ptr<ASTUnit> AST = buildASTFromCodeWithArgs(code, args);
		if (!AST)
		{
			Logger::Warning("LibTooling: failed to parse");
			return {};
		}
		
		m_astContext = &AST->getASTContext();
		m_sourceManager = &m_astContext->getSourceManager();
		m_langOptions = &m_astContext->getLangOpts();

		const TranslationUnitDecl* TU = AST->getASTContext().getTranslationUnitDecl();
		for (const auto* D : TU->decls())
			ProcessDeclaration(D);

		return m_package;
	}

	void ClangParser::ProcessRecord(const CXXRecordDecl* recordDeclaration)
	{
		if (!recordDeclaration || !recordDeclaration->isThisDeclarationADefinition())
			return;

		std::optional<std::pair<std::string, TomlAttributes>> classAttr;
		std::optional<std::pair<std::string, TomlAttributes>> packageAttr;
		for (const auto* A : recordDeclaration->attrs())
		{
			std::string scope; TomlAttributes attrs;
			if (!ExtractCctAttribute(A, *m_astContext, *m_sourceManager, *m_langOptions, scope, attrs))
				continue;
			if (scope == "cct::Class") classAttr = std::make_pair(scope, attrs);
			else if (scope == "cct::Package") packageAttr = std::make_pair(scope, attrs);
		}
		auto txt = recordDeclaration->getNameAsString();
		if (!classAttr && !packageAttr)
			return; // Not a reflection-relevant class

		if (packageAttr)
		{
			if (m_package.name.empty())
			{
				m_package.name = recordDeclaration->getNameAsString();
				auto& attributesTable = packageAttr->second.as_table();
				auto versionIt = attributesTable.find("version");
				m_package.version = versionIt != attributesTable.end() && versionIt->second.is_string() ? versionIt->second.as_string() : std::string("Undefined");
				auto descriptionIt = attributesTable.find("description");
				m_package.description = descriptionIt != attributesTable.end() && descriptionIt->second.is_string() ? descriptionIt->second.as_string() : std::string("Undefined");
			}
			else if (m_package.name != recordDeclaration->getNameAsString())
			{
				Logger::Error("Package has already been defined");
			}
			return;
		}

		if (!classAttr)
			return; // Only keep classes with cct::Class

		Class cls;
		cls.name = recordDeclaration->getNameAsString();
		cls.scope = "cct::Class";
		cls.tomlAttributes = GetAttributesOr(classAttr->second, {});

		// Base class (first base)
		if (recordDeclaration->getNumBases() > 0)
		{
			const auto& base = *recordDeclaration->bases_begin();
			cls.base = QualTypeToString(base.getType(), *m_astContext);
		}

		// Fields with [[cct::Member(...)]]
		for (const auto* F : recordDeclaration->fields())
		{
			bool hasAttr = false;
			TomlAttributes memberAttrs;
			for (const auto* A : F->attrs())
			{
				std::string scope; TomlAttributes attrs;
				if (!ExtractCctAttribute(A, *m_astContext, *m_sourceManager, *m_langOptions, scope, attrs))
					continue;
				if (scope == "cct::Member")
				{
					hasAttr = true;
					memberAttrs = GetAttributesOr(attrs, {});
					break;
				}
			}
			if (!hasAttr) continue;

			Class::Member m;
			m.name = F->getNameAsString();
			m.type = QualTypeToString(F->getType(), *m_astContext);
			m.tomlAttributes = memberAttrs;
			cls.members.push_back(std::move(m));
		}

		// Methods with [[cct::Method(...)]]
		for (const auto* M : recordDeclaration->methods())
		{
			bool hasAttr = false;
			TomlAttributes methodAttrs;
			for (const auto* A : M->attrs())
			{
				std::string scope; TomlAttributes attrs;
				if (!ExtractCctAttribute(A, *m_astContext, *m_sourceManager, *m_langOptions, scope, attrs))
					continue;
				if (scope == "cct::Method")
				{
					hasAttr = true;
					methodAttrs = GetAttributesOr(attrs, {});
					break;
				}
			}
			if (!hasAttr) continue;

			Class::Method mm;
			mm.name = M->getNameAsString();
			mm.returnValue = QualTypeToString(M->getReturnType(), *m_astContext);
			mm.tomlAttributes = methodAttrs;

			unsigned pi = 0;
			for (const auto* P : M->parameters())
			{
				Class::Method::Params param;
				param.type = QualTypeToString(P->getType(), *m_astContext);
				if (P->getIdentifier()) param.name = P->getNameAsString();
				else { param.name = std::string("arg") + std::to_string(pi); }
				mm.params.push_back(std::move(param));
				++pi;
			}
			cls.methods.push_back(std::move(mm));
		}

		std::vector<std::string> nsChain = GetNamespaceChain(recordDeclaration->getDeclContext());
		InsertClassIntoPackage(m_package, nsChain, cls);
	}

	void ClangParser::ProcessEnum(const EnumDecl* enumDeclaration)
	{
		if (!enumDeclaration || !enumDeclaration->isThisDeclarationADefinition())
			return;
		bool hasAttr = false;
		TomlAttributes enumAttrs;
		for (const auto* A : enumDeclaration->attrs())
		{
			std::string scope; TomlAttributes attrs;
			if (!ExtractCctAttribute(A, *m_astContext, *m_sourceManager, *m_langOptions, scope, attrs))
				continue;
			if (scope == "cct::Enum")
			{
				hasAttr = true;
				enumAttrs = GetAttributesOr(attrs, {});
				break;
			}
		}
		if (!hasAttr) return;

		Enum enm;
		enm.name = enumDeclaration->getNameAsString();
		if (const TypeSourceInfo* TSI = enumDeclaration->getIntegerTypeSourceInfo())
			enm.base = QualTypeToString(TSI->getType(), *m_astContext);

		for (const auto* E : enumDeclaration->enumerators())
		{
			Enum::Element elem;
			elem.name = E->getNameAsString();
			if (E->getInitExpr())
			{
				llvm::APSInt val = E->getInitVal();
				llvm::SmallString<32> s;
				val.toString(s, /*Radix=*/10);
				elem.value.assign(s.begin(), s.end());
			}
			enm.elements.push_back(std::move(elem));
		}

		std::vector<std::string> nsChain = GetNamespaceChain(enumDeclaration->getDeclContext());
		InsertEnumIntoPackage(m_package, nsChain, enm);
	}

	void ClangParser::ProcessNamespace(const NamespaceDecl* namespaceDeclaration)
	{
		for (const auto* D : namespaceDeclaration->decls())
			ProcessDeclaration(D);
	}

	void ClangParser::ProcessDeclaration(const Decl* declaration)
	{
		if (const auto* RD = llvm::dyn_cast<CXXRecordDecl>(declaration))
		{
			ProcessRecord(RD);
		}
		else if (const auto* ED = llvm::dyn_cast<EnumDecl>(declaration))
		{
			ProcessEnum(ED);
		}
		else if (const auto* ND = llvm::dyn_cast<NamespaceDecl>(declaration))
		{
			auto nsName = ND->getNameAsString();
			// Visit nested declarations inside namespaces
			for (const auto* SD : ND->decls())
			{
				if (const auto* RD2 = llvm::dyn_cast<CXXRecordDecl>(SD))
					ProcessRecord(RD2);
				else if (const auto* ED2 = llvm::dyn_cast<EnumDecl>(SD))
					ProcessEnum(ED2);
				else if (const auto* ND = llvm::dyn_cast<NamespaceDecl>(declaration))
					ProcessNamespace(ND);
			}
		}
	}
}