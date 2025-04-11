#include <iostream>
#include <format>
#include <filesystem>
#include <cstring>
#include <iostream>
#include <Concerto/Core/Assert.hpp>

#include <cppast/libclang_parser.hpp>
#include <cppast/visitor.hpp>
#include <cppast/cpp_enum.hpp>
#include <cppast/cpp_token.hpp>

#include <Concerto/Core/Types.hpp>
#include <Concerto/Core/Logger.hpp>

#include "Concerto/PackageGenerator/CppGenerator.hpp"
#include "Concerto/PackageGenerator/HeaderGenerator.hpp"
#include "Concerto/PackageGenerator/Parser.hpp"


void PrintHelp()
{
	std::cout << "Usage: ./concerto-pkg-generator compile_commands outputDir";
}



int main(int argc, const char** argv)
{
	using namespace std::string_view_literals;
	if (argc == 2 && std::string_view(argv[1], std::strlen(argv[1])) == "--version"sv)
	{
		std::cout << "concerto-pkg-generator version 1.0.0";
		return EXIT_SUCCESS;
	}
	//if (argc < 3)
	//{
	//	PrintHelp();
	//	return EXIT_FAILURE;
	//}

	cppast::libclang_compilation_database database("C:/Users/Arthur/Documents/Git/ConcertoEngine/ConcertoReflection");

	cppast::stderr_diagnostic_logger logger;
	logger.set_verbose(true);
	auto result = cct::Parser::TryParse(database, logger);

	if (result.IsError())
	{
		std::cerr << result.GetError() << '\n';
		return EXIT_FAILURE;
	}
	try
	{
		std::filesystem::path file(argv[1]);
		file = file.filename().replace_extension();
		std::filesystem::path path = argv[2] / file.filename();
		path = std::filesystem::absolute(path);
		std::filesystem::create_directories(path.parent_path());

		cct::HeaderGenerator headerGenerator(path.string() + ".hpp");
		headerGenerator.Generate(result.GetValue());
		cct::CppGenerator cppGenerator(path.string() + ".cpp");
		cppGenerator.Generate(result.GetValue());
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 84;
	}

	return EXIT_SUCCESS;
}