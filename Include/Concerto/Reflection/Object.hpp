//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_OBJECT_HPP
#define CONCERTO_REFLECTION_OBJECT_HPP

#include <memory>

#include "Concerto/Reflection/Defines.hpp"

#define CCT_OBJECT(className)									\
		static std::shared_ptr<cct::refl::Class> _class;		\
		friend class Internal##className##Class;				\

namespace cct::refl
{
	class Class;
	class CCT_REFLECTION_API Object
	{
	public:
		virtual ~Object() = default;
		const std::shared_ptr<const Class>& GetClass() const;
	private:
		std::shared_ptr<const Class> _class;
	};
}

#endif //CONCERTO_REFLECTION_OBJECT_HPP