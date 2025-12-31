//
// Created by arthur on 09/12/2024.
//

#include "Concerto/PackageGenerator/CppGenerator/CppGenerator.hpp"

#include <Concerto/Core/Logger/Logger.hpp>

namespace cct
{
	using namespace std::string_view_literals;
	using namespace std::string_literals;

	bool CppGenerator::Generate(const Package& package, std::span<std::string> args)
	{
		Write("//This file was automatically generated, do not edit");
		Write("#include <Concerto/Core/Assert.hpp>");
		Write("#include <Concerto/Core/TypeInfo/TypeInfo.hpp>");
		Write("#include \"Concerto/Reflection/GlobalNamespace/GlobalNamespace.hpp\"");
		Write("#include <Concerto/Reflection/GenericClass/GenericClass.hpp>");
		Write("#include <Concerto/Reflection/Enumeration/EnumerationClass.hpp>");
		Write("#include \"{}Package.gen.hpp\"", package.name);

		for (auto& header : args)
			Write("#include \"{}\"", header);

		Write("using namespace std::string_view_literals;");
		Write("using namespace std::string_literals;");
		Write("using namespace cct;");
		Write("using namespace cct::refl;");

		for (auto& enum_ : package.enums)
			GenerateEnum(enum_, "");
		for (auto& klass : package.classes)
		{
			if (klass.isGenericClass)
				GenerateGenericClass({}, klass);
			else if (klass.isTemplateClass)
				GenerateTemplateClass({}, klass);
			else
				GenerateClass({}, klass);
		}
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
				GenerateEnum(enum_, namespaceChain + "::"s + std::string(ns.name));
			for (auto& klass : ns.classes)
			{
				if (klass.isGenericClass)
					GenerateGenericClass(namespaceChain + "::"s + std::string(ns.name), klass);
				else if (klass.isTemplateClass)
					GenerateTemplateClass(namespaceChain + "::"s + std::string(ns.name), klass);
				else
					GenerateClass(namespaceChain + "::"s + std::string(ns.name), klass);
			}
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
					Write("m_classes.clear();");
					Write("m_namespaces.clear();");
				}
				LeaveScope();
				NewLine();
				Write("void LoadNamespaces() override");
				EnterScope();
				{
					for (auto& nestedNs : ns.namespaces)
						Write("AddNamespace({}::{}::Create{}NamespaceInstance());", namespaceChain, ns.name, nestedNs.name);
					Write("for (auto& ns : m_namespaces)");
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
					Write("for(auto& ns : m_namespaces)");
					EnterScope();
					Write("ns->LoadClasses();");
					LeaveScope();
					for (auto& enum_ : ns.enums)
					{
						Write("AddClass(std::make_unique<{}::Internal{}EnumerationClass>());", ns.name, enum_.name);
					}
					for (auto& klass : ns.classes)
					{
						if (klass.isGenericClass)
						Write("AddClass(std::make_unique<{}::Internal{}GenericClass>());", ns.name, klass.name);
					else if (klass.isTemplateClass)
						{
							Write("AddClass(std::make_unique<{}::Internal{}TemplateClass>());", ns.name, klass.name);
							for (const auto& specialization : klass.templateSpecializations)
							{
								if (specialization.empty())
									continue;
								std::string specName = specialization;
								for (auto& c : specName)
								{
									if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && (c < '0' || c > '9'))
										c = '_';
								}
								Write("AddClass(std::make_unique<{}::Internal{}_{}_Class>());", ns.name, klass.name, specName);
							}
						}
						else
							Write("AddClass(std::make_unique<{}::Internal{}Class>());", ns.name, klass.name);
					}

				}
				LeaveScope();
				NewLine();
				Write("void InitializeClasses() override");
				EnterScope();
				{
					Write("for (auto& ns : m_namespaces)");
					EnterScope();
					Write("ns->InitializeClasses();");
					LeaveScope();
					Write("for (auto& klass : m_classes)");
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
				Write("if ({}::m_class != nullptr)", klass.name);
				EnterScope();
				{
					Write("CCT_ASSERT_FALSE(\"Class already created\");");
					Write("return;");
				}
				LeaveScope();
				Write("{}::m_class = this;", klass.name);
			}
			LeaveScope();
			Write("~Internal{}Class() override", klass.name);
			EnterScope();
			{
				Write("{}::m_class = nullptr;", klass.name);
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
					if (member.isNative)
						Write(R"(AddNativeMemberVariable("{}", cct::TypeId<{}>());)", member.name, member.type);
					else
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
			Write("std::unique_ptr<cct::refl::Object> CreateDefaultObject() const override");
			EnterScope();
			{
				Write("auto object = std::unique_ptr<cct::refl::Object>(new {});", klass.name);
				Write("if (object)");
				EnterScope();
				{
					Write("object->SetDynamicClass(this);", klass.name);
					Write("object->InitializeMemberVariables();");
				}
				LeaveScope();
				Write(" return object;");
				LeaveScope();
			}
			NewLine();
			Write("cct::refl::Object* GetMemberVariable(std::size_t index, const cct::refl::Object& self) const override");
			EnterScope();
			{
				std::size_t i = 0;
				for (auto& member : klass.members)
				{
					if (member.isNative)
						continue;
					Write("if (index == {})", i);
					EnterScope();
					{
						Write("return &const_cast<{0}&>(static_cast<const {0}&>(self)).{1};", klass.name, member.name);
					}
					LeaveScope();
					++i;
				}
				Write("CCT_ASSERT_FALSE(\"Invalid index\");");
				Write("return nullptr;");
			}
			LeaveScope();
			NewLine();
			Write("void* GetNativeMemberVariable(std::size_t index, const cct::refl::Object& self) const override");
			EnterScope();
			{
				std::size_t i = 0;
				for (auto& member : klass.members)
				{
					if (member.isNative == false)
						continue;
					Write("if (index == {})", i);
					EnterScope();
					{
						Write("return &const_cast<{0}&>(static_cast<const {0}&>(self)).{1};", klass.name, member.name);
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
	}

	void CppGenerator::GenerateGenericClass(std::string_view ns, const Class& klass)
	{
		Write("class Internal{}GenericClass : public cct::refl::GenericClass", klass.name);
		EnterScope();
		{
			Write("public:");
			Write("Internal{}GenericClass() : cct::refl::GenericClass(nullptr, \"{}\"s, nullptr)",
				klass.name, klass.name);
			EnterScope();
			LeaveScope();

			NewLine();
			Write("void Initialize() override");
			EnterScope();
			{
				if (!ns.empty())
					Write("SetNamespace(GlobalNamespace::Get().GetNamespaceByName(\"{}\"sv));", ns);

				if (!klass.base.empty())
				{
					Write("const Class* baseClass = GetClassByName(\"{}\"sv);", klass.base);
					Write("CCT_ASSERT(baseClass != nullptr, \"Could not find class '{}'\");", klass.base);
					Write("SetBaseClass(baseClass);");
				}

				NewLine();
				Write("SetTypeParameterCount({});", klass.genericTypeParameterFields.size());

				for (const auto& fieldName : klass.genericTypeParameterFields)
					Write("AddTypeParameter(\"{}\");", fieldName);

				NewLine();
				// Add members and methods as in regular class
				for (auto& member : klass.members)
				{
					if (member.isNative)
						Write(R"(AddNativeMemberVariable("{}", cct::TypeId<{}>());)", member.name, member.type);
					else
						Write(R"(AddMemberVariable("{}", cct::refl::GetClassByName("{}"sv));)", member.name, member.type);
				}

			}
			LeaveScope();

			NewLine();
			Write("std::unique_ptr<cct::refl::Object> CreateDefaultObject() const override");
			EnterScope();
			{
				Write("CCT_ASSERT_FALSE(\"Cannot instantiate generic class directly. Use CreateDefaultObject(std::span<const Class*>)\");");
				Write("return nullptr;");
			}
			LeaveScope();

			NewLine();
			Write("std::unique_ptr<cct::refl::Object> CreateDefaultObject(");
			Write("    std::span<const cct::refl::Class*> typeArgs) const override");
			EnterScope();
			{
				Write("if (typeArgs.size() != {})", klass.genericTypeParameterFields.size());
				EnterScope();
				{
					Write("CCT_ASSERT_FALSE(\"Expected {} type arguments, got {{}}\", typeArgs.size());",
						klass.genericTypeParameterFields.size());
					Write("return nullptr;");
				}
				LeaveScope();

				if (ns.empty())
					Write("auto obj = std::make_unique<{}>();", klass.name);
				else
					Write("auto obj = std::make_unique<{}::{}>();", ns, klass.name);

				// Inject type parameters
				for (std::size_t i = 0; i < klass.genericTypeParameterFields.size(); ++i)
					Write("obj->{} = typeArgs[{}];", klass.genericTypeParameterFields[i], i);

				Write("obj->SetDynamicClass(this);");
				Write("obj->InitializeMemberVariables();");
				Write("return obj;");
			}
			LeaveScope();

			NewLine();
			Write("cct::refl::Object* GetMemberVariable(std::size_t, const cct::refl::Object&) const override");
			EnterScope();
			{
				Write("return nullptr;");
			}
			LeaveScope();

			NewLine();
			Write("void* GetNativeMemberVariable(std::size_t, const cct::refl::Object&) const override");
			EnterScope();
			{
				Write("return nullptr;");
			}
			LeaveScope();
		}
		LeaveScope(";");
	}

	void CppGenerator::GenerateTemplateClass(std::string_view ns, const Class& klass)
	{
		Write("namespace");
		EnterScope();
		{
			for (const auto& specialization : klass.templateSpecializations)
			{
				if (specialization.empty())
					continue;

				// Create a sanitized name for the specialization class
				std::string specName = specialization;
				for (auto& c : specName)
				{
					if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && (c < '0' || c > '9'))
						c = '_';
				}

				Write("class Internal{}_{}_Class : public ::cct::refl::Class", klass.name, specName);
				EnterScope();
				{
					Write("public:");
					Write("Internal{}_{}_Class() : ::cct::refl::Class(nullptr, \"{}\"s, nullptr)", klass.name, specName, std::format("{}<{}>", klass.name, specialization));
					EnterScope();
					LeaveScope();
					NewLine();
					Write("void Initialize() override");
					EnterScope();
					{
						// Specialization classes initialize their template instance
					}
					LeaveScope();
					NewLine();
					Write("::cct::refl::Object* GetMemberVariable(std::size_t index, const ::cct::refl::Object& self) const override");
					EnterScope();
					{
						Write("return nullptr;");
					}
					LeaveScope();
					NewLine();
					Write("void* GetNativeMemberVariable(std::size_t index, const ::cct::refl::Object& self) const override");
					EnterScope();
					{
						Write("return nullptr;");
					}
					LeaveScope();
					NewLine();
					Write("std::unique_ptr<::cct::refl::Object> CreateDefaultObject() const override");
					EnterScope();
					{
						if (ns.empty())
						{
							Write("auto obj = std::make_unique<{}{}>();", klass.name, std::format("<{}>", specialization));
						}
						else
						{
							Write("auto obj = std::make_unique<{}::{}{}>();", ns, klass.name, std::format("<{}>", specialization));
						}
						Write("return obj;");
					}
					LeaveScope();
				}
				LeaveScope(";");
				NewLine();
			}

			Write("class Internal{}TemplateClass : public ::cct::refl::TemplateClass", klass.name);
			EnterScope();
			{
				Write("public:");
				Write("Internal{}TemplateClass() : ::cct::refl::TemplateClass(nullptr, \"{}\"s, nullptr)", klass.name, klass.name);
				EnterScope();
				LeaveScope();
				NewLine();
				Write("void Initialize() override");
				EnterScope();
				{
					for (const auto& param : klass.templateParameters)
						Write("cct::refl::TemplateClass::AddTemplateParameter(\"{}\");", param.name);

					if (!klass.templateSpecializations.empty())
						Write("std::vector<std::string> typeArgs;");

					for (const auto& specialization : klass.templateSpecializations)
					{
						if (specialization.empty())
							continue;

						// Create sanitized name
						std::string specName = specialization;
						for (auto& c : specName)
						{
							if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && (c < '0' || c > '9'))
								c = '_';
						}

						Write("typeArgs.clear();");
						Write("typeArgs.push_back(\"{}\");", specialization);
						Write("RegisterSpecialization(typeArgs, std::make_unique<Internal{}_{}_Class>({}));",
							klass.name, specName, "");
					}
				}
				LeaveScope();
				NewLine();
				Write("std::unique_ptr<::cct::refl::Object> CreateDefaultObject() const override");
				EnterScope();
				{
					Write("CCT_ASSERT_FALSE(\"Cannot instantiate template class directly\");");
					Write("return nullptr;");
				}
				LeaveScope();
			}
			LeaveScope(";");
			NewLine();
		}
		LeaveScope();
		NewLine();
	}

	void CppGenerator::GenerateClassMethod(std::string_view className, const Class::Method& method, std::string_view ns, std::size_t methodIndex)
	{
		std::string base;
		if (method.tomlAttributes.is_table() && method.tomlAttributes.as_table().contains("Base"))
		{
			auto it = method.tomlAttributes.as_table().find("Base");
			if (it->second.is_string())
				base = it->second.as_string();
			else
				cct::Logger::Error("Invalid base class for method: {}::{}", className, method.name);
		}
		auto baseClass = base.empty() ? "cct::refl::Method"sv : base;
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

					std::string_view type = param.type;
					for (auto& s : {"const"sv})
						if (type.starts_with(s))
							type = type.substr(s.size());

					Write("using Param{0} = {1};", i, type);
					Write("if (parameters[{0}].Is<Param{0}>() == false)", i);
					EnterScope();
					{
						Write("CCT_ASSERT_FALSE(\"Expected '{}' in argument {}\");", param.type, i);
						Write("return {{\"Expected '{}' in argument {}\"s}};", param.type, i);
					}
					LeaveScope();
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
							Write("CCT_ASSERT_FALSE(\"Missing Delegate\");");
							Write("return {{\"Invalid delegate pointer\"s}};");
							LeaveScope();
							Write("auto func = reinterpret_cast<{}(*)({})>(GetCustomDelegate());", method.returnValue, callArgsTypes);
							if (method.returnValue != "void"s)
								Write("return");
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
							Write("if (GetCustomDelegate() == nullptr)");
							EnterScope();
							Write("CCT_ASSERT_FALSE(\"Missing Delegate\");");
							Write("return {{\"Missing Delegate\"s}};");
							LeaveScope();
							Write("auto func = reinterpret_cast<{}(*)({})>(GetCustomDelegate());", method.returnValue, callArgsTypes);
							Write("return Any::Make<{}>(func({}));", method.returnValue, callArgs);
						}
						else
						{
							Write("return Any::Make<{}>(static_cast<{}&>(self).{}({}));", method.returnValue, className, method.name, callArgs);
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

		if (method.tomlAttributes.as_table().contains("Delegate"))
		{
			std::string methodParameterPrototype;
			std::string callArgs;
			std::string callArgsTypes;
			std::size_t i = 0;
			for (auto& param : method.params)
			{
				if (i != 0 && i < method.params.size())
				{
					methodParameterPrototype += ", ";
					callArgs += ", ";
					callArgsTypes += ", ";
				}
				methodParameterPrototype += param.type + ' ' + param.name;
				callArgs += param.name;
				callArgsTypes += param.type;
				++i;
			}
			if (ns.starts_with("::"sv))
				ns.remove_prefix(2);
			Write("{} {}::{}::{}({})", method.returnValue, ns, className, method.name, methodParameterPrototype);
			EnterScope();
			auto delegateIt = method.tomlAttributes.as_table().find("Delegate");
			if (delegateIt->second.is_string())
			{
				auto functionName = method.tomlAttributes.as_table().find("Delegate")->second.as_string();
				if (method.returnValue != "void"s)
					Write("return");
				Write("{}({})", functionName, callArgs);
				Write(";");
			}
			else if (delegateIt->second.is_boolean())
			{
				Write("const auto* methodClass = GetClass()->GetMethod(\"{}\"sv);", method.name);
				Write("if (methodClass == nullptr || methodClass->GetCustomDelegate() == nullptr)");
				EnterScope(),
				Write("CCT_ASSERT_FALSE(\"Missing Delegate\");");
				Write("return");
				if (method.returnValue != "void"s)
					Write("{}{{}}", method.returnValue);
				Write(";");
				LeaveScope();
				Write("auto func = reinterpret_cast<{}(*)({})>(methodClass->GetCustomDelegate());", method.returnValue, callArgsTypes);
				if (method.returnValue != "void"s)
					Write("return");
				Write("func({})", callArgs);
				Write(";");
			}
			LeaveScope();
		}
	}

	void CppGenerator::GenerateEnum(const Enum& enum_, std::string_view ns)
	{

		Write("class Internal{}EnumerationClass : public cct::refl::EnumerationClass", enum_.name);
		EnterScope();
		{
			Write("public:");
			Write("Internal{}EnumerationClass()", enum_.name);
			Write(": cct::refl::EnumerationClass(nullptr, \"{}\"s)", enum_.name);
			EnterScope();
			{
			}
			LeaveScope();

			NewLine();
			Write("~Internal{}EnumerationClass() override", enum_.name);
			EnterScope();
			{
			}
			LeaveScope();

			NewLine();
			Write("void Initialize() override");
			EnterScope();
			{
				for (const auto& elem : enum_.elements)
				{
					Write("AddEnumValue(\"{}\", {});", elem.name, elem.value);
				}
			}
			LeaveScope();

			NewLine();
			Write("cct::refl::Object* GetMemberVariable(std::size_t, const cct::refl::Object&) const override");
			EnterScope();
			{
				Write("return nullptr;");
			}
			LeaveScope();

			NewLine();
			Write("void* GetNativeMemberVariable(std::size_t, const cct::refl::Object&) const override");
			EnterScope();
			{
				Write("return nullptr;");
			}
			LeaveScope();
		}
		LeaveScope(";");
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
				Write("m_namespaces.clear();");
			}
			LeaveScope();

			NewLine();
			Write("void LoadNamespaces() override");
			EnterScope();
			{
				for (auto& ns : pkg.namepsaces)
					Write("AddNamespace(Create{}NamespaceInstance());", ns.name);
				Write("for (auto& ns : m_namespaces)");
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
				Write("for (auto& ns : m_namespaces)");
				EnterScope();
				Write("ns->LoadClasses();");
				LeaveScope();
			}
			LeaveScope();
			NewLine();
			Write("void InitializeClasses() override");
			EnterScope();
			{
				Write("for (auto& ns : m_namespaces)");
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
