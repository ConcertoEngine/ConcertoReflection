//
// Created by arthur on 09/12/2024.
//

#ifndef CONCERTO_CORE_RESULT_HPP
#define CONCERTO_CORE_RESULT_HPP

#include <variant>
#include <type_traits>

namespace cct
{
	template<typename Value, typename Error>
	requires (!std::is_void_v<Error>)
	class Result
	{
	public:
		constexpr Result() requires(std::default_initializable<Value>) = default;

		template<typename... Args>
		Result(std::in_place_type_t<Value>, Args&&... args);

		template<typename... Args>
		Result(std::in_place_type_t<Error>, Args&&... args);

		constexpr Result(Value&& value);
		constexpr Result(Error&& error);

		constexpr Value& GetValue() &;
		constexpr const Value& GetValue() const &;
		constexpr Value&& GetValue() &&;

		constexpr Error& GetError() &;
		constexpr const Error& GetError() const &;
		constexpr Error&& GetError() &&;

		constexpr bool IsError() const;
		constexpr bool IsOk() const;

		constexpr operator bool() const;

	private:
		std::variant<Value, Error> _value;
		static constexpr decltype(_value.index()) _valueIndex = 0;
		static constexpr decltype(_value.index()) _errorIndex = 1;
	};
}

#include "Result.inl"
#endif //CONCERTO_CORE_RESULT_HPP