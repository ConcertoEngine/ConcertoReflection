//
// Created by arthur on 10/11/2024.
//

#include "Concerto/Reflection/Object/Object.hpp"

#include "Concerto/Reflection/Class/Class.hpp"
#include "Concerto/Reflection/MemberVariable/MemberVariable.hpp"
#include "Concerto/Reflection/Method/Method.hpp"

namespace cct::refl
{
	bool Object::IsInstanceOf(const Class& klass)
	{
		return GetClass()->InheritsFrom(klass);
	}
}
