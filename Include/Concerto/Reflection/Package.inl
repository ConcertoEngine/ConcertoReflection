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
}

#endif //CONCERTO_REFLECTION_PACKAGE_INL
