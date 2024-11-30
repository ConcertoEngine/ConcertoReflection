//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_NAMESPACE_HPP
#define CONCERTO_REFLECTION_NAMESPACE_HPP

#include <string>
#include <vector>
#include <memory>

#include "Concerto/Reflection/Defines.hpp"

namespace cct::refl
{
	class Class;
	class CONCERTO_REFLECTION_API Namespace : public std::enable_shared_from_this<Namespace>
	{
	public:
		Namespace(std::string name);
		~Namespace() = default;

		Namespace(const Namespace&) = delete;
		Namespace(Namespace&&) = default;

		Namespace& operator=(const Namespace&) = delete;
		Namespace& operator=(Namespace&&) = default;

		[[nodiscard]] inline std::string_view GetName() const;
		[[nodiscard]] inline std::size_t GetHash() const;

		[[nodiscard]] inline std::size_t GetClassCount() const;

		[[nodiscard]] inline std::shared_ptr<const Class> GetClass(std::size_t index) const;
		[[nodiscard]] inline std::shared_ptr<const Class> GetClass(std::string_view name) const;

		[[nodiscard]] bool HasClass(std::string_view name) const;
		std::shared_ptr<const Class> AddClass(std::string name, std::shared_ptr<const Class> baseClass);
	private:
		std::string _name;
		std::vector<std::shared_ptr<const Class>> _classes;
		std::size_t _hash;
		friend class InternalSamplePackage;
	};
}

#include "Concerto/Reflection/Namespace.inl"

#endif //CONCERTO_REFLECTION_NAMESPACE_HPP