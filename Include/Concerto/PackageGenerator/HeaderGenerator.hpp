//
// Created by arthur on 09/12/2024.
//

#ifndef CONCERTO_PKGGENERATOR_HEADERGENERATOR_HPP
#define CONCERTO_PKGGENERATOR_HEADERGENERATOR_HPP


#include "Concerto/PackageGenerator/FileGenerator.hpp"

namespace cct
{
	class HeaderGenerator : public FileGenerator
	{
	public:
		using FileGenerator::FileGenerator;
		bool Generate(const Package& package) override;
	private:
		void GenerateNamespace(const Namespace& ns, const std::string& api);
		void GenerateClass(const Class& klass, const std::string& api);
		void GenerateEnum(const Enum& enum_, const std::string& api);
	};
}

#endif //CONCERTO_PKGGENERATOR_HEADERGENERATOR_HPP
