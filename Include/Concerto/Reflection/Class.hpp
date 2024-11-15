//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_CLASS_HPP
#define CONCERTO_REFLECTION_CLASS_HPP

#include "Concerto/Reflection/Defines.hpp"

namespace cct::refl
{
	class CONCERTO_REFLECTION_API Class
	{
	public:
		virtual ~Class() = default;

		virtual std::string_view GetName() const = 0;
		virtual std::string_view GetNamespaceName() const = 0;
		virtual const Namespace& GetNamespace() const = 0;

		virtual std::size_t GetMemberVariableCount() const = 0;
		virtual std::size_t GetMethodCount() const = 0;

		virtual const MemberVariable& GetMemberVariable(std::size_t index) const = 0;
		virtual const MemberVariable& GetMemberVariable(std::string_view name) const = 0;

		virtual const Method& GetMethod(std::size_t index) const = 0;
		virtual const Method& GetMethod(std::string_view name) const = 0;

		virtual bool HasMemberVariable(std::string_view name) const = 0;
		virtual bool HasMethod(std::string_view name) const = 0;

		std::unique_ptr<Object> CreateInstance() const = 0;

		template<typename T>
		std::unique_ptr<T> CreateInstance() const
		{
			return std::unique_ptr<T>(static_cast<T*>(CreateInstance().release()));
		}

		bool InheritsFrom(const Class& other) const;
		bool InheritsFrom(std::string_view name) const;

		template<typename T>
		bool InheritsFrom() const
		{
			return InheritsFrom(T::GetStaticClass());
		}
	private:
		std::string _name;
		Namespace* _namespace;
		std::vector<MemberVariable> _memberVariables;
		std::vector<Method> _methods;
		Class* _baseClasse;
	};
}

#endif //CONCERTO_REFLECTION_CLASS_HPP