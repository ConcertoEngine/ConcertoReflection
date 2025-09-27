//
// Clang LibTooling based parser for Concerto Package Generator
//

#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <clang/AST/DeclCXX.h>
#include <clang/Frontend/ASTUnit.h>

#include "Concerto/PackageGenerator/Defines.hpp"

namespace cct
{
#ifdef CCT_WITH_CLANG_TOOLING
	class ClangParser
	{
	public:
		Package Parse(const std::vector<std::string>& includeDirs,
			const std::vector<std::string>& defines,
			const std::vector<std::string>& sources);

		void ProcessDeclaration(Namespace& currentNamespace, const clang::Decl* declaration);
		void ProcessNamespace(Namespace& currentNamespace, const clang::NamespaceDecl* namespaceDeclaration);
		void ProcessRecord(Namespace& currentNamespace, const clang::CXXRecordDecl* recordDeclaration);
		void ProcessEnum(Namespace& currentNamespace, const clang::EnumDecl* enumDeclaration);

	private:
		Package m_package;
		clang::SourceManager* m_sourceManager = nullptr;
		const clang::LangOptions* m_langOptions = nullptr;
		clang::ASTContext* m_astContext = nullptr;
	};
#endif // CCT_WITH_CLANG_TOOLING
}

