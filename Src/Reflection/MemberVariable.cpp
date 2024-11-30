//
// Created by arthur on 10/11/2024.
//

#include "Concerto/Reflection/MemberVariable.hpp"

namespace cct::refl
{
	MemberVariable::MemberVariable(std::string name, std::shared_ptr<Class> type) :
		_name(std::move(name)),
		_type(std::move(type))
	{
	}
}
