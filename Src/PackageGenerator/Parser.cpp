//
// Created by arthur on 09/12/2024.
//
#include <cstring>
#include <format>
#include <iostream>

#include <Concerto/Core/Assert.hpp>

#include "Concerto/PackageGenerator/Parser.hpp"

#define CCT_TRY_DECLARE_ATTR(varName, node, attrName)													\
	const auto* varName = (node).attribute((attrName).c_str()).value();									\
	if ((varName) == nullptr)																			\
	{																									\
		CCT_ASSERT_FALSE("");																			\
		return std::format("Package {} does not have required attribute: '{}'", node.name(), attrName);	\
	}

#define CCT_TRY_DECLARE(varName, func, node)	\
	auto varName = func(node);					\
	if (varName.IsError())						\
	{											\
		CCT_ASSERT_FALSE("");					\
		return std::move(varName).GetError();	\
	}

#define CCT_MAKE_SV(var) std::string_view(var, std::strlen(var))

namespace cct
{
	using namespace std::string_view_literals;

	Result<Package, std::string> Parser::TryParse(const pugi::xml_document& doc)
	{
		auto packageElement = doc.child(PackageElement.c_str());
		if (!packageElement)
			return std::format("Xml document does not have root element {}", PackageElement);

		CCT_TRY_DECLARE_ATTR(packageName, packageElement, PackageNameAttrib);
		CCT_TRY_DECLARE_ATTR(packageDescription, packageElement, PackageDescriptionAttrib);
		CCT_TRY_DECLARE_ATTR(packageVersion, packageElement, PackageVersionAttrib);

		Package package = {
			.name = CCT_MAKE_SV(packageName),
			.version = CCT_MAKE_SV(packageDescription),
			.description = CCT_MAKE_SV(packageVersion),
			.includes = {},
			.classes = {},
			.namepsaces = {},
			.enums = {},
		};

		for (auto child : packageElement)
		{
			const auto* name = child.name();
			if (name == IncludeElement)
			{
				CCT_TRY_DECLARE(include, TryParseInclude, child);
				package.includes.push_back(std::move(include).GetValue());
			}
			else if (name == ClassElement)
			{
				CCT_TRY_DECLARE(klass, TryParseClass, child);
				package.classes.push_back(std::move(klass).GetValue());
			}
			else if (name == NamespaceElement)
			{
				CCT_TRY_DECLARE(nameSpace, TryParseNamespace, child);
				package.namepsaces.push_back(std::move(nameSpace).GetValue());
			}
			else if (name == EnumElement)
			{
				CCT_TRY_DECLARE(nameSpace, TryParseEnum, child);
				package.enums.push_back(std::move(nameSpace).GetValue());
			}
			else
			{
				return std::format("Unknown element '{}'", name);
			}
		}
		return std::move(package);
	}

	Result<Include, std::string> Parser::TryParseInclude(const pugi::xml_node& node)
	{
		if (!node.children().empty())
			return std::format("Include '{}' must not have child elements", node.name());

		CCT_TRY_DECLARE_ATTR(isPublic, node, IncludeIsPublicAttr);
		CCT_TRY_DECLARE_ATTR(file, node, IncludeFileAttr);

		if (isPublic != "true"sv && isPublic != "false"sv)
			return std::format("Include '{}' has an invalid {} attribute: '{}'", node.name(), IncludeIsPublicAttr, isPublic);
		
		return Include {
			.isPublic = isPublic == "true"sv,
			.file = CCT_MAKE_SV(file)
		};
	}

	Result<Namespace, std::string> Parser::TryParseNamespace(const pugi::xml_node& node)
	{
		CCT_TRY_DECLARE_ATTR(namespaceName, node, NamespaceNameAttr);

		Namespace ns = {};
		ns.name = CCT_MAKE_SV(namespaceName);

		for (auto child : node)
		{
			const auto* name = child.name();
			if (name == ClassElement)
			{
				CCT_TRY_DECLARE(klass, TryParseClass, child);
				ns.classes.push_back(std::move(klass).GetValue());
			}
			else if (name == NamespaceElement)
			{
				CCT_TRY_DECLARE(nameSpace, TryParseNamespace, child);
				ns.namespaces.push_back(std::move(nameSpace).GetValue());
			}
			else if (name == EnumElement)
			{
				CCT_TRY_DECLARE(nameSpace, TryParseEnum, child);
				ns.enums.push_back(std::move(nameSpace).GetValue());
			}
			else
			{
				CCT_ASSERT_FALSE("");
				std::cout << std::format("Unknown element '{}' in namespace '{}'", name, namespaceName) << '\n';
			}
		}
		return std::move(ns);
	}

	Result<Enum, std::string> Parser::TryParseEnum(const pugi::xml_node& node)
	{
		CCT_TRY_DECLARE_ATTR(enumName, node, EnumNameAttr);
		CCT_TRY_DECLARE_ATTR(enumBase, node, EnumBaseAttr);
		CCT_TRY_DECLARE(attributes, TryParseAttributes, node);

		Enum enumA = {
			.name = CCT_MAKE_SV(enumName),
			.base = CCT_MAKE_SV(enumBase),
			.elements = {},
			.attributes = std::move(attributes).GetValue()
		};

		for (auto child : node)
		{
			const auto* name = child.name();
			if (name == AttributeElement)
				continue;
			if (name != EnumMemberElement)
			{
				CCT_ASSERT_FALSE("");
				std::cout << std::format("Unknown element '{}' in enum '{}'", name, enumName);
				continue;
			}
			CCT_TRY_DECLARE_ATTR(memberName, child, EnumMemberNameAttr);
			CCT_TRY_DECLARE_ATTR(memberValue, child, EnumMemberValueAttr);
			CCT_TRY_DECLARE(elemAttributes, TryParseAttributes, node);

			Enum::Element enumElement = {
				.name = CCT_MAKE_SV(memberName),
				.value = CCT_MAKE_SV(memberValue),
				.attributes = std::move(elemAttributes.GetValue())
			};

			for (auto elementAttr : child.attributes())
			{
				if (elementAttr.name() == EnumMemberNameAttr)
					continue;
				if (elementAttr.name() == EnumMemberValueAttr)
					continue;
				enumElement.attributes.emplace(CCT_MAKE_SV(elementAttr.name()), CCT_MAKE_SV(elementAttr.value()));
			}

			enumA.elements.push_back(std::move(enumElement));
		}
		return std::move(enumA);
	}

	Result<Class, std::string> Parser::TryParseClass(const pugi::xml_node& node)
	{
		CCT_TRY_DECLARE_ATTR(className, node, EnumNameAttr);
		const auto* base = node.attribute(ClassBaseAttr.c_str()).value();
		const auto* autoGenerated = node.attribute(ClassAutoGeneratedAttr.c_str()).value();

		if (autoGenerated && autoGenerated != "true"sv && autoGenerated != "false"sv)
			return std::format("Class {} has an invalid {} attribute: {}", className, ClassAutoGeneratedAttr, autoGenerated);
		CCT_TRY_DECLARE(attributes, TryParseAttributes, node);

		Class klass = {
			.name = CCT_MAKE_SV(className),
			.base = CCT_MAKE_SV(base),
			.autoGenerated = autoGenerated ? autoGenerated == "true"sv : false,
			.methods = {},
			.members = {},
			.attributes = std::move(attributes).GetValue()
		};

		for (auto child : node)
		{
			const auto* name = child.name();
			if (name == ClassMemberElement)
			{
				CCT_TRY_DECLARE(member, TryParseClassMember, child);
				klass.members.push_back(std::move(member).GetValue());
			}
			else if (name == ClassMethodElement)
			{
				CCT_TRY_DECLARE(method, TryParseClassMethod, child);
				klass.methods.push_back(std::move(method).GetValue());
			}
			else if (name == AttributeElement)
			{
				continue;
			}
			else
			{
				CCT_ASSERT_FALSE("");
				std::cout << std::format("Unknown element '{}' in class '{}'", name, className) << '\n';
			}
		}
		return std::move(klass);
	}

	Result<Class::Member, std::string> Parser::TryParseClassMember(const pugi::xml_node& node)
	{
		CCT_TRY_DECLARE_ATTR(memberName, node, ClassMemberNameAttr);
		CCT_TRY_DECLARE_ATTR(memberType, node, ClassTypeNameAttr);
		CCT_TRY_DECLARE(attributes, TryParseAttributes, node);

		for (auto child : node)
		{
			if (child.name() == AttributeElement)
				continue;
			CCT_ASSERT_FALSE("");
			return std::format("Class::Member {} has an invalid element: {}", node.name(), child.name());
		}

		return Class::Member{
			.name = CCT_MAKE_SV(memberName),
			.type = CCT_MAKE_SV(memberType),
			.attributes = std::move(attributes).GetValue()
		};
	}

	Result<Class::Method, std::string> Parser::TryParseClassMethod(const pugi::xml_node& node)
	{
		CCT_TRY_DECLARE_ATTR(methodName, node, ClassMethodNameAttr);
		CCT_TRY_DECLARE_ATTR(returnType, node, ClassMethodReturnAttr);

		CCT_TRY_DECLARE(attributes, TryParseAttributes, node);
		

		std::vector<Class::Method::Params> params;

		for (auto child : node)
		{
			if (child.name() == AttributeElement)
				continue;
			if (child.name() != ClassMethodParamElement)
				return std::format("Class:Method has an invalid element: '{}'", child.name());

			CCT_TRY_DECLARE_ATTR(paramName, child, ClassMethodParamNameAttr);
			CCT_TRY_DECLARE_ATTR(paramType, child, ClassMethodParamTypeAttr);
			CCT_TRY_DECLARE(paramAttributes, TryParseAttributes, node);

			params.emplace_back(CCT_MAKE_SV(paramName), CCT_MAKE_SV(paramType), std::move(paramAttributes).GetValue());
		}

		return Class::Method{
			.name = CCT_MAKE_SV(methodName),
			.returnValue = CCT_MAKE_SV(returnType),
			.params = std::move(params),
			.attributes = std::move(attributes).GetValue()
		};
	}

	Result<Attributes, std::string> Parser::TryParseAttributes(const pugi::xml_node& node)
	{
		Attributes attributes;
		for (auto child : node)
		{
			if (child.name() != AttributeElement)
				continue;
			if (!child.children().empty())
				return std::format("Attribute '{}' must not have child elements in element: {}", child.name(), node.name());
			CCT_TRY_DECLARE_ATTR(name, child, AttributeMemberNameAttr);
			CCT_TRY_DECLARE_ATTR(value, child, AttributeMemberValueAttr);
			for (auto attribute : child.attributes())
			{
				if (attribute.name() == AttributeMemberNameAttr)
					continue;
				if (attribute.name() == AttributeMemberValueAttr)
					continue;
				return std::format("Attribute element must only have two attributes ({}, {}) but '{}' was found in parent element {}", AttributeMemberNameAttr, AttributeMemberValueAttr, child.name(), node.name());
			}
			attributes.emplace(CCT_MAKE_SV(name), CCT_MAKE_SV(value));
		}
		return std::move(attributes);
	}
}
