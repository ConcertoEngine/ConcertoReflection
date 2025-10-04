//
// Created by arthur on 09/12/2024.
//

#include "Concerto/PackageGenerator/FileGenerator/FileGenerator.hpp"

#include <filesystem>
#include <sstream>
#include <Concerto/Core/Logger/Logger.hpp>

namespace cct
{
	FileGenerator::FileGenerator(const std::string& path) :
		m_indentLevel(0),
		m_stream(),
		m_path(path)
	{
	}

	namespace
	{
		static std::string NormalizeNewlines(std::string s)
		{
			// Convert CRLF to LF to allow platform-agnostic comparisons
			std::string out;
			out.reserve(s.size());
			for (std::size_t i = 0; i < s.size(); ++i)
			{
				char c = s[i];
				if (c == '\r')
				{
					// skip, will be normalized by following '\n' or ignored if standalone
					continue;
				}
				out.push_back(c);
			}
			return out;
		}
	}

	FileGenerator::~FileGenerator()
	{
		try
		{
			std::string generated = m_stream.str();
			std::string existing;
			{
				std::ifstream in(m_path, std::ios::in | std::ios::binary);
				if (in)
				{
					std::ostringstream buf;
					buf << in.rdbuf();
					existing = buf.str();
				}
			}
			if (NormalizeNewlines(existing) == NormalizeNewlines(generated))
				return;

			std::ofstream out(m_path, std::ios::out | std::ios::trunc);
			if (out)
			{
				out << generated;
			}
		}
		catch (const std::exception& e)
		{
			cct::Logger::Error("Failed to write generated file {}: {}", m_path, e.what());
		}
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
