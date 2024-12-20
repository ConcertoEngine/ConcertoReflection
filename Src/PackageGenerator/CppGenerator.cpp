//
// Created by arthur on 09/12/2024.
//

#include "Concerto/PackageGenerator/CppGenerator.hpp"

namespace cct
{
	using namespace std::string_view_literals;
	using namespace std::string_literals;

	bool CppGenerator::Generate(const Package& package)
	{
		Write("//This file was automatically generated, do not edit");
		Write("#include <Concerto/Core/Assert.hpp>");
		Write("#include \"Concerto/Reflection/GlobalNamespace.hpp\"");
		Write("#include \"{}Package.hpp\"", package.name);
		Write("using namespace std::string_view_literals;");
		Write("using namespace std::string_literals;");
		Write("using namespace cct;");
		Write("using namespace cct::refl;");

		for (auto& include : package.includes)
		{
			if (include.isPublic)
				continue;
			Write("#include \"{}\"", include.file);
		}
		for (auto& enum_ : package.enums)
			GenerateEnum(enum_);
		for (auto& klass : package.classes)
			GenerateClass({}, klass);
		for (auto& ns : package.namepsaces)
			GenerateNamespace(ns);
		GeneratePackage(package);
		return true;
	}

	void CppGenerator::GenerateNamespace(const Namespace& ns, const std::string& namepsaceChain)
	{
		Write("namespace {}", ns.name);
		EnterScope();
		{
			for (auto& nestedNs : ns.namespaces)
				GenerateNamespace(nestedNs, namepsaceChain + "::"s + std::string(ns.name));
			for (auto& enum_ : ns.enums)
				GenerateEnum(enum_);
			for (auto& klass : ns.classes)
				GenerateClass(namepsaceChain + "::"s + std::string(ns.name), klass);
		}
		LeaveScope();
		Write("namespace");
		EnterScope();
		{
			Write("class Internal{}Namespace : public cct::refl::Namespace", ns.name);
			EnterScope();
			{
				Write("public:");
				Write("Internal{}Namespace() : cct::refl::Namespace(\"{}\"s) {{}}", ns.name, ns.name);
				NewLine();
				Write("~Internal{}Namespace() override", ns.name);
				EnterScope();
				{
					Write("_classes.clear();");
					Write("_namespaces.clear();");
				}
				LeaveScope();
				NewLine();
				Write("void LoadNamespaces() override");
				EnterScope();
				{
					for (auto& nestedNs : ns.namespaces)
						Write("AddNamespace(Create{}NamespaceInstance());", nestedNs.name);
					Write("for (auto& ns : _namespaces)");
					EnterScope();
					{
						Write("ns->LoadNamespaces();");
					}
					LeaveScope();
				}
				LeaveScope();
				NewLine();
				Write("void LoadClasses() override");
				EnterScope();
				{
					Write("for(auto& ns : _namespaces)");
					EnterScope();
					Write("ns->LoadClasses();");
					LeaveScope();
					for (auto& klass : ns.classes)
						Write("AddClass(std::make_unique<Internal{}Class>());", klass.name);

				}
				LeaveScope();
				NewLine();
				Write("void InitializeClasses() override");
				EnterScope();
				{
					Write("for (auto& ns : _namespaces)");
					EnterScope();
					Write("ns->InitializeClasses();");
					LeaveScope();
					Write("for (auto& klass : _classes)");
					EnterScope();
					Write("klass->Initialize();");
					LeaveScope();
				}
				LeaveScope();
				NewLine();
			}
			LeaveScope(";"sv);
			Write("std::unique_ptr<cct::refl::Namespace> Create{}NamespaceInstance(){{return std::make_unique<Internal{}Namespace>();}}", ns.name, ns.name);
		}
		LeaveScope();
	}

	void CppGenerator::GenerateClass(std::string_view ns, const Class& klass)
	{
		for (auto& method : klass.methods)
			GenerateClassMethod(klass.name, method);
		NewLine();
		Write("class Internal{}Class : public cct::refl::Class", klass.name);
		EnterScope();
		{
			Write("public:");
			Write("Internal{0}Class() : cct::refl::Class(nullptr, \"{0}\"s, nullptr)", klass.name);
			EnterScope();
			{
				Write("if ({}::_class != nullptr)", klass.name);
				EnterScope();
				{
					Write("CCT_ASSERT_FALSE(\"Class already created\");");
					Write("return;");
				}
				LeaveScope();
				Write("{}::_class = this;", klass.name);
			}
			LeaveScope();
			Write("~Internal{}Class()", klass.name);
			EnterScope();
			{
				Write("{}::_class = nullptr;", klass.name);
			}
			LeaveScope();
			NewLine();
			Write("void Initialize() override");
			EnterScope();
			{
				Write("SetNamespace(GlobalNamespace::Get().GetNamespaceByName(\"{}\"sv));", ns);
				if (!klass.base.empty())
					Write("SetBaseClass(GetClassByName(\"{}\"sv));", klass.base);
				NewLine();
				for (auto& member : klass.members)
					Write(R"(AddMemberVariable("{}", cct::refl::GetClassByName("{}"));)", member.name, member.type);
				NewLine();
				std::size_t i = 0;
				for (auto& method : klass.methods)
				{
					std::string params;
					for (auto& param : method.params)
						params += std::format(R"(cct::refl::GetClassByName(""sv, "{}"sv), )", param.type);
					Write(R"(auto* {}Method = new {}{}Method("{}"sv, cct::refl::GetClassByName("{}"sv), {{ {} }}, {});)", method.name, klass.name, method.name, method.name, method.returnValue, params, i);
					Write("AddMemberFunction(std::unique_ptr<cct::refl::Method>({}Method));", method.name);
					++i;
				}

				for (auto& [name, value] : klass.attributes)
					Write(R"(AddAttribute("{}"s, "{}"s);)", name, value);
			}
			LeaveScope();
			NewLine();
			Write("std::unique_ptr<cct::refl::Object> CreateDefaultObject() const override {{ return std::unique_ptr<cct::refl::Object>(new {}); }}", klass.name);
			NewLine();
			Write("cct::refl::Object* GetMemberVariable(std::size_t index, cct::refl::Object& self) const override");
			EnterScope();
			{
				std::size_t i = 0;
				for (auto& member : klass.members)
				{
					Write("if (index == {})", i);
					EnterScope();
					{
						Write("return &static_cast<{}&>(self)._{};", klass.name, member.name);
					}
					LeaveScope();
					++i;
				}
				Write("CCT_ASSERT_FALSE(\"Invalid index\");");
				Write("return nullptr;");
			}
			LeaveScope();
			NewLine();
		}
		LeaveScope(";"sv);
		NewLine();
		Write("const cct::refl::Class* {}::_class = nullptr;", klass.name);
		NewLine();
	}

	void CppGenerator::GenerateClassMethod(std::string_view className, const Class::Method& method)
	{
		Write("class {}{}Method : public {}", className, method.name, method.base);
		EnterScope();
		{
			Write("public:");
			Write("using Method::Method;");
			NewLine();
			Write("void Initialize() override");
			EnterScope();
			for (auto& [name, value] : method.attributes)
				Write(R"(AddAttribute("{}"s, "{}"s);)", name, value);
			LeaveScope();
			NewLine();
			if (method.overrideInvoke)
			{
				Write("cct::Any Invoke(cct::refl::Object& self, std::span<cct::Any> parameters) const override");
				EnterScope();
				{
					Write("CCT_ASSERT(parameters.size() == {}, \"Invalid parameters size\");", method.params.size());
					std::size_t i = 0;
					std::string callArgs;
					for (auto& param : method.params)
					{
						if (i != 0 && i < method.params.size())
							callArgs += ", ";
						Write("{}& {} = parameters[{}].As<{}&>();", param.type, param.name, i, param.type);
						callArgs += param.name;
						++i;
					}
					NewLine();
					if (method.returnValue == "void")
					{
						Write("static_cast<{}&>(self).{}({});", className, method.name, callArgs);
						Write("return {{}};");
					}
					else
					{
						Write("auto res = static_cast<{}&>(self).{}({});", className, method.name, callArgs);
						Write("return cct::Any::Make<{}>(res);", method.returnValue);
					}
				}
				LeaveScope();
			}
		}
		LeaveScope(";"sv);
	}

	void CppGenerator::GenerateEnum(const Enum& enum_)
	{
		Write("std::string_view {}ToString({} value)", enum_.name, enum_.name);
		EnterScope();
		{
			Write("switch(value)");
			EnterScope();
			{

				for (auto& elem : enum_.elements)
				{
					Write("case {}::{}:", enum_.name, elem.name);
					Write("return \"{}\"sv;", elem.name);
				}
			}
			LeaveScope();
			Write("CCT_ASSERT_FALSE(\"Invalid enum value\");");
			Write("return {{}};");
		}
		LeaveScope();

		Write("{} {}FromString(std::string_view value)", enum_.name, enum_.name);
		EnterScope();
		{
			for (auto& elem : enum_.elements)
			{
				Write("if (value == \"{}\"sv)", elem.name);
				EnterScope();
				Write("return {}::{};", enum_.name, elem.name);
				LeaveScope();
			}
			Write("CCT_ASSERT_FALSE(\"Invalid enum value: {{}}\", value);");
			Write("return {{}};");
		}
		LeaveScope();
	}

	void CppGenerator::GeneratePackage(const Package& pkg)
	{
		Write("class Internal{}Package : public cct::refl::Package", pkg.name);
		EnterScope();
		{
			Write("public:");
			Write("Internal{0}Package() : cct::refl::Package(\"{0}\"s) {{}}", pkg.name);
			NewLine();
			Write("~Internal{}Package() override", pkg.name);
			EnterScope();
			{
				for (auto& klass : pkg.classes)
					Write("GlobalNamespace::Get().RemoveClass(\"{}\"sv);", klass.name);
				for (auto& ns : pkg.namepsaces)
					Write("GlobalNamespace::Get().RemoveNamespace(\"{}\"sv);", ns.name);
				Write("_namespaces.clear();");
			}
			LeaveScope();

			NewLine();
			Write("void LoadNamespaces() override");
			EnterScope();
			{
				for (auto& ns : pkg.namepsaces)
					Write("AddNamespace(Create{}NamespaceInstance());", ns.name);
				Write("for (auto& ns : _namespaces)");
				EnterScope();
				{
					Write("ns->LoadNamespaces();");
				}
				LeaveScope();
			}
			LeaveScope();
			NewLine();
			Write("void InitializeNamespaces() override");
			EnterScope();
			{
				for (auto& klass : pkg.classes)
				{
					EnterScope();
					Write("AddClass(std::make_unique<Internal{}Class>());", klass.name);
					LeaveScope();
				}
				NewLine();
				Write("for (auto& ns : _namespaces)");
				EnterScope();
				Write("ns->LoadClasses();");
				LeaveScope();
			}
			LeaveScope();
			NewLine();
			Write("void InitializeClasses() override");
			EnterScope();
			{
				Write("for (auto& ns : _namespaces)");
				EnterScope();
				{
					Write("ns->InitializeClasses();");
				}
				LeaveScope();
			}
			LeaveScope();
		}
		LeaveScope(";"sv);
		NewLine();
		Write("std::unique_ptr<cct::refl::Package> Create{}Package()", pkg.name);
		EnterScope();
		Write("return std::make_unique<Internal{}Package>();", pkg.name);
		LeaveScope();
	}
}
