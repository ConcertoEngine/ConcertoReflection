//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_NAMESPACE_INL
#define CONCERTO_REFLECTION_NAMESPACE_INL

#include "Concerto/Reflection/Namespace.hpp"
#include "Concerto/Reflection/Class.hpp"

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

	inline const Class* Namespace::GetClass(std::size_t index) const
	{
		if (index > GetClassCount())
			return nullptr;
		return _classes[index].get();
	}

	inline const Class* Namespace::GetClass(std::string_view name) const
	{
		auto it = std::find_if(_classes.begin(), _classes.end(), [&](const std::unique_ptr<Namespace>& value) -> bool
		{
			return value->GetName() == name;
		});

		if (it != _classes.end())
			return it->get();
		return nullptr;
	}

	inline bool Namespace::HasClass(std::string_view name) const
	{
		return GetClass(name) != nullptr;
	}
}

#endif //CONCERTO_REFLECTION_NAMESPACE_INL