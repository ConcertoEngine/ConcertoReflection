//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_PACKAGE_INL
#define CONCERTO_REFLECTION_PACKAGE_INL

#include "Concerto/Reflection/Package.hpp"
#include "Concerto/Reflection/Namespace.hpp"

namespace cct::refl
{
	inline std::string_view Package::GetName() const
	{
		return _name;
	}

	inline std::size_t Package::GetClassCount() const
	{
		std::size_t count = 0;
		for (const auto& nameSpace : _namespaces)
			count += nameSpace->GetClassCount();
		return count;
	}

	inline std::size_t Package::GetNamespaceCount() const
	{
		return _namespaces.size();
	}

	inline const Namespace* Package::GetNamespace(std::size_t index) const
	{
		if (index > _namespaces.size())
			return nullptr;
		return _namespaces[index].get();
	}

	inline const Namespace* Package::GetNamespace(std::string_view name) const
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

#endif //CONCERTO_REFLECTION_PACKAGE_INL
