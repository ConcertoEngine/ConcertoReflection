//
// Created by arthur on 10/11/2024.
//

#include "Concerto/Reflection/Object.hpp"

#include "Concerto/Reflection/Class.hpp"
#include "Concerto/Reflection/MemberVariable.hpp"
#include "Concerto/Reflection/Method.hpp"

namespace cct::refl
{
	bool Object::IsInstanceOf(const Class& klass)
	{
		return GetClass()->InheritsFrom(klass);
	}
}
