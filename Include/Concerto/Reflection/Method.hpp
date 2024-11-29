//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_METHOD_HPP
#define CONCERTO_REFLECTION_METHOD_HPP

#include <string>
#include <memory>
#include <vector>

#include "Concerto/Reflection/Defines.hpp"

namespace cct::refl
{
	class Class;
	class CONCERTO_REFLECTION_API Method
	{
	public:
		Method(std::string_view name, std::shared_ptr<const Class> returnValue, std::vector<std::shared_ptr<const Class>> parameters);
		~Method() = default;

		Method(const Method&) = delete;
		Method(Method&&) = default;

		Method& operator=(const Method&) = delete;
		Method& operator=(Method&&) = default;

		[[nodiscard]] std::string_view GetName() const;
		[[nodiscard]] std::shared_ptr<const Class> GetReturnValue() const;
		[[nodiscard]] std::vector<std::shared_ptr<const Class>> GetParameters() const;
	private:
		std::string _name;
		std::shared_ptr<const Class> _returnValue;
		std::vector<std::shared_ptr<const Class>> _parameters;
	};
}

#include "Concerto/Reflection/Method.inl"

#endif //CONCERTO_REFLECTION_METHOD_HPP