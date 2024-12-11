//
// Created by arthur on 10/11/2024.
//

#include "Concerto/Reflection/MemberVariable.hpp"

namespace cct::refl
{
	MemberVariable::MemberVariable(std::string name, const Class* type, std::size_t index) :
		_name(std::move(name)),
		_index(index),
		_type(type)
	{
	}
}
