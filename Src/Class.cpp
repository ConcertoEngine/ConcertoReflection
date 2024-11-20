//
// Created by arthur on 10/11/2024.
//

#include <stdexcept>

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Reflection/Class.hpp"
#include "Concerto/Reflection/Namespace.hpp"
#include "Concerto/Reflection/Method.hpp"
#include "Concerto/Reflection/MemberVariable.hpp"

namespace cct::refl
{
	Class::Class(Namespace* nameSpace, std::string name, Class* baseClass) :
		_name(std::move(name)),
		_namespace(nameSpace),
		_baseClass(baseClass),
		_hash(0)
	{
		_hash = nameSpace ? nameSpace->GetHash() : 0;
		_hash |= baseClass ? baseClass->GetHash() : 0;
	}

	std::string_view Class::GetName() const
	{
		return _name;
	}

	std::string_view Class::GetNamespaceName() const
	{
		if (_namespace == nullptr)
		{
			CONCERTO_ASSERT_FALSE("Invalid namespace pointer");
			return {};
		}
		return _namespace->GetName();
	}

	const Namespace& Class::GetNamespace() const
	{
		if (_namespace == nullptr)
		{
			CONCERTO_ASSERT_FALSE("Invalid namespace pointer");
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
			CONCERTO_ASSERT_FALSE("Invalid base class, it should at least derive frol cct::refl::Object");
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
			CONCERTO_ASSERT_FALSE("Invalid base class, it should at least derive frol cct::refl::Object");
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
}
