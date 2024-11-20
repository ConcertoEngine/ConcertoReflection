//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_MEMBERVARIABLE_HPP
#define CONCERTO_REFLECTION_MEMBERVARIABLE_HPP

#include <string>
#include <string_view>

#include "Concerto/Reflection/Defines.hpp"

namespace cct::refl
{
	class CONCERTO_REFLECTION_API MemberVariable
	{
	public:
		MemberVariable(std::string name);
		~MemberVariable() = default;

		MemberVariable(const MemberVariable&) = delete;
		MemberVariable(MemberVariable&&) = default;

		MemberVariable& operator=(const MemberVariable&) = delete;
		MemberVariable& operator=(MemberVariable&&) = default;

		[[nodiscard]] inline std::string_view GetName() const;

	private:
		std::string _name;
	};
}

#include "Concerto/Reflection/MemberVariable.inl"

#endif //CONCERTO_REFLECTION_MEMBERVARIABLE_HPP