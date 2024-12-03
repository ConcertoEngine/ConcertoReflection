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

	class CCT_REFLECTION_API Int8 : public Object
	{
	public:
		virtual ~Int8() = default;
		CCT_OBJECT(Int8);
	};
	class CCT_REFLECTION_API Int16 : public Object
	{
	public:
		virtual ~Int16() = default;
		CCT_OBJECT(Int16);
	};
		class CCT_REFLECTION_API Int32 : public Object
	{
	public:
		virtual ~Int32() = default;
		CCT_OBJECT(Int32);
	};
	class CCT_REFLECTION_API Int64 : public Object
	{
	public:
		virtual ~Int64() = default;
		CCT_OBJECT(Int64);
	};
}

#endif //CONCERTO_REFLECTION_OBJECT_HPP