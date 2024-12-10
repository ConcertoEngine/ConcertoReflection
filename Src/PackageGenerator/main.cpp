#include <iostream>
#include <format>

#include <pugixml.hpp>

#include "Concerto/PackageGenerator/CppGenerator.hpp"
#include "Concerto/PackageGenerator/HeaderGenerator.hpp"
#include "Concerto/PackageGenerator/Parser.hpp"


void PrintHelp()
{
	std::cout << "Usage: ./concerto-pkg-generator input.xml outputDir";
}


int main(int argc, const char** argv)
{
	using namespace std::string_view_literals;
	if (argc == 2 && std::string_view(argv[1], std::strlen(argv[1])) == "--version"sv)
	{
		std::cout << "concerto-pkg-generator version 1.0.0";
		return EXIT_SUCCESS;
	}

	if (argc < 3)
	{
		PrintHelp();
		return EXIT_FAILURE;
	}

	pugi::xml_document doc;
	{
		pugi::xml_parse_result result = doc.load_file("E:/Documents/git/ConcertoEngine/Reflection/Tests/SamplePackage.xml");

		if (!result)
		{
			std::cerr << std::format("Invalid xml document: {}\n", result.description());
			return EXIT_FAILURE;
		}
	}

	auto result = cct::Parser::TryParse(doc);

	if (result.IsError())
	{
		std::cerr << result.GetError() << '\n';
		return EXIT_FAILURE;
	}

	cct::HeaderGenerator headerGenerator("./SamplePackage.hpp");
	headerGenerator.Generate(result.GetValue());
	cct::CppGenerator cppGenerator("./SamplePackage.cpp");
	cppGenerator.Generate(result.GetValue());

	return EXIT_SUCCESS;
}