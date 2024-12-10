//
// Created by arthur on 09/12/2024.
//

#ifndef CONCERTO_PKGGENERATOR_FILEGENERATOR_HPP
#define CONCERTO_PKGGENERATOR_FILEGENERATOR_HPP

#include <fstream>
#include <string>
#include <format>
#include <ranges>

#include "Concerto/PackageGenerator/Defines.hpp"

namespace cct
{
	class FileGenerator
	{
	public:
		explicit FileGenerator(const std::string& path);

		virtual bool Generate(const Package& package) = 0;

		void EnterScope();
		void LeaveScope(std::string_view str = "");

		void NewLine();

		static std::string Capitalize(std::string_view str);

		template<typename... T>
		void Write(const std::format_string<T...> fmt, T&&... args)
		{
			using namespace std::string_view_literals;
			std::string data = std::vformat(fmt.get(), std::make_format_args(args...));
			auto lines = data | std::ranges::views::split("\n"sv);

			for (auto line : lines)
			{
				stream << std::string(_indentLevel, '\t');
				stream << std::string_view(line.data(), line.size());
			}
			stream << '\n';
		}
	private:
		std::size_t _indentLevel;
	protected:
		std::ofstream stream;
	};
}

#endif //CONCERTO_PKGGENERATOR_FILEGENERATOR_HPP
