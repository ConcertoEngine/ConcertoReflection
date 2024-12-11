//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_MEMBERVARIABLE_HPP
#define CONCERTO_REFLECTION_MEMBERVARIABLE_HPP

#include <string>
#include <memory>

#include "Concerto/Reflection/Defines.hpp"

namespace cct::refl
{
	class Class;
	class CCT_REFLECTION_API MemberVariable
	{
	public:
		MemberVariable(std::string name, const Class* type, std::size_t index);
		~MemberVariable() = default;

		MemberVariable(const MemberVariable&) = delete;
		MemberVariable(MemberVariable&&) = default;

		MemberVariable& operator=(const MemberVariable&) = delete;
		MemberVariable& operator=(MemberVariable&&) = default;

		[[nodiscard]] inline std::string_view GetName() const;
		[[nodiscard]] inline std::size_t GetIndex() const;

	private:
		std::string _name;
		std::size_t _index;
		const Class* _type;
	};
}

#include "Concerto/Reflection/MemberVariable.inl"

#endif //CONCERTO_REFLECTION_MEMBERVARIABLE_HPP