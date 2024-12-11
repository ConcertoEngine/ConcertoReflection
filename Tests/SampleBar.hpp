#pragma once
#include <Concerto/Reflection/Defines.hpp>
#include <Concerto/Core/Types.hpp>

#include "Concerto/Reflection/Object.hpp"

namespace cct
{
	class SampleBar : public cct::refl::Object
	{
	public:

		 void Foo(const cct::refl::Int32& bar)
		 {
			_bar = bar;
		 }

		cct::refl::Int32 Bar(const cct::refl::Int32& bar1, const cct::refl::Int32& bar2, const cct::refl::Int32& bar3)
		{
		 	return 22;
		}
		CCT_OBJECT(SampleBar);
	private:
		refl::Int32 _bar;
	};
}
