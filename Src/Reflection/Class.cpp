//
// Created by arthur on 10/11/2024.
//

#include <ranges>
#include <Concerto/Core/Assert.hpp>

#include "Concerto/Reflection/Class.hpp"
#include "Concerto/Reflection/Namespace.hpp"
#include "Concerto/Reflection/Method.hpp"
#include "Concerto/Reflection/MemberVariable.hpp"

namespace cct::refl
{
	Class::Class(std::shared_ptr<Namespace> nameSpace, std::string name, std::shared_ptr<const Class> baseClass) :
		_name(std::move(name)),
		_namespace(nameSpace ? std::move(nameSpace) : Namespace::GetGlobalNamespace()),
		_baseClass(baseClass ? std::move(baseClass) : nullptr),
		_hash(0)
	{
		_hash = nameSpace ? _namespace->GetHash() : 0;
		_hash |= baseClass ? _baseClass->GetHash() : 0;
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

	const MemberVariable* Class::GetMemberVariable(std::size_t index) const
	{
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

	const Method* Class::GetMethod(std::size_t index) const
	{
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

	bool Class::operator==(const Class& other) const
	{
		return GetHash() == other.GetHash();
	}

	bool Class::operator!=(const Class& other) const
	{
		return !(*this == other);
	}

	void Class::AddMemberVariable(std::string_view name, std::shared_ptr<const Class> type)
	{
	}

	void Class::AddMemberFunction(std::unique_ptr<Method> method)
	{
		CCT_ASSERT(!GetMethod(method->GetName()), "Method already registered");
		_methods.emplace_back(std::move(method));
	}

	void Class::SetNamespace(std::shared_ptr<Namespace> nameSpace)
	{
		_namespace = std::move(nameSpace);
	}

	void Class::SetBaseClass(std::shared_ptr<const Class> klass)
	{
		_baseClass = std::move(klass);
	}

	std::shared_ptr<Namespace> GetNamespaceByName(std::string_view name)
	{
		if (name == Namespace::GetGlobalNamespace()->GetName() || name.empty())
			return Namespace::GetGlobalNamespace();
		return Namespace::GetGlobalNamespace()->GetNamespace(name);
	}

	std::shared_ptr<const Class> GetClassByName(std::string_view nameSpaceName, std::string_view name)
	{
		const auto nameSpace = GetNamespaceByName(nameSpaceName);
		if (nameSpace)
		{
			return nameSpace->GetClass(name);
		}
		CCT_ASSERT_FALSE("Should not happen");
		return nullptr;
	}

	std::shared_ptr<const Class> GetClassByName(std::span<std::string_view> nameSpaceNames, std::string_view name)
	{
		auto nameSpace = Namespace::GetGlobalNamespace()->GetNamespace(nameSpaceNames);
		if (nameSpace)
			return nameSpace->GetClass(name);
		return nullptr;
	}

	std::shared_ptr<const Class> GetClassByName(std::string_view name)
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
