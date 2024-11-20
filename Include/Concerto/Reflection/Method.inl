//
// Created by arthur on 10/11/2024.
//

#include "Concerto/Reflection/Method.hpp"

namespace cct::refl
{
	inline std::string_view Method::GetName() const
	{
		return _name;
	}
}
