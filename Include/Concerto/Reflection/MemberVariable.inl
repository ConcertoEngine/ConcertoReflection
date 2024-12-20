//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_MEMBERVARIABLE_INL
#define CONCERTO_REFLECTION_MEMBERVARIABLE_INL

#include "Concerto/Reflection/MemberVariable.hpp"

namespace cct::refl
{
	inline std::string_view MemberVariable::GetName() const
	{
		return _name;
	}

	inline std::size_t MemberVariable::GetIndex() const
	{
		return _index;
	}
}

#endif //CONCERTO_REFLECTION_MEMBERVARIABLE_INL