//
// Created by arthur on 09/12/2024.
//

#include <algorithm>
#include <format>

#include "Concerto/PackageGenerator/HeaderGenerator/HeaderGenerator.hpp"


namespace cct
{
	bool HeaderGenerator::Generate(const Package& package, std::span<std::string> args)
	{
		std::string upperPackageName(package.name);
		std::ranges::transform(upperPackageName, upperPackageName.begin(), [](char c)
			{
				return std::toupper(c);
			});
		std::string api = upperPackageName + "PACKAGE_API";
		Write("//This file was automatically generated, do not edit\n");
		Write("#pragma once");
		Write("#include <string_view>");
		Write("#include <Concerto/Reflection/Defines.hpp>");
		NewLine();
		Write("#include <Concerto/Reflection/Class/Class.hpp>");
		Write("#include <Concerto/Reflection/Object/Object.hpp>");
		Write("#include <Concerto/Reflection/MemberVariable/MemberVariable.hpp>");
		Write("#include <Concerto/Reflection/Method/Method.hpp>");
		Write("#include <Concerto/Reflection/Package/Package.hpp>");
		NewLine();

		NewLine();
		Write("#ifdef {}PACKAGE_STATIC", upperPackageName);
		{
			Write("	#define {}PACKAGE_API", upperPackageName);
		}
		Write("#else");
		{
			Write("#ifdef {}PACKAGE_BUILD", upperPackageName);
			Write("	#define {}PACKAGE_API CCT_EXPORT", upperPackageName);
			Write("#else");
			Write("	#define {}PACKAGE_API CCT_IMPORT\n", upperPackageName);
			Write("#endif");
		}
		Write("#endif");
		NewLine();

		for (auto& enum_ : package.enums)
			GenerateEnum(enum_, api);

		NewLine();

		for (auto& klass : package.classes)
		{
			GenerateClass(klass, api);
			NewLine();
		}

		NewLine();

		for (auto& ns : package.namepsaces)
			GenerateNamespace(ns, api);
		NewLine();
		Write("{} std::unique_ptr<cct::refl::Package> Create{}Package();", api, package.name);
		return true;
	}

	void HeaderGenerator::GenerateNamespace(const Namespace& ns, const std::string& api)
	{
		Write("namespace {}", ns.name);
		EnterScope();
		for (auto& nestedNamespace : ns.namespaces)
			GenerateNamespace(nestedNamespace, api);
		for (auto& enum_ : ns.enums)
			GenerateEnum(enum_, api);
		NewLine();
		/*for (auto& klass : ns.classes)
			GenerateClass(klass, api);*/
		LeaveScope();
	}

	void HeaderGenerator::GenerateClass(const Class& klass, const std::string& api)
	{
		if (klass.base.empty())
			Write("class {} {}", api, klass.name);
		else
			Write("class {} {} : public {}", api, klass.name, klass.base);
		EnterScope();
		{
			Write("public:");
			for (const auto& member : klass.members)
			{
				Write("const {}& Get{}() const", member.type, Capitalize(member.name));
				EnterScope();
				Write("return _{};", member.name);
				LeaveScope();
				Write("{}& Get{}()", member.type, Capitalize(member.name));
				EnterScope();
				Write("return _{};", member.name);
				LeaveScope();
			}
			NewLine();
			Write("CCT_OBJECT({});", klass.name);
			NewLine();
			Write("private:");
			for (const auto& member : klass.members)
			{
				Write("{} _{};", member.type, member.name);
			}
		}
		LeaveScope(";");
	}

	void HeaderGenerator::GenerateEnum(const Enum& enum_, const std::string& api)
	{
		Write("enum class {} : {};", enum_.name, enum_.base);
		Write("{} std::string_view {}ToString({} value);", api, Capitalize(enum_.name), enum_.name);
		Write("{} {} {}FromString(std::string_view str);", api, enum_.name, Capitalize(enum_.name));
	}
}
