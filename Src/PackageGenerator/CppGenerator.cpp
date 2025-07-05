//
// Created by arthur on 09/12/2024.
//

#include "Concerto/PackageGenerator/CppGenerator.hpp"

namespace cct
{
	using namespace std::string_view_literals;
	using namespace std::string_literals;

	bool CppGenerator::Generate(const Package& package, std::span<std::string_view> args)
	{
		Write("//This file was automatically generated, do not edit");
		Write("#include <Concerto/Core/Assert.hpp>");
		Write("#include \"Concerto/Reflection/GlobalNamespace.hpp\"");
		Write("#include \"{}Package.gen.hpp\"", package.name);
		Write("using namespace std::string_view_literals;");
		Write("using namespace std::string_literals;");
		Write("using namespace cct;");
		Write("using namespace cct::refl;");

		for (auto& enum_ : package.enums)
			GenerateEnum(enum_);
		for (auto& klass : package.classes)
			GenerateClass({}, klass);
		for (auto& ns : package.namepsaces)
			GenerateNamespace(ns);
		GeneratePackage(package);
		return true;
	}

	void CppGenerator::GenerateNamespace(const Namespace& ns, const std::string& namespaceChain)
	{
		Write("namespace {}", ns.name);
		EnterScope();
		{
			for (auto& nestedNs : ns.namespaces)
				GenerateNamespace(nestedNs, namespaceChain + "::"s + std::string(ns.name));
			for (auto& enum_ : ns.enums)
				GenerateEnum(enum_);
			for (auto& klass : ns.classes)
				GenerateClass(namespaceChain + "::"s + std::string(ns.name), klass);
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
						Write("AddClass(std::make_unique<{}::Internal{}Class>());", ns.name, klass.name);

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
		std::size_t methodIndex = 0;
		for (auto& method : klass.methods)
		{
			GenerateClassMethod(klass.name, method, ns, methodIndex);
			++methodIndex;
		}
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
				{
					Write("const Class* baseClass = GetClassByName(\"{}\"sv);", klass.base);
					Write("CCT_ASSERT(baseClass != nullptr, \"Could not find class '{}'\");", klass.base);
					Write("SetBaseClass(baseClass);");
				}
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

				//for (auto& [name, value] : klass.attributes)
				//	Write(R"(AddAttribute("{}"s, "{}"s);)", name, value);
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
						Write("return &static_cast<{}&>(self).{};", klass.name, member.name);
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

	void CppGenerator::GenerateClassMethod(std::string_view className, const Class::Method& method, std::string_view ns, std::size_t methodIndex)
	{
		auto baseClass = method.base.empty() ? "cct::refl::Method"sv : method.base;
		Write("class {}{}Method : public {}", className, method.name, baseClass);
		EnterScope();
		{
			Write("public:");
			Write("using BaseClass = {};", baseClass);
			Write("using BaseClass::BaseClass;");
			NewLine();
			Write("void Initialize() override");
			EnterScope();
			//for (auto& [name, value] : method.attributes)
			//	Write(R"(AddAttribute("{}"s, "{}"s);)", name, value);
			LeaveScope();
			NewLine();
			Write("cct::Result<cct::Any, std::string> Invoke(cct::refl::Object& self, std::span<cct::Any> parameters) const override");
			EnterScope();
			{
				Write("if (parameters.size() != {})", method.params.size());
				EnterScope();
				{
					Write("CCT_ASSERT_FALSE(\"Invalid parameters size\");");
					Write("return {{\"Invalid parameters size\"s}};");
				}
				LeaveScope();
				std::size_t i = 0;
				std::string callArgs;
				std::string callArgsTypes;
				for (auto& param : method.params)
				{
					if (i != 0 && i < method.params.size())
					{
						callArgs += ", ";
						callArgsTypes += ", ";
					}
					Write("if (parameters[{}].Is<{}>() == false)", i, param.type);
					EnterScope();
					{
						Write("CCT_ASSERT_FALSE(\"Expected '{}' in argument {}\");", param.type, i);
						Write("return {{\"Expected '{}' in argument {}\"s}};", param.type, i);
					}
					LeaveScope();
					Write("using Param{0} = std::conditional_t<std::is_pointer_v<{1}>, {1}, std::add_lvalue_reference_t<{1}>>;", i, param.type);
					Write("Param{0} {1} = parameters[{0}].As<Param{0}>();", i, param.name);
					NewLine();
					callArgs += param.name;
					callArgsTypes += param.type;
					++i;
				}
				NewLine();
				if (method.returnValue == "void")
				{
					if (method.tomlAttributes.is_table() && method.tomlAttributes.as_table().contains("Delegate"))
					{
						auto it = method.tomlAttributes.as_table().find("Delegate");
						if (it->second.is_boolean())
						{
							Write("if (GetCustomDelegate() == nullptr)");
							EnterScope();
							Write("return {{\"Invalid delegate pointer\"s}};");
							LeaveScope();
							Write("auto func = reinterpret_cast<void(*)({})>(GetCustomDelegate());", method.returnValue, callArgsTypes);
							Write("func({});", callArgs);
						}
						else if (it->second.is_string())
						{
							auto delegateName = it->second.as_string();
							Write("{}({});", delegateName, callArgs);
						}
					}
					else
						Write("static_cast<{}&>(self).{}({});", className, method.name, callArgs);
					Write("return Any{{}};");
				}
				else
				{
					if (method.tomlAttributes.is_table() && method.tomlAttributes.as_table().contains("Delegate"))
					{
						auto it = method.tomlAttributes.as_table().find("Delegate");
						if (it->second.is_boolean())
						{
							Write("if (GetCustomInvoker() == nullptr)");
							EnterScope();
							Write("return {{\"Invalid invoker pointer\"s}};");
							LeaveScope();
							Write("auto func = reinterpret_cast<void(*)({})>(GetCustomInvoker());", method.returnValue, callArgsTypes);
							Write("return Any::Make<{}>(func({}));", method.returnValue, callArgs);
						}
						else if (it->second.is_string())
						{
							auto delegateName = it->second.as_string();
							Write("return Any::Make<{}>({}({}));", method.returnValue, delegateName, callArgs);
						}
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

		//if (method.tomlAttributes.as_table().contains("Delegate"))
		//{
		//	std::string methodParameterPrototype;
		//	std::string callArgs;
		//	std::string callArgsTypes;
		//	std::size_t i = 0;
		//	for (auto& param : method.params)
		//	{
		//		if (i != 0 && i < method.params.size())
		//		{
		//			methodParameterPrototype += ", ";
		//			callArgs += ", ";
		//			callArgsTypes += ", ";
		//		}
		//		methodParameterPrototype += param.type + ' ' + param.name;
		//		callArgs += param.name;
		//		callArgsTypes += param.type;
		//		++i;
		//	}
		//	if (ns.starts_with("::"sv))
		//		ns.remove_prefix(2);
		//	Write("{} {}::{}::{}({})", method.returnValue, ns, className, method.name, methodParameterPrototype);
		//	EnterScope();
		//	auto delegateIt = method.tomlAttributes.as_table().find("Delegate");
		//	if (delegateIt->second.is_string())
		//	{
		//		auto functionName = method.tomlAttributes.as_table().find("Delegate")->second.as_string();
		//		Write("{}({});", functionName, callArgs);
		//	}
		//	else if (delegateIt->second.is_boolean())
		//	{
		//		Write("if (GetCustomInvoker() == nullptr)");
		//		EnterScope(),
		//		Write("return {{ \"Invalid invoker pointer\"s }};");
		//		LeaveScope();
		//		Write("auto func = reinterpret_cast<{}(*)({})>(GetCustomInvoker());", method.returnValue, callArgsTypes);
		//		if (method.returnValue != "void"s)
		//			Write("return");
		//		Write("func({});", callArgs);
		//	}
			//LeaveScope();
		//}
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
		if (pkg.name.empty())
			return;
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
