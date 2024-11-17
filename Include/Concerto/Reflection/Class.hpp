//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_CLASS_HPP
#define CONCERTO_REFLECTION_CLASS_HPP

#include <string>
#include <string_view>
#include <memory>
#include <vector>

#include "Concerto/Reflection/Defines.hpp"

namespace cct::refl
{
	class Object;
	class Namespace;
	class MemberVariable;
	class Method;

	class CONCERTO_REFLECTION_API Class
	{
	public:
		virtual ~Class() = default;

		[[nodiscard]] virtual std::string_view GetName() const = 0;
		[[nodiscard]] virtual std::string_view GetNamespaceName() const = 0;
		[[nodiscard]] virtual const Namespace& GetNamespace() const = 0;

		[[nodiscard]] virtual std::size_t GetMemberVariableCount() const = 0;
		[[nodiscard]] virtual std::size_t GetMethodCount() const = 0;

		[[nodiscard]] virtual const MemberVariable& GetMemberVariable(std::size_t index) const = 0;
		[[nodiscard]] virtual const MemberVariable& GetMemberVariable(std::string_view name) const = 0;

		[[nodiscard]] virtual const Method& GetMethod(std::size_t index) const = 0;
		[[nodiscard]] virtual const Method& GetMethod(std::string_view name) const = 0;

		[[nodiscard]] virtual bool HasMemberVariable(std::string_view name) const = 0;
		[[nodiscard]] virtual bool HasMethod(std::string_view name) const = 0;

		[[nodiscard]] virtual std::unique_ptr<Object> CreateInstance() const = 0;

		template<typename T>
		std::unique_ptr<T> CreateInstance() const;

		[[nodiscard]] bool InheritsFrom(const Class& other) const;
		[[nodiscard]] bool InheritsFrom(std::string_view name) const;

		template<typename T>
		[[nodiscard]] bool InheritsFrom() const;
	private:
		std::string _name;
		Namespace* _namespace;
		std::vector<MemberVariable> _memberVariables;
		std::vector<Method> _methods;
		Class* _baseClass;
	};
}

#include "Concerto/Reflection/Class.inl"

#endif //CONCERTO_REFLECTION_CLASS_HPP