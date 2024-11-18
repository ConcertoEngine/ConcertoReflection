//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_NAMESPACE_HPP
#define CONCERTO_REFLECTION_NAMESPACE_HPP

#include <string>
#include <vector>
#include <memory>

#include "Concerto/Reflection/Defines.hpp"

namespace cct::refl
{
	class Class;
	class CONCERTO_REFLECTION_API Namespace
	{
	public:
		Namespace(std::string name);

		inline [[nodiscard]] std::string_view GetName() const;
		inline [[nodiscard]] std::size_t GetClassCount() const;
		inline [[nodiscard]] const Class* GetClass(std::size_t index) const;
		inline [[nodiscard]] const Class* GetClass(std::string_view name) const;

		[[nodiscard]] bool HasClass(std::string_view name) const;
	private:
		std::string _name;
		std::vector<std::unique_ptr<Class>> _classes;
	};
}

#endif //CONCERTO_REFLECTION_NAMESPACE_HPP