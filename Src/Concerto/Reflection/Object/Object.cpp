//
// Created by arthur on 10/11/2024.
//

#include "Concerto/Reflection/Object/Object.hpp"

#include "Concerto/Reflection/Class/Class.hpp"
#include "Concerto/Reflection/MemberVariable/MemberVariable.hpp"
#include "Concerto/Reflection/Method/Method.hpp"

namespace cct::refl
{
	Object::Object() : m_dynamicClass(nullptr)
	{
	}

	Object::Object(const Object& other)
	{
		m_dynamicClass = other.m_dynamicClass;
	}

	Object::Object(Object&& other) noexcept
	{
		m_dynamicClass = std::exchange(other.m_dynamicClass, nullptr);
	}

	Object& Object::operator=(const Object& other)
	{
		m_dynamicClass = other.m_dynamicClass;

		return *this;
	}

	Object& Object::operator=(Object&& other) noexcept
	{
		std::swap(m_dynamicClass, other.m_dynamicClass);

		return *this;
	}
}
