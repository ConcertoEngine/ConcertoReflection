//
// Created by arthur on 11/12/2024.
//

#include "Concerto/Reflection/Class.hpp"
#include "Concerto/Reflection/MemberVariable.hpp"
#include "Concerto/Reflection/Method.hpp"
#include "Concerto/Reflection/Namespace.hpp"

#include "Concerto/Reflection/GlobalNamespace.hpp"

#include <ranges>

namespace cct::refl
{
	std::unique_ptr<GlobalNamespace> GlobalNamespace::_globalNamespace = std::make_unique<GlobalNamespace>();

	GlobalNamespace& GlobalNamespace::Get()
	{
		CCT_ASSERT(_globalNamespace, "Invalid pointer");
		return *_globalNamespace;
	}

	Namespace* GlobalNamespace::GetNamespaceByName(std::string_view name) const
	{
		using namespace std::string_view_literals;
		if (name.empty())
			return nullptr;

		if (name == "::"sv)
			name = name.substr(2);

		using namespace std::string_view_literals;
		auto split = (name.starts_with("::"sv) ? name.substr(2) : name) | std::ranges::views::split("::"sv);

		std::vector<std::string_view> res;
		for (const auto elem : split)
			res.emplace_back(elem.data(), elem.size());

		if (res.empty())
		{
			CCT_ASSERT_FALSE("Invalid class: {}", name);
			return nullptr;
		}

		if (res.size() == 1)
		{
			for (auto* ns : _namespaces)
			{
				if (ns->GetName() == res[0])
					return ns;
			}
		}

		return GetNamespaceByName(res);
	}

	Namespace* GlobalNamespace::GetNamespaceByName(std::span<std::string_view> names) const
	{
		using namespace std::string_view_literals;
		if (names.empty())
			return nullptr;
		if (names[0] == "::"sv)
			names = names.subspan(1);

		for (auto* ns : _namespaces)
		{
			if (ns->GetName() == names[0])
				return ns->GetNamespace(names.subspan(1));
		}
		return nullptr;
	}

	void GlobalNamespace::LoadNamespaces() const
	{
		for (auto& nameSpace : _namespaces)
		{
			nameSpace->LoadNamespaces();
		}
	}

	void GlobalNamespace::LoadClasses() const
	{
		for (auto& nameSpace : _namespaces)
		{
			CCT_ASSERT(nameSpace, "Invalid namespace pointer");
			nameSpace->LoadClasses();
		}
	}

	void GlobalNamespace::InitializeClasses() const
	{
		for (auto& nameSpace : _namespaces)
		{
			CCT_ASSERT(nameSpace, "Invalid namespace pointer");
			nameSpace->InitializeClasses();
		}
	}

	void GlobalNamespace::AddNamespace(Namespace* namespace_)
	{
		_namespaces.push_back(namespace_);
	}

	void GlobalNamespace::AddClass(const Class* klass)
	{
		_classes.push_back(klass);
	}
}
