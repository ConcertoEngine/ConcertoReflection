//
// Created by arthur on 09/12/2024.
//

#include "Concerto/PackageGenerator/FileGenerator.hpp"

namespace cct
{
	FileGenerator::FileGenerator(const std::string& path) :
		_indentLevel(0),
		stream(path)
	{
	}

	void FileGenerator::EnterScope()
	{
		Write("{{");
		++_indentLevel;
	}

	void FileGenerator::LeaveScope(std::string_view str)
	{
		--_indentLevel;
		Write("}}{}", str);
	}

	void FileGenerator::NewLine()
	{
		stream << '\n';
	}

	std::string FileGenerator::Capitalize(std::string_view str)
	{
		if (str.empty())
			return {};
		std::string s(str);
		s[0] = toupper(s[0]);
		return s;
	}
}
