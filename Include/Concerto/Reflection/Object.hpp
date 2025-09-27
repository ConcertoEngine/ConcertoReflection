//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_OBJECT_HPP
#define CONCERTO_REFLECTION_OBJECT_HPP

#include "Concerto/Reflection/Defines.hpp"
#include <memory>

#define CCT_OBJECT(className)						\
	public:											\
		static const cct::refl::Class* GetClass()	\
			{return _class;}						\
	private:										\
		static const cct::refl::Class* _class;		\
		friend class Internal##className##Class

struct CCT_PACKAGE("version = \"1.0.0\"", "description = \"Concerto Reflection Standard Package\"") ConcertoReflection {};

namespace cct::refl
{
	class Class;
	
	class CCT_CLASS() CCT_REFLECTION_API Object
	{
	public:
		Object() = default;
		virtual ~Object() = default;

		static bool IsInstanceOf(const Class& klass);

		CCT_OBJECT(Object);
	};

	class CCT_CLASS() CCT_REFLECTION_API Int8 : public cct::refl::Object
	{
	public:
		virtual ~Int8() = default;
		CCT_OBJECT(Int8);
	};

	class CCT_CLASS() CCT_REFLECTION_API Int16 : public cct::refl::Object
	{
	public:
		virtual ~Int16() = default;
		CCT_OBJECT(Int16);
	};

	class CCT_CLASS() CCT_REFLECTION_API Int32 : public cct::refl::Object
	{
	public:
		Int32() = default;
		Int32(cct::Int32 value) : m_value(value){}
		~Int32() override = default;

		bool operator==(const Int32& other) const
		{
			return other.m_value == m_value;
		}

		bool operator!=(const Int32& other) const
		{
			return !(other == m_value);
		}

		CCT_OBJECT(Int32);
	private:
		cct::Int32 m_value;
	};

	class CCT_CLASS() CCT_REFLECTION_API Int64 : public cct::refl::Object
	{
	public:
		virtual ~Int64() = default;
		CCT_OBJECT(Int64);
	};
}

#endif //CONCERTO_REFLECTION_OBJECT_HPP