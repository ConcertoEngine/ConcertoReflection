//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_PACKAGE_HPP
#define CONCERTO_REFLECTION_PACKAGE_HPP

#include <memory>
#include <optional>
#include <string_view>
#include <vector>

#include "Concerto/Reflection/Defines.hpp"

namespace cct::refl
{
	class Class;
	class Object;
	class Namespace;

	class CONCERTO_REFLECTION_API Package
	{
	public:
		inline Package(std::string name);
		~Package() = default;

		inline [[nodiscard]] std::string_view GetName() const;
		[[nodiscard]] std::size_t GetClassCount() const;
		[[nodiscard]] std::size_t GetNamespaceCount() const;

		[[nodiscard]] const Namespace* GetNamespace(std::size_t index) const;
		[[nodiscard]] const Namespace* GetNamespace(std::string_view name) const;
	private:
		std::string _name;
		std::vector<std::unique_ptr<Namespace>> _namespaces;
	};
}

#endif //CONCERTO_REFLECTION_PACKAGE_HPP
