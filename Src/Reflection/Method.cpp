//
// Created by arthur on 10/11/2024.
//

#include "Concerto/Reflection/Method.hpp"
#include "Concerto/Reflection/Class.hpp"
#include "Concerto/Reflection/MemberVariable.hpp"
#include "Concerto/Reflection/Method.hpp"

namespace cct::refl
{
	Method::Method(std::string_view name, const Class* returnValue, std::vector<const Class*> parameters, std::size_t index, void* customInvoker) :
		_name(name),
		_returnValue(returnValue),
		_parameters(std::move(parameters)),
		_index(index),
		_customInvoker(customInvoker)
	{
	}
}
