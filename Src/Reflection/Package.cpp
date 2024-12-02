//
// Created by arthur on 10/11/2024.
//

#include "Concerto/Reflection/Namespace.hpp"
#include "Concerto/Reflection/Class.hpp"
#include "Concerto/Reflection/Method.hpp"
#include "Concerto/Reflection/MemberVariable.hpp"

#include "Concerto/Reflection/Package.hpp"

#include <Concerto/Core/Assert.hpp>

namespace cct::refl
{
	Package::Package(std::string name) :
		_name(std::move(name))
	{
	}

	std::shared_ptr<Namespace> Package::GetNamespace(std::size_t index) const
	{
		if (index > _namespaces.size())
			return nullptr;
		return _namespaces[index];
	}

	std::shared_ptr<Namespace> Package::GetNamespace(std::string_view name) const
	{
		auto it = std::find_if(_namespaces.begin(), _namespaces.end(), [&](const std::shared_ptr<Namespace>& value) -> bool
			{
				return value->GetName() == name;
			});

		if (it != _namespaces.end())
			return *it;
		return nullptr;
	}

	void Package::AddNamespace(std::shared_ptr<Namespace> nameSpace)
	{
		if (nameSpace == nullptr)
		{
			CCT_ASSERT_FALSE("Namespace is null");
			return;
		}
		_namespaces.emplace_back(nameSpace);
		GlobalNamespace::Get()->AddNamespace(nameSpace);
	}
}
