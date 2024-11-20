//
// Created by arthur on 10/11/2024.
//

#include "Concerto/Reflection/Package.hpp"
#include "Concerto/Reflection/Namespace.hpp"

namespace cct::refl
{
	Package::Package(std::string name) :
		_name(std::move(name))
	{
	}

	const Namespace* Package::GetNamespace(std::size_t index) const
	{
		if (index > _namespaces.size())
			return nullptr;
		return _namespaces[index].get();
	}

	const Namespace* Package::GetNamespace(std::string_view name) const
	{
		auto it = std::find_if(_namespaces.begin(), _namespaces.end(), [&](const std::unique_ptr<Namespace>& value) -> bool
			{
				return value->GetName() == name;
			});

		if (it != _namespaces.end())
			return it->get();
		return nullptr;
	}
}
