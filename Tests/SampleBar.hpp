#pragma once
#include <Concerto/Reflection/Defines.hpp>s
#include <Concerto/Core/Types.hpp>

#include "Concerto/Reflection/Object.hpp"

namespace cct
{
	
class SampleBar : cct::refl::Object
{
public:

	 void Foo(const Int32& bar);


	CCT_OBJECT(SampleBar);
private:
	Int32 _bar;
};

}
