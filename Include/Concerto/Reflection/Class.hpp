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
		Class(Namespace* nameSpace, std::string name, Class* baseClass);
		virtual ~Class() = default;

		[[nodiscard]] std::string_view GetName() const;
		[[nodiscard]] std::string_view GetNamespaceName() const;
		[[nodiscard]] const Namespace& GetNamespace() const;
		[[nodiscard]] std::size_t GetHash() const;

		[[nodiscard]] std::size_t GetMemberVariableCount() const;
		[[nodiscard]] std::size_t GetMethodCount() const;

		[[nodiscard]] const MemberVariable* GetMemberVariable(std::size_t index) const;
		[[nodiscard]] const MemberVariable* GetMemberVariable(std::string_view name) const;

		[[nodiscard]] const Method* GetMethod(std::size_t index) const;
		[[nodiscard]] const Method* GetMethod(std::string_view name) const;

		[[nodiscard]] bool HasMemberVariable(std::string_view name) const;
		[[nodiscard]] bool HasMethod(std::string_view name) const;

		[[nodiscard]] virtual std::unique_ptr<Object> CreateInstance() const = 0;

		template<typename T>
		std::unique_ptr<T> CreateInstance() const;

		[[nodiscard]] bool InheritsFrom(const Class& other) const;
		[[nodiscard]] bool InheritsFrom(std::string_view name) const;

		template<typename T>
		[[nodiscard]] bool InheritsFrom() const;

		bool operator==(const Class& other) const;
		bool operator!=(const Class& other) const;
	private:
		std::string _name;
		Namespace* _namespace;
		std::vector<std::unique_ptr<MemberVariable>> _memberVariables;
		std::vector<std::unique_ptr<Method>> _methods;
		Class* _baseClass;

		std::size_t _hash;
	};
}

#include "Concerto/Reflection/Class.inl"

#endif //CONCERTO_REFLECTION_CLASS_HPP