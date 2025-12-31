//
// Created by arthur on 31/12/2025
//

#ifndef CONCERTO_REFLECTION_ENUMERATION_CLASS_HPP
#define CONCERTO_REFLECTION_ENUMERATION_CLASS_HPP

#include <string>
#include <string_view>
#include <memory>
#include <vector>
#include <span>

#include "Concerto/Reflection/Class/Class.hpp"
#include "Concerto/Reflection/EnumValue/EnumValue.hpp"
#include "Concerto/Reflection/Enumeration/EnumIterator.hpp"

namespace cct::refl
{
	class CCT_REFLECTION_API EnumerationClass : public Class
	{
	public:
		EnumerationClass(Namespace* nameSpace, std::string name);
		~EnumerationClass() override = default;

		EnumerationClass(const EnumerationClass&) = delete;
		EnumerationClass(EnumerationClass&&) = default;

		EnumerationClass& operator=(const EnumerationClass&) = delete;
		EnumerationClass& operator=(EnumerationClass&&) = default;

		[[nodiscard]] std::span<const std::unique_ptr<EnumValue>> GetEnumValues() const;
		[[nodiscard]] std::size_t GetEnumValueCount() const;

		[[nodiscard]] const EnumValue* GetEnumValue(std::size_t index) const;
		[[nodiscard]] const EnumValue* GetEnumValue(std::string_view name) const;
		[[nodiscard]] const EnumValue* GetEnumValue(cct::Int64 value) const;

		[[nodiscard]] EnumIterable Enumerate() const;

		[[nodiscard]] std::unique_ptr<Object> CreateDefaultObject() const override;

	protected:
		void AddEnumValue(std::string name, cct::Int64 value);

	private:
		virtual void Initialize() override;

		std::vector<std::unique_ptr<EnumValue>> m_enumValues;
	};
}

#endif // CONCERTO_REFLECTION_ENUMERATION_CLASS_HPP
