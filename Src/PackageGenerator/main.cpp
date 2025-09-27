#include <iostream>
#include <filesystem>
#include <cstring>
#include <thread>

#include <cxxopts.hpp>
#include <Concerto/Core/Logger.hpp>

#include "Concerto/PackageGenerator/CppGenerator.hpp"
#include "Concerto/PackageGenerator/HeaderGenerator.hpp"
#include "Concerto/PackageGenerator/ClangParser.hpp"


void PrintHelp()
{
	std::cout << "Usage: concerto-pkg-generator target_name [file_list] [-- <clang-args>]";
}

int main(int argc, const char** argv)
{
	std::string outputFolder;
	std::vector<std::string> includes;
	std::vector<std::string> defines;
	std::vector<std::string> sources;

	try {
		cxxopts::Options options("Concerto Reflection Generator",
			"Concerto Reflection Generator");

		options.add_options()
			("outputDir", "Output directory", cxxopts::value<std::string>())
			("I,include", "Include directories (one or more)", cxxopts::value<std::vector<std::string>>())
			("D,define", "Preprocessor defines (one or more)", cxxopts::value<std::vector<std::string>>())
			("s,source", "Source files (one or more)", cxxopts::value<std::vector<std::string>>())
			("v,version", "1.0.0")
			("h,help", "Show help");

		options.parse_positional({ "outputDir" });

		auto result = options.parse(argc, argv);

		if (result.count("help"))
		{
			std::cout << options.help({ "" }) << "\n";
			return 0;
		}

		if (result.count("version"))
		{
			std::cout << "1.0.0\n";
			return 0;
		}

		if (!result.count("outputDir")) {
			std::cerr << "Missing required 'outputDir' argument.\n";
			std::cerr << options.help({ "" }) << "\n";
			return EXIT_FAILURE;
		}
		outputFolder = result["outputDir"].as<std::string>();

		if (result.count("include"))
			includes = result["include"].as<std::vector<std::string>>();
		if (result.count("define"))
			defines = result["define"].as<std::vector<std::string>>();
		if (result.count("source"))
			sources = result["source"].as<std::vector<std::string>>();


		//std::this_thread::sleep_for(std::chrono::seconds(10));

		Package package = cct::ClangParser().Parse(includes, defines, sources);
		std::filesystem::path file(outputFolder);
		std::filesystem::create_directories(file);

		cct::HeaderGenerator headerGenerator((file / (package.name + "Package.gen.hpp")).string());
		headerGenerator.Generate(package, {});
		cct::CppGenerator cppGenerator((file / (package.name + "Package.gen.cpp")).string());
		cppGenerator.Generate(package, {});

	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << "\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
