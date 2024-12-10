//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_CLASS_HPP
#define CONCERTO_REFLECTION_CLASS_HPP

#include <string>
#include <string_view>
#include <memory>
#include <vector>
#include <span>
#include <unordered_map>

#include "Concerto/Reflection/Defines.hpp"

namespace cct::refl
{
	class Object;
	class Namespace;
	class MemberVariable;
	class Method;

	class CCT_REFLECTION_API Class
	{
	public:
		Class(std::shared_ptr<Namespace> nameSpace, std::string name, std::shared_ptr<const Class> baseClass);
		virtual ~Class() = default;

		Class(const Class&) = delete;
		Class(Class&&) = default;

		Class& operator=(const Class&) = delete;
		Class& operator=(Class&&) = default;

		[[nodiscard]] std::string_view GetName() const;
		[[nodiscard]] std::string_view GetNamespaceName() const;
		[[nodiscard]] const Namespace& GetNamespace() const;
		[[nodiscard]] std::size_t GetHash() const;

		[[nodiscard]] std::size_t GetMemberVariableCount() const;
		[[nodiscard]] std::size_t GetMethodCount() const;

		[[nodiscard]] const MemberVariable* GetMemberVariable(std::size_t index) const;
		[[nodiscard]] const MemberVariable* GetMemberVariable(std::string_view name) const;

		virtual [[nodiscard]] cct::refl::Object* GetMemberVariable(std::size_t index, cct::refl::Object& self) const = 0;
		[[nodiscard]] cct::refl::Object* GetMemberVariable(std::string_view name, cct::refl::Object& self) const;

		[[nodiscard]] const Method* GetMethod(std::size_t index) const;
		[[nodiscard]] const Method* GetMethod(std::string_view name) const;

		[[nodiscard]] bool HasMemberVariable(std::string_view name) const;
		[[nodiscard]] bool HasMethod(std::string_view name) const;

		[[nodiscard]] bool InheritsFrom(const Class& other) const;
		[[nodiscard]] bool InheritsFrom(std::string_view name) const;

		bool HasAttribute(std::string_view attribute) const;
		std::string_view GetAttribute(std::string_view attribute);

		bool operator==(const Class& other) const;
		bool operator!=(const Class& other) const; 

		virtual std::unique_ptr<cct::refl::Object> CreateDefaultObject() const = 0;

		template<typename T>
		requires (std::is_base_of_v<cct::refl::Object, T>&& std::is_polymorphic_v<T>)
		std::unique_ptr<T> CreateDefaultObject() const;

		//should be private
		virtual void Initialize() = 0;
	protected:
		void AddMemberVariable(std::string_view name, std::shared_ptr<const Class> type);
		void AddMemberFunction(std::unique_ptr<Method> method);
		void AddAttribute(std::string name, std::string value);
		void SetNamespace(std::shared_ptr<Namespace> nameSpace);
		void SetBaseClass(std::shared_ptr<const Class> klass);
	private:
		std::string _name;
		std::shared_ptr<Namespace> _namespace;
		std::vector<std::unique_ptr<MemberVariable>> _memberVariables;
		std::vector<std::unique_ptr<Method>> _methods;
		std::shared_ptr<const Class> _baseClass;
		std::unordered_map<std::string /*name*/, std::string /*value*/> _attributes;

		std::size_t _hash;
	};

	std::shared_ptr<Namespace> CCT_REFLECTION_API GetNamespaceByName(std::string_view nameSpaceName);
	std::shared_ptr<const Class> CCT_REFLECTION_API GetClassByName(std::string_view nameSpaceName, std::string_view name);
	std::shared_ptr<const Class> CCT_REFLECTION_API GetClassByName(std::span<std::string_view> nameSpaceNames, std::string_view name);

	/**
	 * 
	 * @param name The name of the class, it can be prefixed with the namespace, GetClassByName("cct::refl::Object")
	 * @return nullptr if the class could not be found
	 */
	std::shared_ptr<const Class> CCT_REFLECTION_API GetClassByName(std::string_view name);

}

#include "Concerto/Reflection/Class.inl"

#endif //CONCERTO_REFLECTION_CLASS_HPP