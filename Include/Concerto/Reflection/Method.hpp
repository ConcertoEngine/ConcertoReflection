//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_METHOD_HPP
#define CONCERTO_REFLECTION_METHOD_HPP

#include <string>
#include <memory>
#include <vector>
#include <span>
#include <array>

#include <Concerto/Core/Any.hpp>

#include "Concerto/Reflection/Defines.hpp"
#include "Concerto/Reflection/Object.hpp"

namespace cct::refl
{
	class Class;
	class CCT_REFLECTION_API Method
	{
	public:
		Method(std::string_view name, std::shared_ptr<const Class> returnValue, std::vector<std::shared_ptr<const Class>> parameters, std::size_t index);
		~Method() = default;

		Method(const Method&) = delete;
		Method(Method&&) = default;

		Method& operator=(const Method&) = delete;
		Method& operator=(Method&&) = default;

		[[nodiscard]] std::string_view GetName() const;
		[[nodiscard]] std::shared_ptr<const Class> GetReturnValue() const;
		[[nodiscard]] std::vector<std::shared_ptr<const Class>> GetParameters() const;
		[[nodiscard]] inline std::size_t GetIndex() const;

		template<typename T, typename ...Args>
		T Invoke(Object& self, Args... args) const;

		inline bool HasAttribute(std::string_view attribute) const;
		inline std::string_view GetAttribute(std::string_view attribute);

		//Should be private
		virtual void Initialize() = 0;
	protected:
		void AddAttribute(std::string name, std::string value);
	private:
		virtual cct::Any Invoke(cct::refl::Object& self, std::span<cct::Any> parameters) const = 0;
		std::string _name;
		std::shared_ptr<const Class> _returnValue;
		std::vector<std::shared_ptr<const Class>> _parameters;
		std::size_t _index;
		std::unordered_map<std::string /*name*/, std::string /*value*/> _attributes;
	};

	template <typename T, typename... Args>
	T Method::Invoke(Object& self, Args... args) const
	{
		constexpr auto size = sizeof...(Args);
		std::array<cct::Any, size> erasedArgs = {(cct::Any::Make<Args>(args) + ...)};

		if constexpr (std::is_void_v<T>)
		{
			Invoke(self, erasedArgs);
			return;
		}
		else
		{
			auto result = Invoke(self, erasedArgs);
			return result.template As<T>();
		}
	}
}

#include "Concerto/Reflection/Method.inl"

#endif //CONCERTO_REFLECTION_METHOD_HPP