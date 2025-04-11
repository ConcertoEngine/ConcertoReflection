//
// Created by arthur on 09/12/2024.
//

#ifndef CONCERTO_PKGGENERATOR_DEFINE_HPP
#define CONCERTO_PKGGENERATOR_DEFINE_HPP

#include <string_view>
#include <vector>
#include <unordered_map>


#include <Concerto/Core/Types.hpp>
#include <toml11/types.hpp>

#ifdef CCT_PKGGENERATOR_BUILD
#define CCT_PKGGENERATOR_API CCT_EXPORT
#else
#define CCT_PKGGENERATOR_API CCT_IMPORT
#endif // CCT_PKGGENERATOR_BUILD

using TomlAttributes = toml::basic_value<toml::type_config>;
using Attributes = std::unordered_map<std::string_view /*name*/, std::string_view /*value*/>;

struct Enum
{
	struct Element
	{
		std::string name;
		std::string value;
		Attributes attributes;
		TomlAttributes tomlAttributes;
	};
	std::string name;
	std::string base;
	std::vector<Element> elements;
	Attributes attributes;
	TomlAttributes tomlAttributes;
};

struct Class
{
	struct Member
	{
		std::string name;
		std::string type;
		Attributes attributes;
		TomlAttributes tomlAttributes;
	};
	struct Method
	{
		struct Params
		{
			std::string name;
			std::string type;
			Attributes attributes;
			TomlAttributes tomlAttributes;
		};
		std::string base;
		std::string name;
		std::string returnValue;
		std::vector<Params> params;
		bool customInvoker;
		Attributes attributes;
		TomlAttributes tomlAttributes;
	};

	std::string name;
	std::string base;
	std::vector<Method> methods;
	std::vector<Member> members;
	Attributes attributes;
	TomlAttributes tomlAttributes;
};

struct Namespace
{
	std::string name;
	std::vector<Class> classes;
	std::vector<Enum> enums;
	std::vector<Namespace> namespaces;
};

struct Package
{
	std::string name;
	std::string version;
	std::string description;

	std::vector<Class> classes;
	std::vector<Namespace> namepsaces;
	std::vector<Enum> enums;
};

#endif //CONCERTO_PKGGENERATOR_DEFINE_HPP