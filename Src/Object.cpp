//
// Created by arthur on 10/11/2024.
//

#include "Concerto/Reflection/Object.hpp"

namespace cct::refl
{
	const std::shared_ptr<const Class>& Object::GetClass() const
	{
		return _class;
	}
}
