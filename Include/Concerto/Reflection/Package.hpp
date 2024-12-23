//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_PACKAGE_HPP
#define CONCERTO_REFLECTION_PACKAGE_HPP

#include <memory>
#include <string_view>
#include <string>
#include <vector>

#include "Concerto/Reflection/Defines.hpp"

namespace cct::refl
{
	class Object;
	class Namespace;

	class CCT_REFLECTION_API Package
	{
	public:
		Package(std::string name);
		virtual ~Package() = default;

		Package(const Package&) = delete;
		Package(Package&&) = default;

		Package& operator=(const Package&) = delete;
		Package& operator=(Package&&) = default;


		[[nodiscard]] inline std::string_view GetName() const;
		[[nodiscard]] inline std::size_t GetClassCount() const;
		[[nodiscard]] inline std::size_t GetNamespaceCount() const;

		[[nodiscard]] Namespace* GetNamespace(std::size_t index) const;
		[[nodiscard]] Namespace* GetNamespace(std::string_view name) const;

		virtual void LoadNamespaces() = 0;
		virtual void InitializeNamespaces() = 0;
		virtual void InitializeClasses() = 0;
	protected:
		void AddNamespace(std::unique_ptr<Namespace> nameSpace);
		void AddClass(std::unique_ptr<Class> klass);
		std::string _name;
		std::vector<std::unique_ptr<Namespace>> _namespaces;
		std::vector<std::unique_ptr<Class>> _classes;
		friend class Namespace;
	};
}

#include "Concerto/Reflection/Package.inl"

#endif //CONCERTO_REFLECTION_PACKAGE_HPP
