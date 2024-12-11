//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_NAMESPACE_HPP
#define CONCERTO_REFLECTION_NAMESPACE_HPP

#include <string>
#include <vector>
#include <memory>
#include <span>

#include "Concerto/Reflection/Defines.hpp"

namespace cct::refl
{
	class Class;
	class CCT_REFLECTION_API Namespace
	{
	public:
		Namespace(std::string name);
		virtual ~Namespace() = default;

		Namespace(const Namespace&) = delete;
		Namespace(Namespace&&) = default;

		Namespace& operator=(const Namespace&) = delete;
		Namespace& operator=(Namespace&&) = default;

		[[nodiscard]] inline std::string_view GetName() const;
		[[nodiscard]] inline std::size_t GetHash() const;

		[[nodiscard]] inline std::size_t GetClassCount() const;
		[[nodiscard]] inline std::size_t GetNamespaceCount() const;

		[[nodiscard]] inline const Class* GetClass(std::size_t index) const;
		[[nodiscard]] inline const Class* GetClass(std::string_view name) const;

		[[nodiscard]] inline Namespace* GetNamespace(std::size_t index) const;
		[[nodiscard]] Namespace* GetNamespace(std::string_view name) const;
		[[nodiscard]] Namespace* GetNamespace(std::span<std::string_view> namespaces) const;

		[[nodiscard]] inline bool HasClass(std::string_view name) const;

		//should be private
		void AddClass(std::unique_ptr<Class> klass);
		void AddNamespace(std::unique_ptr<Namespace> nameSpace);
		virtual void LoadNamespaces() = 0;
		virtual void LoadClasses() = 0;
		virtual void InitializeClasses() = 0;

	protected:
		std::string _name;
		std::vector<std::unique_ptr<Class>> _classes;
		std::vector<std::unique_ptr<Namespace>> _namespaces;
		std::size_t _hash;
	};
}

#include "Concerto/Reflection/Namespace.inl"

#endif //CONCERTO_REFLECTION_NAMESPACE_HPP