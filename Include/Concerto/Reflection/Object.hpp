//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_OBJECT_HPP
#define CONCERTO_REFLECTION_OBJECT_HPP

#include <memory>

#include "Concerto/Reflection/Defines.hpp"

#define CCT_OBJECT(className)										\
	public:															\
		static std::shared_ptr<const cct::refl::Class> GetClass()	\
			{return _class;}										\
	private:														\
		static std::shared_ptr<cct::refl::Class> _class;			\
		friend class Internal##className##Class

namespace cct::refl
{
	class Class;
	class CCT_REFLECTION_API Object
	{
	public:
		virtual ~Object() = default;
		CCT_OBJECT(Object);
	};
}

#endif //CONCERTO_REFLECTION_OBJECT_HPP