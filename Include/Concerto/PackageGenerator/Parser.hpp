//
// Created by arthur on 09/12/2024.
//

#ifndef CONCERTO_PKGGENERATOR_PARSER_HPP
#define CONCERTO_PKGGENERATOR_PARSER_HPP

#include <string_view>
#include <Concerto/Core/Result.hpp>

#include <cppast/libclang_parser.hpp>
#include <cppast/cpp_namespace.hpp>
#include <cppast/cpp_enum.hpp>
#include <cppast/cpp_function.hpp>
#include <cppast/cpp_member_function.hpp>
#include <cppast/cpp_member_variable.hpp>
#include <cppast/cpp_token.hpp>
#include <cppast/cpp_attribute.hpp>

#include "Concerto/PackageGenerator/Defines.hpp"

namespace cct
{
	using namespace std::string_literals;
	using namespace std::string_view_literals;
	class Parser
	{
	public:
		static Result<Package, std::string> TryParse(const cppast::libclang_compilation_database& database, cppast::stderr_diagnostic_logger& logger);
		static Result<Package, std::string> TryParsePackage(const cppast::cpp_class& klass);
		static Result<Namespace, std::string> TryParseNamespace(const cppast::cpp_namespace&);
		static Result<Enum, std::string> TryParseEnum(const cppast::cpp_enum&);
		static Result<Class, std::string> TryParseClass(const cppast::cpp_class& klass);
		static Result<Class::Member, std::string> TryParseClassMember(const cppast::cpp_member_variable&);
		static Result<Class::Method, std::string> TryParseClassMethod(const cppast::cpp_member_function&);
		static Result<TomlAttributes, std::string> GetAttributes(const cppast::cpp_attribute_list& cppAttributes);
		static bool IsValidEntity(const cppast::cpp_entity& e, std::string_view expected);
		static Attributes GetAttributes(TomlAttributes attributes);
	private:
		static constexpr std::string_view BaseAttributeName = "cct"sv;
		static constexpr std::string_view ClassAttributeName = "Class"sv;
		static constexpr std::string_view MethodAttributeName = "Method"sv;
		static constexpr std::string_view PackageAttributeName = "Package"sv;
		static constexpr std::string_view MemberAttributeName = "Member"sv;
		static constexpr std::string_view EnumAttributeName = "Enum"sv;

		static constexpr std::string_view VersionAttributeName = "version"sv;
		static constexpr std::string_view DescriptionAttributeName = "description"sv;
		static constexpr std::string_view ClassBaseAttr = "base"sv;
		static constexpr std::string_view AttributesAttr = "attributes"sv;

		static constexpr std::string_view NotApplicable = "N/A"sv;
		static constexpr std::string_view AttributeScopeElement = "cct"sv;

	};
}

#endif // CONCERTO_PKGGENERATOR_PARSER_HPP