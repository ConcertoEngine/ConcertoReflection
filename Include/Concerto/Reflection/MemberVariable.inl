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
		return m_name;
	}

	inline std::size_t MemberVariable::GetIndex() const
	{
		return m_index;
	}
}

#endif //CONCERTO_REFLECTION_MEMBERVARIABLE_INL