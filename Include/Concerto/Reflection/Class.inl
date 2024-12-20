//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_CLASS_INL
#define CONCERTO_REFLECTION_CLASS_INL

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Reflection/Class.hpp"
#include "Concerto/Reflection/Object.hpp"

namespace cct::refl
{
	template <typename T>
	requires (std::is_base_of_v<cct::refl::Object, T> && std::is_polymorphic_v<T>)
	std::unique_ptr<T> Class::CreateDefaultObject() const
	{
		if (*T::GetClass() != *this)
		{
			CCT_ASSERT_FALSE("Invalid class");
			return nullptr;
		}
		if (!T::GetClass()->InheritsFrom(*cct::refl::Object::GetClass()))
		{
			CCT_ASSERT_FALSE("Trying to create object '{}' but it does not inherits from 'Class'", T::GetClass()->GetName());
			return nullptr;
		}
		Object* obj = CreateDefaultObject().release();
		if (!obj)
		{
			CCT_ASSERT_FALSE("Invalid pointer");
			return nullptr;
		}
		
		return std::unique_ptr<T>(reinterpret_cast<T*>(obj));
	}
}

#endif // CONCERTO_REFLECTION_CLASS_INL