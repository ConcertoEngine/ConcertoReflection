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
		return m_name;
	}

	inline std::size_t Package::GetClassCount() const
	{
		std::size_t count = 0;
		for (const auto& nameSpace : m_namespaces)
			count += nameSpace->GetClassCount();
		return count;
	}

	inline std::size_t Package::GetNamespaceCount() const
	{
		return m_namespaces.size();
	}
}

#endif //CONCERTO_REFLECTION_PACKAGE_INL
