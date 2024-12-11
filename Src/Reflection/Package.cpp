//
// Created by arthur on 10/11/2024.
//

#include "Concerto/Reflection/Namespace.hpp"
#include "Concerto/Reflection/Class.hpp"
#include "Concerto/Reflection/Method.hpp"
#include "Concerto/Reflection/MemberVariable.hpp"

#include "Concerto/Reflection/Package.hpp"

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Reflection/GlobalNamespace.hpp"

namespace cct::refl
{
	Package::Package(std::string name) :
		_name(std::move(name))
	{
	}

	Namespace* Package::GetNamespace(std::size_t index) const
	{
		if (index > _namespaces.size())
			return nullptr;
		return _namespaces[index].get();
	}

	Namespace* Package::GetNamespace(std::string_view name) const
	{
		auto it = std::find_if(_namespaces.begin(), _namespaces.end(), [&](const std::unique_ptr<Namespace>& value) -> bool
			{
				return value->GetName() == name;
			});

		if (it != _namespaces.end())
			return it->get();
		return nullptr;
	}

	void Package::AddNamespace(std::unique_ptr<Namespace> nameSpace)
	{
		if (nameSpace == nullptr)
		{
			CCT_ASSERT_FALSE("Namespace is null");
			return;
		}
		GlobalNamespace::Get().AddNamespace(nameSpace.get());
		_namespaces.emplace_back(std::move(nameSpace));
	}

	void Package::AddClass(std::unique_ptr<Class> klass)
	{
		GlobalNamespace::Get().AddClass(klass.get());
		_classes.push_back(std::move(klass));
	}
}
