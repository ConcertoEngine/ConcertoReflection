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

	class CONCERTO_REFLECTION_API Package
	{
	public:
		Package(std::string name);
		~Package() = default;

		Package(const Package&) = delete;
		Package(Package&&) = default;

		Package& operator=(const Package&) = delete;
		Package& operator=(Package&&) = default;


		[[nodiscard]] inline std::string_view GetName() const;
		[[nodiscard]] inline std::size_t GetClassCount() const;
		[[nodiscard]] inline std::size_t GetNamespaceCount() const;

		[[nodiscard]] const Namespace* GetNamespace(std::size_t index) const;
		[[nodiscard]] const Namespace* GetNamespace(std::string_view name) const;
	protected:
		std::shared_ptr<Namespace> AddNamespace(std::string name);
	private:
		std::string _name;
		std::vector<std::shared_ptr<Namespace>> _namespaces;
		friend class Namespace;
	};
}

#include "Concerto/Reflection/Package.inl"

#endif //CONCERTO_REFLECTION_PACKAGE_HPP
