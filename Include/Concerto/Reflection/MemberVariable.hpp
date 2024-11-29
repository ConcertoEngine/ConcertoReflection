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
	class CONCERTO_REFLECTION_API MemberVariable
	{
	public:
		MemberVariable(std::string name, std::shared_ptr<Class> type);
		~MemberVariable() = default;

		MemberVariable(const MemberVariable&) = delete;
		MemberVariable(MemberVariable&&) = default;

		MemberVariable& operator=(const MemberVariable&) = delete;
		MemberVariable& operator=(MemberVariable&&) = default;

		[[nodiscard]] inline std::string_view GetName() const;

	private:
		std::string _name;
		std::shared_ptr<Class> _type;
	};
}

#include "Concerto/Reflection/MemberVariable.inl"

#endif //CONCERTO_REFLECTION_MEMBERVARIABLE_HPP