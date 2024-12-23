//
// Created by arthur on 09/12/2024.
//

#ifndef CONCERTO_PKGGENERATOR_PARSER_HPP
#define CONCERTO_PKGGENERATOR_PARSER_HPP

#include <string_view>
#include <pugixml.hpp>
#include <Concerto/Core/Result.hpp>

#include "Concerto/PackageGenerator/Defines.hpp"

namespace cct
{
	using namespace std::string_literals;
	class Parser
	{
	public:
		static Result<Package, std::string> TryParse(const pugi::xml_document& doc);
		static Result<Include, std::string> TryParseInclude(const pugi::xml_node& node);
		static Result<Namespace, std::string> TryParseNamespace(const pugi::xml_node& node);
		static Result<Enum, std::string> TryParseEnum(const pugi::xml_node& node);
		static Result<Class, std::string> TryParseClass(const pugi::xml_node& node);
		static Result<Class::Member, std::string> TryParseClassMember(const pugi::xml_node& node);
		static Result<Class::Method, std::string> TryParseClassMethod(const pugi::xml_node& node);
		static Result<Attributes, std::string> TryParseAttributes(const pugi::xml_node& node);

	private:
		//not using string_view because pugixml only support null terminated string
		//Elements
		static inline const std::string PackageElement = "package"s;
		static inline const std::string IncludeElement = "include"s;
		static inline const std::string NamespaceElement = "namespace"s;


		static inline const std::string ClassElement = "class"s;
		static inline const std::string ClassMemberElement = "member"s;
		static inline const std::string ClassMethodElement = "method"s;
		static inline const std::string ClassMethodParamElement = "param"s;

		static inline const std::string EnumElement = "enum"s;
		static inline const std::string EnumMemberElement = "member"s;

		static inline const std::string AttributeElement = "attribute"s;

		//Attributes
		static inline const std::string PackageNameAttrib = "name"s;
		static inline const std::string PackageVersionAttrib = "version"s;
		static inline const std::string PackageDescriptionAttrib = "description"s;

		static inline const std::string IncludeIsPublicAttr = "public"s;
		static inline const std::string IncludeFileAttr = "file"s;

		static inline const std::string NamespaceNameAttr = "name"s;

		static inline const std::string EnumNameAttr = "name"s;
		static inline const std::string EnumBaseAttr = "base"s;

		static inline const std::string EnumMemberNameAttr = "name"s;
		static inline const std::string EnumMemberValueAttr = "value"s;

		static inline const std::string ClassNameAttr = "name"s;
		static inline const std::string ClassBaseAttr = "base"s;
		static inline const std::string ClassAutoGeneratedAttr = "autoGenerated"s;

		static inline const std::string ClassMemberNameAttr = "name"s;
		static inline const std::string ClassTypeNameAttr = "type"s;

		static inline const std::string ClassMethodNameAttr = "name"s;
		static inline const std::string ClassMethodReturnAttr = "return"s;
		static inline const std::string ClassMethodBaseAttr = "base"s;
		static inline const std::string ClassMethodCustomInvokerAttr = "customInvoke"s;

		static inline const std::string ClassMethodParamNameAttr = "name"s;
		static inline const std::string ClassMethodParamTypeAttr = "type"s;

		static inline const std::string AttributeMemberNameAttr = "name"s;
		static inline const std::string AttributeMemberValueAttr = "value"s;
	};
}

#endif // CONCERTO_PKGGENERATOR_PARSER_HPP