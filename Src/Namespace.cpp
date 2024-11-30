//
// Created by arthur on 10/11/2024.
//

#include "Concerto/Reflection/Class.hpp"
#include "Concerto/Reflection/Method.hpp"
#include "Concerto/Reflection/MemberVariable.hpp"

#include "Concerto/Reflection/Namespace.hpp"

namespace cct::refl
{
	std::shared_ptr<const Class> Namespace::GetClass(std::size_t index) const
	{
		if (index > GetClassCount())
			return nullptr;
		return _classes[index];
	}

	std::shared_ptr<const Class> Namespace::GetClass(std::string_view name) const
	{
		auto it = std::find_if(_classes.begin(), _classes.end(), [&](const std::shared_ptr<const Class>& value) -> bool
			{
				return value->GetName() == name;
			});

		if (it != _classes.end())
			return *it;
		return nullptr;
	}

	std::shared_ptr<const Class> Namespace::AddClass(std::string name, std::shared_ptr<const Class> baseClass)
	{
		auto klass = std::make_shared<Class>(shared_from_this(), std::move(name), std::move(baseClass));
		_classes.emplace_back(klass);
		return klass;
	}
}
