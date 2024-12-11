//
// Created by arthur on 11/12/2024.
//

#ifndef CONCERTO_REFLECTION_GLOBALNAMESPACE_HPP
#define CONCERTO_REFLECTION_GLOBALNAMESPACE_HPP

#include <vector>
#include <span>

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Reflection/Defines.hpp"

namespace cct::refl
{
	class Class;
	class Namespace;

	class CCT_REFLECTION_API GlobalNamespace
	{
	public:
		GlobalNamespace() = default;
		static GlobalNamespace& Get();

		std::size_t GetNamespaceCount() const;
		std::size_t GetClassCount() const;

		Namespace* GetNamespaceByName(std::string_view nameSpaceName) const;
		Namespace* GetNamespaceByName(std::span<std::string_view> names) const;

		//These functions are only used inside a generated package
		void LoadNamespaces() const;
		void LoadClasses() const;
		void InitializeClasses() const;
		void AddNamespace(Namespace* namespace_);
		void AddClass(const Class* klass);
		void RemoveNamespace(std::string_view name);
		void RemoveClass(std::string_view name);
	private:
		std::vector<Namespace*> _namespaces;
		std::vector<const Class*> _classes;
		static std::unique_ptr<GlobalNamespace> _globalNamespace;
	};
}

#endif //CONCERTO_REFLECTION_GLOBALNAMESPACE_HPP
