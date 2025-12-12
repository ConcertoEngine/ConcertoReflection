#include <iostream>
#include <filesystem>
#include <cstring>
#include <string>
#include <thread>

#include <cxxopts.hpp>
#include <Concerto/Core/Logger/Logger.hpp>

#include "Concerto/PackageGenerator/CppGenerator/CppGenerator.hpp"
#include "Concerto/PackageGenerator/HeaderGenerator/HeaderGenerator.hpp"
#include "Concerto/PackageGenerator/ClangParser/ClangParser.hpp"

int main(int argc, const char** argv)
{
	std::string outputFolder;
	std::vector<std::string> includes;
	std::vector<std::string> defines;
	std::vector<std::string> sources;
	std::vector<std::string> headers;
	std::string resourceDir;
	std::string sdk;

	try {
		cxxopts::Options options("Concerto Reflection Generator",
			"Concerto Reflection Generator");

		options.add_options()
			("outputDir", "Output directory", cxxopts::value<std::string>())
			("I,include", "Include directories (one or more)", cxxopts::value<std::vector<std::string>>())
			("D,define", "Preprocessor defines (one or more)", cxxopts::value<std::vector<std::string>>())
			("s,source", "Source files (one or more)", cxxopts::value<std::vector<std::string>>())
			("H,header", "Header file to include in the generated cpp", cxxopts::value<std::vector<std::string>>())
			("R,resource-dir", "Clang resource directory", cxxopts::value<std::string>())
			("S,sdk", "SDK path", cxxopts::value<std::string>())
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
		if (result.count("header"))
			headers = result["header"].as<std::vector<std::string>>();
		if (result.count("resource-dir"))
			resourceDir = result["resource-dir"].as<std::string>();
		if (result.count("sdk"))
			sdk = result["sdk"].as<std::string>();

		//std::this_thread::sleep_for(std::chrono::seconds(5));
		cct::ClangParser parser;
		Package* package = parser.Parse(includes, defines, sources, resourceDir, sdk);
		if (!package)
			return EXIT_FAILURE;

		std::filesystem::path file(outputFolder);
		std::filesystem::create_directories(file);

		cct::HeaderGenerator headerGenerator((file / (package->name + "Package.gen.hpp")).string());
		headerGenerator.Generate(*package, headers);
		cct::CppGenerator cppGenerator((file / (package->name + "Package.gen.cpp")).string());
		cppGenerator.Generate(*package, headers);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << "\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
