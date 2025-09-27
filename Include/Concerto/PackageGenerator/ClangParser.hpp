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

		void ProcessDeclaration(const clang::Decl* declaration);
		void ProcessNamespace(const clang::NamespaceDecl* namespaceDeclaration);
		void ProcessRecord(const clang::CXXRecordDecl* recordDeclaration);
		void ProcessEnum(const clang::EnumDecl* enumDeclaration);

	private:
		Package m_package;
		clang::SourceManager* m_sourceManager = nullptr;
		const clang::LangOptions* m_langOptions = nullptr;
		clang::ASTContext* m_astContext = nullptr;
	};
#endif // CCT_WITH_CLANG_TOOLING
}

