//
// Created by arthur on 10/11/2024.
//

#include "Concerto/Reflection/Class.hpp"

namespace cct::refl
{
	template <typename T>
	std::unique_ptr<T> Class::CreateInstance() const
	{
		return std::unique_ptr<T>(static_cast<T*>(CreateInstance().release()));
	}

	template <typename T>
	bool Class::InheritsFrom() const
	{
		return false;
	}
}
