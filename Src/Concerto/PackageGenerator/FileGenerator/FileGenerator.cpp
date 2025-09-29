//
// Created by arthur on 09/12/2024.
//

#include "Concerto/PackageGenerator/FileGenerator/FileGenerator.hpp"

#include <iostream>

namespace cct
{
	FileGenerator::FileGenerator(const std::string& path) :
		m_indentLevel(0),
		m_stream(path, std::ios::trunc)
	{
		std::cout << "Generating file: " << path << '\n';
	}

	void FileGenerator::EnterScope()
	{
		Write("{{");
		++m_indentLevel;
	}

	void FileGenerator::LeaveScope(std::string_view str)
	{
		--m_indentLevel;
		Write("}}{}", str);
	}

	void FileGenerator::NewLine()
	{
		m_stream << '\n';
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
