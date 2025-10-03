//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_OBJECT_HPP
#define CONCERTO_REFLECTION_OBJECT_HPP

#include "Concerto/Reflection/Defines.hpp"
#include <memory>
#include <string_view>

#define CCT_OBJECT(className)						\
	public:											\
		static const cct::refl::Class* GetClass()	\
			{return m_class;}						\
	private:										\
		static const cct::refl::Class* m_class;		\
		friend class Internal##className##Class

struct CCT_REFL_PACKAGE("version = \"1.0.0\"", "description = \"Concerto Reflection Standard Package\"") ConcertoReflection {};

namespace cct::refl
{
	class Class;
	
	class CCT_REFL_CLASS() CCT_REFLECTION_API Object
	{
	public:
		Object() : m_dynamicClass(nullptr) {}
		virtual ~Object() = default;

		static inline bool IsInstanceOf(const Class& klass);

		[[nodiscard]] inline const cct::refl::Object* GetMemberVariable(std::size_t index) const;
		[[nodiscard]] inline cct::refl::Object* GetMemberVariable(std::size_t index);

		[[nodiscard]] inline const cct::refl::Object* GetMemberVariable(std::string_view name) const;
		[[nodiscard]] inline cct::refl::Object* GetMemberVariable(std::string_view name);

		template<typename T> [[nodiscard]] T* GetMemberVariable(std::size_t index);
		template<typename T> [[nodiscard]] const T* GetMemberVariable(std::size_t index) const;

		template<typename T> [[nodiscard]] T* GetMemberVariable(std::string_view name);
		template<typename T>[[nodiscard]] const T* GetMemberVariable(std::string_view name) const;

		[[nodiscard]] inline const cct::refl::Class* GetDynamicClass() const;

		CCT_OBJECT(Object);

	protected:
		const cct::refl::Class* m_dynamicClass;
	};

	class CCT_REFL_CLASS() CCT_REFLECTION_API Int8 : public cct::refl::Object
	{
	public:
		virtual ~Int8() = default;
		CCT_OBJECT(Int8);
	};

	class CCT_REFL_CLASS() CCT_REFLECTION_API Int16 : public cct::refl::Object
	{
	public:
		virtual ~Int16() = default;
		CCT_OBJECT(Int16);
	};

	class CCT_REFL_CLASS() CCT_REFLECTION_API Int32 : public cct::refl::Object
	{
	public:
		Int32() :
			m_value(0)
		{
			m_dynamicClass = m_class;
		}
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

	class CCT_REFL_CLASS() CCT_REFLECTION_API Int64 : public cct::refl::Object
	{
	public:
		virtual ~Int64() = default;
		CCT_OBJECT(Int64);
	};
}


#include "Concerto/Reflection/Object/Object.inl"
#endif //CONCERTO_REFLECTION_OBJECT_HPP