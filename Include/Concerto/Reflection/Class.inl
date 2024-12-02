//
// Created by arthur on 10/11/2024.
//

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Reflection/Class.hpp"
#include "Concerto/Reflection/Object.hpp"

namespace cct::refl
{
	template <typename T>
	bool Class::InheritsFrom(const Class& klass) const
	{
		return false;
	}

	template <typename T>
	std::unique_ptr<T> Class::CreateDefaultObject() const
	{
		if (!T::GetClass()->InheritsFrom(*this))
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
