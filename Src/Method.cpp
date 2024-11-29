//
// Created by arthur on 10/11/2024.
//

#include "Concerto/Reflection/Method.hpp"
#include "Concerto/Reflection/Class.hpp"
#include "Concerto/Reflection/MemberVariable.hpp"
#include "Concerto/Reflection/Method.hpp"

namespace cct::refl
{
	Method::Method(std::string_view name, std::shared_ptr<const Class> returnValue, std::vector<std::shared_ptr<const Class>> parameters) :
		_name(name),
		_returnValue(std::move(returnValue)),
		_parameters(std::move(parameters))
	{
	}
}
