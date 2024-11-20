//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_NAMESPACE_INL
#define CONCERTO_REFLECTION_NAMESPACE_INL

#include "Concerto/Reflection/Class.hpp"
#include "Concerto/Reflection/Namespace.hpp"

namespace cct::refl
{
	inline Namespace::Namespace(std::string name) :
		_name(std::move(name)),
		_hash(std::hash<std::string>()(name))
	{
	}

	inline std::string_view Namespace::GetName() const
	{
		return _name;
	}

	inline std::size_t Namespace::GetHash() const
	{
		return _hash;
	}

	inline std::size_t Namespace::GetClassCount() const
	{
		return _classes.size();
	}

	inline bool Namespace::HasClass(std::string_view name) const
	{
		return GetClass(name) != nullptr;
	}
}

#endif //CONCERTO_REFLECTION_NAMESPACE_INL