//
// Created by arthur on 09/12/2024.
//

#include <cstring>
#include <format>
#include <iostream>
#include <filesystem>

#include <Concerto/Core/Assert.hpp>
#include <cppast/visitor.hpp>
#include <toml.hpp>

#include "Concerto/PackageGenerator/Parser.hpp"

#include <ranges>


#define CCT_TRY_DECLARE(varName, func, param)				\
	auto varName##_result = func(param);					\
	if (varName##_result.IsError())							\
	{														\
		CCT_ASSERT_FALSE("");								\
		return std::move(varName##_result).GetError();		\
	}														\
	auto varName = std::move(varName##_result).GetValue();


#define CCT_TRY_GET_ATTRIBUTE(attributes, attributeName, attributeNameStr, type, parentAttribute)						\
	auto attributeName##_optional = toml::find<std::optional<type>>(attributes, std::string(attributeNameStr));			\
	if (!(attributeName##_optional))																					\
		return std::format("Missing attribute '{}' in {}::{}", attributeNameStr, BaseAttributeName, parentAttribute);	\
	auto attributeName = std::move(attributeName##_optional).value()

#define CCT_TRY_GET_ATTRIBUTE_OR_DEFAULT(attributes, attributeName, attributeNameStr, type, parentAttribute, defaultV)	\
	auto attributeName##_optional = toml::find<std::optional<type>>(attributes, std::string(attributeNameStr));			\
	auto attributeName = defaultV;																						\
	if (attributeName##_optional)																						\
		attributeName = std::move(attributeName##_optional).value()

//#define CCT_MAKE_SV(var) std::string_view(var, std::strlen(var))
using namespace std::string_view_literals;

namespace
{
	std::string FindSourceFileForHeader(const std::vector<std::string>& files, const std::filesystem::path& header)
	{
		for (auto& f : files)
		{
			std::filesystem::path p(f);

			if (p.filename().replace_extension() == header.filename().replace_extension())
				return f;
		}
		return {};
	}

	//template<typename T>
	//T GetAndRemoveAttributeOrDefault(Attributes& attributes, const std::string& name, T defaultValue)
	//{
	//	auto it = attributes.find(name);
	//	if (it != attributes.end())
	//	{
	//		T attrib;
	//		if constexpr (std::is_same_v<bool, T>)
	//			attrib = it->second == "true"sv;
	//		else
	//			 attrib = T(it->second);
	//		attributes.erase(it);
	//		return attrib;
	//	}

	//	return T(defaultValue);
	//}

	std::string GetFullType(const cppast::cpp_type& type)
	{
		std::string fullTpe;

		switch (type.kind())
		{
		case cppast::cpp_type_kind::builtin_t:
		{
			const auto& builtin = static_cast<const cppast::cpp_builtin_type&>(type);
			fullTpe = cppast::to_string(builtin);
			break;
		}
		case cppast::cpp_type_kind::user_defined_t:
		{
			const auto& userDefined = static_cast<const cppast::cpp_user_defined_type&>(type);
			fullTpe = userDefined.entity().name();
			break;
		}
		case cppast::cpp_type_kind::auto_t:
		case cppast::cpp_type_kind::decltype_t:
		case cppast::cpp_type_kind::decltype_auto_t:
			break;
		case cppast::cpp_type_kind::cv_qualified_t:
		{
			auto& cvQualified = static_cast<const cppast::cpp_cv_qualified_type&>(type);
			fullTpe = GetFullType(cvQualified.type());
			break;
		}
		case cppast::cpp_type_kind::pointer_t:
		{
			auto& pointer = static_cast<const cppast::cpp_pointer_type&>(type);
			fullTpe = GetFullType(pointer.pointee());
			break;
		}
		case cppast::cpp_type_kind::reference_t:
		{
			const auto& reference = static_cast<const cppast::cpp_reference_type&>(type);
			fullTpe = GetFullType(reference.referee());
			break;
		}
		case cppast::cpp_type_kind::array_t:
		case cppast::cpp_type_kind::function_t:
		case cppast::cpp_type_kind::member_function_t:
		case cppast::cpp_type_kind::member_object_t:
		case cppast::cpp_type_kind::template_parameter_t:
		case cppast::cpp_type_kind::template_instantiation_t:
		case cppast::cpp_type_kind::dependent_t:
		case cppast::cpp_type_kind::unexposed_t:
			break;
		}
		return fullTpe;
	}
}
namespace cct
{
//	using namespace std::string_view_literals;
//
	Result<Package, std::string> Parser::TryParse(const cppast::libclang_compilation_database& database, cppast::stderr_diagnostic_logger& logger)
	{
		std::vector<std::string> databaseFiles;
		cppast::detail::for_each_file(database, &databaseFiles, [](void* data, std::string path)
		{
			auto* files = static_cast<std::vector<std::string>*>(data);
			if (!files)
				return;
			files->push_back(std::move(path));
		});

		Package package = {};
		for (auto& headerFile : std::filesystem::recursive_directory_iterator("C:/Users/Arthur/Documents/Git/ConcertoEngine/ConcertoReflection/Tests"))
		{
			if (!headerFile.is_regular_file() || headerFile.path().extension() != ".hpp")
				continue;
			std::string path = headerFile.path().string();
			cppast::cpp_entity_index index;
			auto src = FindSourceFileForHeader(databaseFiles, headerFile.path());
			if (src.empty())
			{
				std::cerr << "Could not find corresponding source file for " + path + '\n';
				continue;
			}
			cppast::libclang_compile_config config(database, src);
			config.write_preprocessed(true);
			cppast::libclang_parser parser(type_safe::ref(logger));
			auto file = parser.parse(index, path, config);
			if (parser.error())
			{
				std::cerr << "Could not parse file " + path << '\n';
				continue;
			}
			if (!file)
				continue;

			std::vector<Namespace> namespaces;
			Namespace* currentNameSpace = nullptr;
			std::unique_ptr<Class> currentClass = {};
			std::string currentError;
			cppast::visit(*file, [&](const cppast::cpp_entity& e, cppast::visitor_info info)
			{
				if (info.event == cppast::visitor_info::container_entity_enter)
				{
					switch (e.kind())
					{
					case cppast::cpp_entity_kind::file_t:
						return true;
					case cppast::cpp_entity_kind::class_t:
					{
						const auto& classType = static_cast<const cppast::cpp_class&>(e);
						if (!classType.is_definition())
							return true;
						if (classType.class_kind() == cppast::cpp_class_kind::struct_t)
						{
							if (!package.name.empty())
							{
								currentError = std::format("Package has been already defined before with name: '{}'", package.name);
								return false;
							}
							auto result = TryParsePackage(classType);
							if (!result)
							{
								std::string err = std::move(result).GetError();
								if (err == NotApplicable)
									break;
								currentError = std::move(err);
								return false;
							}
							package = result.GetValue();
							break;
						}
						auto res = TryParseClass(classType);
						if (!res)
						{
							currentError = std::move(res).GetError();
							return false;
						}
						currentClass = std::make_unique<Class>(std::move(res).GetValue());
						break;
					}
					case cppast::cpp_entity_kind::enum_t:
					{
						const auto& enumType = static_cast<const cppast::cpp_enum&>(e);
						if (!enumType.is_definition())
							return true;
						auto res = TryParseEnum(enumType);
						if (!res)
						{
							currentError = std::move(res).GetError();
							return false;
						}
						currentNameSpace->enums.push_back(std::move(res).GetValue());
						break;
					}
					case cppast::cpp_entity_kind::namespace_t:
					{
						const auto& nsType = static_cast<const cppast::cpp_namespace&>(e);
						auto res = TryParseNamespace(nsType);
						if (!res)
						{
							currentError = std::move(res).GetError();
							return false;
						}

						if (currentNameSpace)
						{
							currentNameSpace->namespaces.push_back(std::move(res).GetValue());
							currentNameSpace = &currentNameSpace->namespaces.back();
						}
						else
						{
							namespaces.push_back(std::move(res).GetValue());
							currentNameSpace = &namespaces.back();
						}
						break;
					}
					default:
						break;
					}
				}
				else if (info.event == cppast::visitor_info::leaf_entity)
				{
					switch (e.kind())
					{
					case cppast::cpp_entity_kind::member_function_t:
					{
						const auto& functionType = static_cast<const cppast::cpp_member_function&>(e);
						auto result = TryParseClassMethod(functionType);
						if (result.IsError())
						{
							std::string err = std::move(result).GetError();
							if (err == NotApplicable)
								break;
							currentError = std::move(result).GetError();
							return false;
						}
						currentClass->methods.push_back(std::move(result).GetValue());
						break;
					}
					case cppast::cpp_entity_kind::member_variable_t:
					{
						const auto& memberVariable = static_cast<const cppast::cpp_member_variable&>(e);
						auto result = TryParseClassMember(memberVariable);
						if (!result)
						{
							std::string err = std::move(result).GetError();
							if (err == NotApplicable)
								break;
							currentError = std::move(err);
							return false;
						}
						currentClass->members.push_back(std::move(result).GetValue());
						break;
					}
					default:
						break;
					}
				}
				else if (info.event == cppast::visitor_info::container_entity_exit)
				{
					switch (e.kind())
					{
					case cppast::cpp_entity_kind::namespace_t:
					{
						currentNameSpace = nullptr;
						break;
					}
					case cppast::cpp_entity_kind::class_t:
					{
						if (!currentClass)
							break;
						currentNameSpace->classes.push_back(*currentClass);
						currentClass = nullptr;
						break;
					}
					default:
						break;
					}
				}
				return true;
			});
			if (!currentError.empty())
				return std::move(currentError);
			package.namepsaces = std::move(namespaces);
		}
		return std::move(package);
	}

	Result<Package, std::string> Parser::TryParsePackage(const cppast::cpp_class& klass)
	{
		if (!IsValidEntity(klass, PackageAttributeName))
			return std::string(NotApplicable);
		CCT_TRY_DECLARE(attributes, GetAttributes, klass.attributes());
		
		if (!attributes.is_table())
			return std::format("Invalid attributes found for {}::{}", BaseAttributeName, PackageAttributeName);

		CCT_TRY_GET_ATTRIBUTE(attributes, version, VersionAttributeName, std::string, PackageAttributeName);
		CCT_TRY_GET_ATTRIBUTE(attributes, description, DescriptionAttributeName, std::string, PackageAttributeName);

		if (version.empty())
			return std::format("Attribute '{}' must not be an empty string in {}::{}", VersionAttributeName, BaseAttributeName, PackageAttributeName);
		if (description.empty())
			return std::format("Attribute '{}' must not be an empty string in {}::{}", DescriptionAttributeName, BaseAttributeName, PackageAttributeName);

		Package package = {};
		package.name = klass.name();
		package.version = std::move(version);
		package.description = std::move(description);
		return package;
	}

	Result<Namespace, std::string> Parser::TryParseNamespace(const cppast::cpp_namespace& nameSpace)
	{
		Namespace ns = {};
		ns.name = nameSpace.name();

		return ns;
	}

	Result<Enum, std::string> Parser::TryParseEnum(const cppast::cpp_enum& enumeration)
	{
		if (!IsValidEntity(enumeration, EnumAttributeName))
			return std::string(NotApplicable);

		CCT_TRY_DECLARE(attributes, GetAttributes, enumeration.attributes())

		Enum enumA = {
			.name = enumeration.name(),
			.base = GetFullType(enumeration.underlying_type()),
			.elements = {},
			.attributes = GetAttributes(attributes),
			.tomlAttributes = std::move(attributes)
		};

		int i = 0;
		for (const auto& enumValue : enumeration)
		{
			CCT_TRY_DECLARE(enumValueAttributes, GetAttributes, enumValue.attributes())

			Enum::Element enumElement = {
				.name = enumValue.name(),
				.value = std::to_string(i),
				.attributes = GetAttributes(enumValueAttributes),
				.tomlAttributes = std::move(enumValueAttributes)
			};

			enumA.elements.push_back(std::move(enumElement));
			++i;
		}
		return std::move(enumA);
	}

	Result<Class, std::string> Parser::TryParseClass(const cppast::cpp_class& cppClass)
	{
		if (cppClass.class_kind() != cppast::cpp_class_kind::class_t)
			return std::string(NotApplicable);
		if (!IsValidEntity(cppClass, ClassAttributeName))
			return std::string(NotApplicable);

		std::string base;
		for (auto& b : cppClass.bases())
		{
			base = b.name(); // Fixme the base class must always be in the first position
			break;
		}

		CCT_TRY_DECLARE(attributes, GetAttributes, cppClass.attributes())

		Class klass = {
			.name = cppClass.name(),
			.base = std::move(base),
			.methods = {},
			.members = {},
			.attributes = GetAttributes(attributes),
			.tomlAttributes = std::move(attributes)
		};
		return std::move(klass);
	}

	Result<Class::Member, std::string> Parser::TryParseClassMember(const cppast::cpp_member_variable& memberVariable)
	{
		if (IsValidEntity(memberVariable, MemberAttributeName))
			return std::string(NotApplicable);
		CCT_TRY_DECLARE(attributes, GetAttributes, memberVariable.attributes())

		return Class::Member{
			.name = memberVariable.name(),
			.type = GetFullType(memberVariable.type()),
			.attributes = GetAttributes(attributes),
			.tomlAttributes = std::move(attributes)
		};
	}

	Result<Class::Method, std::string> Parser::TryParseClassMethod(const cppast::cpp_member_function& function)
	{
		if (!IsValidEntity(function, MethodAttributeName))
			return std::string(NotApplicable);

		CCT_TRY_DECLARE(attributes, GetAttributes, function.attributes())

		std::vector<Class::Method::Params> params;
		for (const auto& funcParam: function.parameters())
		{
			CCT_TRY_DECLARE(paramAttributes, GetAttributes, funcParam.attributes())

			auto& param = params.emplace_back();
			param.name = funcParam.name();
			param.type = GetFullType(funcParam.type());
			param.attributes = GetAttributes(paramAttributes);
			param.tomlAttributes = std::move(paramAttributes);
		}

		CCT_TRY_GET_ATTRIBUTE_OR_DEFAULT(attributes, base, ClassBaseAttr, std::string, ClassAttributeName, "cct::refl::Method"s);
		CCT_TRY_GET_ATTRIBUTE_OR_DEFAULT(attributes, customInvoker, ClassBaseAttr, bool, ClassAttributeName, false);

		Class::Method method;
		method.base = std::move(base);
		method.name = function.name();
		method.returnValue = GetFullType(function.return_type());
		method.params = std::move(params);
		method.customInvoker = customInvoker;
		method.attributes = GetAttributes(method.attributes);
		method.tomlAttributes = std::move(attributes);
		return method;
	}

	Result<TomlAttributes, std::string> Parser::GetAttributes(const cppast::cpp_attribute_list& cppAttributes)
	{
		for (auto child : cppAttributes)
		{
			if (child.scope() != AttributeScopeElement)
				continue;
			const type_safe::optional<cppast::cpp_token_string>& arg = child.arguments();
			if (!arg)
				continue;
			const auto &tokens= arg.value();
			std::string res;

			bool isInsideBrackets = false;
			for (const auto& token : tokens)
			{
				if (token.kind == cppast::cpp_token_kind::punctuation && (token.spelling == "{" || token.spelling == "["))
					isInsideBrackets = true;
				if (token.kind == cppast::cpp_token_kind::punctuation && (token.spelling == "}" || token.spelling == "]"))
					isInsideBrackets = false;
				if (isInsideBrackets)
				{
					res += token.spelling;
				}
				else
				{
					if (token.kind == cppast::cpp_token_kind::punctuation && token.spelling == ",")
						res += '\n';
					else res += token.spelling; 
				}
			}
			auto result = toml::try_parse_str(res);
			if (result.is_err())
				return std::string(result.as_err()[0].title());
			return TomlAttributes(result.as_ok());
		}
		return {};
	}


	bool Parser::IsValidEntity(const cppast::cpp_entity& e, std::string_view expected)
	{
		if (e.attributes().empty())
			return false;
		for (auto& attrib : e.attributes())
		{
			if (attrib.scope() == BaseAttributeName && attrib.name() == expected)
				return true;
		}
		return false;
	}

	Attributes Parser::GetAttributes(TomlAttributes attributes)
	{
		try
		{
			return toml::get<Attributes>(attributes.at(std::string(AttributesAttr)));
		}
		catch (...)
		{
			return {};
		}
	}

}
