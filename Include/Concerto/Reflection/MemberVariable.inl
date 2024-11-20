//
// Created by arthur on 10/11/2024.
//

#include "Concerto/Reflection/MemberVariable.hpp"

namespace cct::refl
{
	inline std::string_view MemberVariable::GetName() const
	{
		return _name;
	}
}
