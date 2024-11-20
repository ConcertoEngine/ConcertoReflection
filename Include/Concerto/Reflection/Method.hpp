//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_METHOD_HPP
#define CONCERTO_REFLECTION_METHOD_HPP

#include <string>
#include <string_view>

#include "Concerto/Reflection/Defines.hpp"

namespace cct::refl
{
	class CONCERTO_REFLECTION_API Method
	{
	public:
		Method() = default;
		~Method() = default;

		Method(const Method&) = delete;
		Method(Method&&) = default;

		Method& operator=(const Method&) = delete;
		Method& operator=(Method&&) = default;

		std::string_view GetName() const;
	private:
		std::string _name;
	};
}

#include "Concerto/Reflection/Method.inl"

#endif //CONCERTO_REFLECTION_METHOD_HPP