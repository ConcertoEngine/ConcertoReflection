//
// Created by arthur on 10/11/2024.
//

#pragma once

#include "Concerto/Reflection/Object/Object.hpp"

#include "Concerto/Reflection/Class/Class.hpp"
#include "Concerto/Reflection/MemberVariable/MemberVariable.hpp"
#include "Concerto/Reflection/Method/Method.hpp"

namespace cct::refl
{
	inline bool Object::IsInstanceOf(const Class& klass)
	{
		return GetClass()->InheritsFrom(klass);
	}

	inline const cct::refl::Object* Object::GetMemberVariable(std::size_t index) const
	{
		return GetDynamicClass()->GetMemberVariable(index, *this);
	}

	inline cct::refl::Object* Object::GetMemberVariable(std::size_t index)
	{
		return GetDynamicClass()->GetMemberVariable(index, *this);
	}

	inline const cct::refl::Object* Object::GetMemberVariable(std::string_view name) const
	{
		return GetDynamicClass()->GetMemberVariable(name, *this);
	}

	inline cct::refl::Object* Object::GetMemberVariable(std::string_view name)
	{
		return GetDynamicClass()->GetMemberVariable(name, *this);
	}

	inline const cct::refl::Class* Object::GetDynamicClass() const
	{
		return m_dynamicClass;
	}

	template <typename T>
	T* Object::GetMemberVariable(std::size_t index)
	{
		Object* object = GetClass()->GetMemberVariable(index, *this);
#ifdef CCT_DEBUG
		if (dynamic_cast<T*>(object) == nullptr) [[unlikely]]
		{
			CCT_ASSERT_FALSE("'{}' is not convertible to '{}'", this->GetClass()->GetName(), T::GetClass()->GetName());
			return nullptr;
		}
#endif
		return static_cast<T*>(object);
	}

	template <typename T>
	const T* Object::GetMemberVariable(std::size_t index) const
	{
		Object* object = GetClass()->GetMemberVariable(index, *this);
#ifdef CCT_DEBUG
		if (dynamic_cast<T*>(object) == nullptr) [[unlikely]]
		{
			CCT_ASSERT_FALSE("'{}' is not convertible to '{}'", this->GetClass()->GetName(), T::GetClass()->GetName());
			return nullptr;
		}
#endif
		return static_cast<T*>(object);
	}

	template <typename T>
	T* Object::GetMemberVariable(std::string_view name)
	{
		const MemberVariable* memberVariable = GetClass()->GetMemberVariable(name);
		if (memberVariable == nullptr)
			return nullptr;
		return GetMemberVariable<T>(memberVariable->GetIndex());
	}

	template <typename T>
	const T* Object::GetMemberVariable(std::string_view name) const
	{
		const MemberVariable* memberVariable = GetClass()->GetMemberVariable(name);
		if (memberVariable == nullptr)
			return nullptr;
		return GetMemberVariable<T>(memberVariable->GetIndex());
	}
}
