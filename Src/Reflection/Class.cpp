//
// Created by arthur on 10/11/2024.
//

#include <ranges>
#include <string_view>
#include <Concerto/Core/Assert.hpp>

#include "Concerto/Reflection/Class.hpp"
#include "Concerto/Reflection/Namespace.hpp"
#include "Concerto/Reflection/Method.hpp"
#include "Concerto/Reflection/MemberVariable.hpp"
#include "Concerto/Reflection/GlobalNamespace.hpp"

namespace cct::refl
{
	Class::Class(Namespace* nameSpace, std::string name, const Class* baseClass) :
		_name(std::move(name)),
		_namespace(nameSpace),
		_baseClass(baseClass ? baseClass : nullptr),
		_hash(0)
	{
		_hash = _namespace ? _namespace->GetHash() : 0;
		_hash |= _baseClass ? _baseClass->GetHash() : 0;
		_hash |= std::hash<std::string>()(_name);
	}

	std::string_view Class::GetName() const
	{
		return _name;
	}

	std::string_view Class::GetNamespaceName() const
	{
		if (_namespace == nullptr)
		{
			CCT_ASSERT_FALSE("Invalid namespace pointer");
			return {};
		}
		return _namespace->GetName();
	}

	const Namespace& Class::GetNamespace() const
	{
		if (_namespace == nullptr)
		{
			CCT_ASSERT_FALSE("Invalid namespace pointer");
			throw std::runtime_error("Invalid namespace pointer");
		}
		return *_namespace;
	}

	std::size_t Class::GetHash() const
	{
		return _hash;
	}

	std::size_t Class::GetMemberVariableCount() const
	{
		return _memberVariables.size();
	}

	std::size_t Class::GetMethodCount() const
	{
		return _methods.size();
	}

	const Class* Class::GetBaseClass() const
	{
		return _baseClass;
	}

	const MemberVariable* Class::GetMemberVariable(std::size_t index) const
	{
		if (_memberVariables.empty())
			return nullptr;
		if (index > _memberVariables.size())
			return nullptr;
		return _memberVariables[index].get();
	}

	const MemberVariable* Class::GetMemberVariable(std::string_view name) const
	{
		for (const auto& variable : _memberVariables)
		{
			if (variable->GetName() == name)
				return variable.get();
		}
		return nullptr;
	}

	cct::refl::Object* Class::GetMemberVariable(std::string_view name, cct::refl::Object& self) const
	{
		auto* memberVariable = GetMemberVariable(name);
		if (memberVariable == nullptr)
			return nullptr;
		return GetMemberVariable(memberVariable->GetIndex(), self);
	}

	const Method* Class::GetMethod(std::size_t index) const
	{
		if (_methods.empty())
			return nullptr;
		if (index > _methods.size())
			return nullptr;
		return _methods[index].get();
	}

	const Method* Class::GetMethod(std::string_view name) const
	{
		for (const auto& method : _methods)
		{
			if (method->GetName() == name)
				return method.get();
		}
		return nullptr;
	}

	bool Class::HasMemberVariable(std::string_view name) const
	{
		return GetMemberVariable(name) != nullptr;
	}

	bool Class::HasMethod(std::string_view name) const
	{
		return GetMethod(name) != nullptr;
	}

	bool Class::InheritsFrom(const Class& other) const
	{
		if (!_baseClass)
		{
			CCT_ASSERT_FALSE("Invalid base class, it should at least derive frol cct::refl::Object");
			return false;
		}
		if (other == *_baseClass)
			return true;
		return _baseClass->InheritsFrom(other);
	}

	bool Class::InheritsFrom(std::string_view name) const
	{
		if (!_baseClass)
		{
			CCT_ASSERT_FALSE("Invalid base class, it should at least derive frol cct::refl::Object");
			return false;
		}
		if (_baseClass->GetName() == name)
			return true;
		return _baseClass->InheritsFrom(name);
	}

	bool Class::HasAttribute(std::string_view attribute) const
	{
		//not using "contains", because it does not support std::string_view
		auto it = std::find_if(_attributes.begin(), _attributes.end(), [&](const std::pair<std::string, std::string>& value) -> bool
			{
				return "attribute" == value.first;
			});
		return it != _attributes.end();
	}

	std::string_view Class::GetAttribute(std::string_view attribute)
	{
		// not using "contains", because it does not support std::string_view
		auto it = std::find_if(_attributes.begin(), _attributes.end(), [&](const std::pair<std::string, std::string>& value) -> bool
			{
				return "attribute" == value.first;
			});
		if (it == _attributes.end())
		{
			CCT_ASSERT_FALSE("Attribute '{}' does not exist", attribute);
			return {};
		}
		return it->second;
	}

	bool Class::operator==(const Class& other) const
	{
		return GetHash() == other.GetHash();
	}

	bool Class::operator!=(const Class& other) const
	{
		return !(*this == other);
	}

	void Class::AddMemberVariable(std::string_view name, const Class* type)
	{
		CCT_ASSERT(!GetMemberVariable(name), "Member variable already exists");
		_memberVariables.emplace_back(std::make_unique<MemberVariable>(std::string(name), type, _memberVariables.size()));
	}

	void Class::AddMemberFunction(std::unique_ptr<Method> method)
	{
		CCT_ASSERT(!GetMethod(method->GetName()), "Method already registered");
		_methods.emplace_back(std::move(method));
	}

	void Class::AddAttribute(std::string name, std::string value)
	{
		CCT_ASSERT(HasAttribute(name), "Class attribute already exist");
		_attributes.emplace(std::move(name), std::move(value));
	}

	void Class::SetNamespace(Namespace* nameSpace)
	{
		_namespace = nameSpace;
	}

	void Class::SetBaseClass(const Class* klass)
	{
		_baseClass = klass;
	}

	const Class* GetClassByName(std::string_view nameSpaceName, std::string_view name)
	{
		using namespace std::string_view_literals;
		if (nameSpaceName.empty() || nameSpaceName == "::"sv)
			return GlobalNamespace::Get().GetClassByName(name);
		const auto nameSpace = GlobalNamespace::Get().GetNamespaceByName(nameSpaceName);
		if (nameSpace)
			return nameSpace->GetClass(name);
		return nullptr;
	}

	const Class* GetClassByName(std::span<std::string_view> nameSpaceNames, std::string_view name)
	{
		auto nameSpace = GlobalNamespace::Get().GetNamespaceByName(nameSpaceNames);
		if (nameSpace)
			return nameSpace->GetClass(name);
		return nullptr;
	}

	const Class* GetClassByName(std::string_view name)
	{
		using namespace std::string_view_literals;
		auto split = name | std::ranges::views::split("::"sv);

		std::vector<std::string_view> res;
		for (const auto elem : split)
			res.emplace_back(elem.data(), elem.size());

		if (res.empty())
		{
			CCT_ASSERT_FALSE("Invalid class: {}", name);
			return nullptr;
		}

		if (res.size() == 1)
			return GetClassByName("", res[0]);

		std::string_view klass  = res.back();
		res.pop_back();
		return GetClassByName(res, klass);
	}
}
