//
// Created by arthur on 10/11/2024.
//

#include "Concerto/Reflection/Class.hpp"
#include "Concerto/Reflection/Method.hpp"
#include "Concerto/Reflection/MemberVariable.hpp"

#include "Concerto/Reflection/Namespace.hpp"

namespace cct::refl
{
	using namespace std::string_literals;
	std::shared_ptr<GlobalNamespace> GlobalNamespace::_globalNameSpace = std::make_shared<GlobalNamespace>();

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
		CONCERTO_ASSERT_FALSE("Could bot find class {}", name);
		return nullptr;
	}

	void Namespace::AddClass(std::shared_ptr<Class> klass)
	{
		_classes.emplace_back(std::move(klass));
	}

//GlobalNamespace class
	GlobalNamespace::GlobalNamespace() : Namespace("Global"s)
	{
	}

	void GlobalNamespace::LoadClasses()
	{
		for (auto& nameSpace : _namespaces)
		{
			nameSpace->LoadClasses();
		}
	}

	void GlobalNamespace::InitializeClasses()
	{
		for (auto& nameSpace : _namespaces)
		{
			nameSpace->InitializeClasses();
		}
	}

	void GlobalNamespace::AddNamespace(std::shared_ptr<Namespace> nameSpace)
	{
		_namespaces.emplace_back(std::move(nameSpace));
	}

	std::shared_ptr<Namespace> GlobalNamespace::GetNamespace(std::string_view name)
	{
		for (auto& nameSpace : _namespaces)
		{
			if (nameSpace->GetName() == name)
				return nameSpace;
		}
		return nullptr;
	}
}
