//
// Created by arthur on 09/12/2024.
//

#ifndef CONCERTO_PKGGENERATOR_CPPGENERATOR_HPP
#define CONCERTO_PKGGENERATOR_CPPGENERATOR_HPP

#include "FileGenerator.hpp"

namespace cct
{
	class CppGenerator : public FileGenerator
	{
	public:
		using FileGenerator::FileGenerator;
		bool Generate(const Package& package) override;
	private:
		void GenerateNamespace(const Namespace& ns, const std::string& namespaceChain = "");
		void GenerateClass(std::string_view ns, const Class& klass);
		void GenerateClassMethod(std::string_view className, const Class::Method& method);
		void GenerateEnum(const Enum& enum_);
		void GeneratePackage(const Package& pkg);
	};
}

#endif //CONCERTO_PKGGENERATOR_CPPGENERATOR_HPP
