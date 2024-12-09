#include <iostream>
#include <format>

#include <pugixml.hpp>

#include "Concerto/PackageGenerator/Parser.hpp"


void PrintHelp()
{
	std::cout << "Usage: ./concerto-pkg-generator input.xml outputDir";
}


int main(int argc, const char** argv)
{
	//if (argc < 3)
	//{
	//	PrintHelp();
	//	return EXIT_FAILURE;
	//}

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
		std::cout << result.GetError() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}