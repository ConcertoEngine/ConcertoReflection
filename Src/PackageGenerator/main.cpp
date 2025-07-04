#include <iostream>
#include <filesystem>
#include <cstring>
#include <thread>


#include "Concerto/PackageGenerator/CppGenerator.hpp"
#include "Concerto/PackageGenerator/HeaderGenerator.hpp"
#include "Concerto/PackageGenerator/Lexer.hpp"
#include "Concerto/PackageGenerator/Parser.hpp"

void PrintHelp()
{
	std::cout << "Usage: ./concerto-pkg-generator target_name [file_list]";
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

	std::vector<std::string_view> args;
	args.reserve(argc - 2);

	//std::this_thread::sleep_for(std::chrono::seconds(10));
	std::vector<Token> tokens;
	tokens.reserve(0xFFFF);
	for (int i = 2; i < argc; ++i)
	{
		std::ifstream t(argv[i]);
		if (!t.is_open())
			continue;
		std::stringstream buffer;
		buffer << t.rdbuf();

		Lexer lexer(buffer.str());
		auto fileTokens = lexer.tokenize();
		tokens.insert(tokens.end(), fileTokens.begin(), fileTokens.end());
		std::string_view str(argv[i], std::strlen(argv[i]));
		auto pos = str.find('\\');
		if (pos != std::string_view::npos)
			str.remove_prefix(pos + 1);
		args.emplace_back(str);
	}
	
	Parser parser(std::move(tokens));
	auto package = parser.ParsePackage();
	try
	{
		std::filesystem::path file(argv[1]);
		std::filesystem::create_directories(file.parent_path());

		cct::HeaderGenerator headerGenerator(file.string() + "Package.hpp");
		headerGenerator.Generate(package, args);
		cct::CppGenerator cppGenerator(file.string() + "Package.cpp");
		cppGenerator.Generate(package, args);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 84;
	}

	return EXIT_SUCCESS;
}