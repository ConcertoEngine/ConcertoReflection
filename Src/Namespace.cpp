//
// Created by arthur on 10/11/2024.
//

#include "Concerto/Reflection/Class.hpp"
#include "Concerto/Reflection/Method.hpp"
#include "Concerto/Reflection/MemberVariable.hpp"

#include "Concerto/Reflection/Namespace.hpp"

namespace cct::refl
{
	const Class* Namespace::GetClass(std::size_t index) const
	{
		if (index > GetClassCount())
			return nullptr;
		return _classes[index].get();
	}

	const Class* Namespace::GetClass(std::string_view name) const
	{
		auto it = std::find_if(_classes.begin(), _classes.end(), [&](const std::unique_ptr<Class>& value) -> bool
			{
				return value->GetName() == name;
			});

		if (it != _classes.end())
			return it->get();
		return nullptr;
	}
}
