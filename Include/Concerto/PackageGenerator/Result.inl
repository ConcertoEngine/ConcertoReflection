//
// Created by arthur on 09/12/2024.
//

#ifndef CONCERTO_CORE_RESULT_INL
#define CONCERTO_CORE_RESULT_INL

#include "Result.hpp"

namespace cct
{
	template <typename Value, typename Error>
	requires (!std::is_void_v<Error>)
	constexpr bool Result<Value, Error>::IsError() const
	{
		return _value.index() == _errorIndex;
	}

	template <typename Value, typename Error>
	requires (!std::is_void_v<Error>)
	constexpr bool Result<Value, Error>::IsOk() const
	{
		return _value.index() == _valueIndex;
	}

	template <typename Value, typename Error>
	requires (!std::is_void_v<Error>)
	constexpr Result<Value, Error>::operator bool() const
	{
		return IsOk();
	}

	template <typename Value, typename Error> requires (!std::is_void_v<Error>)
	template <typename ... Args>
	Result<Value, Error>::Result(std::in_place_type_t<Value>, Args&&... args)
	{
		_value.template emplace<Value>(std::forward<Args>(args)...);
	}

	template <typename Value, typename Error> requires (!std::is_void_v<Error>)
	template <typename ... Args>
	Result<Value, Error>::Result(std::in_place_type_t<Error>, Args&&... args)
	{
		_value.template emplace<Error>(std::forward<Args>(args)...);
	}

	template <typename Value, typename Error> requires (!std::is_void_v<Error>)
	constexpr Result<Value, Error>::Result(Value&& value)
	{
		_value.template emplace<Value>(std::forward<Value>(value));
	}

	template <typename Value, typename Error> requires (!std::is_void_v<Error>)
	constexpr Result<Value, Error>::Result(Error&& error)
	{
		_value.template emplace<Error>(std::forward<Error>(error));
	}

	template <typename Value, typename Error>
	requires (!std::is_void_v<Error>)
	constexpr Value& Result<Value, Error>::GetValue() &
	{
		return std::get<_valueIndex>(_value);
	}

	template <typename Value, typename Error>
	requires (!std::is_void_v<Error>)
	constexpr const Value& Result<Value, Error>::GetValue() const &
	{
		return std::get<_valueIndex>(_value);
	}

	template <typename Value, typename Error>
	requires (!std::is_void_v<Error>)
	constexpr Value&& Result<Value, Error>::GetValue() &&
	{
		return std::get<_valueIndex>(std::move(_value));
	}

	template <typename Value, typename Error>
	requires (!std::is_void_v<Error>)
	constexpr Error& Result<Value, Error>::GetError() &
	{
		return std::get<_errorIndex>(_value);
	}

	template <typename Value, typename Error>
	requires (!std::is_void_v<Error>)
	constexpr const Error& Result<Value, Error>::GetError() const &
	{
		return std::get<_errorIndex>(_value);
	}

	template <typename Value, typename Error>
	requires (!std::is_void_v<Error>)
	constexpr Error&& Result<Value, Error>::GetError() &&
	{
		return std::get<_errorIndex>(std::move(_value));
	}
}

#endif //CONCERTO_CORE_RESULT_INL
