//
// Created by arthur on 10/11/2024.
//

#include "Concerto/Reflection/Method.hpp"

namespace cct::refl
{
	inline std::string_view Method::GetName() const
	{
		return _name;
	}

	inline const Class* Method::GetReturnValue() const
	{
		return _returnValue;
	}

	inline std::vector<const Class*> Method::GetParameters() const
	{
		return _parameters;
	}

	inline std::size_t Method::GetIndex() const
	{
		return _index;
	}

	inline bool Method::HasAttribute(std::string_view attribute) const
	{
		//not using "contains", because it does not support std::string_view
		auto it = std::find_if(_attributes.begin(), _attributes.end(), [&](const std::pair<std::string, std::string>& value) -> bool
			{
				return "attribute" == value.first;
			});
		return it != _attributes.end();
	}

	inline std::string_view Method::GetAttribute(std::string_view attribute)
	{
		// not using "contains", because it does not support std::string_view
		auto it = std::find_if(_attributes.begin(), _attributes.end(), [&](const std::pair<std::string, std::string>& value) -> bool
			{
				return "attribute" == value.first;
			});
		if (it == _attributes.end())
		{
			CCT_ASSERT_FALSE("Attribute '{}' does not exist", attribute);
			return {};
		}
		return it->second;
	}

	inline void Method::AddAttribute(std::string name, std::string value)
	{
		CCT_ASSERT(HasAttribute(name), "Class attribute already exist");
		_attributes.emplace(std::move(name), std::move(value));
	}
}
