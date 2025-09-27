#pragma once
#include <Concerto/Reflection/Defines.hpp>
#include <Concerto/Core/Types.hpp>
#include <Concerto/Core/FunctionRef.hpp>

#include "Concerto/Reflection/Object.hpp"


struct CCT_PACKAGE("version = \"1.0\"", "description = \"Sample Package description\"", "serialize = [\"JSON\", \"YML\"]") ConcertoReflectionTests {};

static cct::FunctionRef<cct::refl::Int32(const cct::refl::Int32& bar1, const cct::refl::Int32& bar2, const cct::refl::Int32& bar3)> CustomDelegatePtr = [](const cct::refl::Int32& bar1, const cct::refl::Int32& bar2, const cct::refl::Int32& bar3)
{
	return bar1;
};

namespace cct::sample
{
	enum class CCT_ENUM() SampleEnum : cct::UInt32
	{
		Foo = 0,
		Bar = 1
	};

	class CCT_CLASS("Test=\"test\"") SampleBar : public cct::refl::Object
	{ 
	public:
		CCT_METHOD("Test=\"test\"")
		int* Foo(const cct::refl::Int32& bar)
		{
			_bar = bar;
			return nullptr;
		}

		CCT_METHOD("Test=\"test\"")
		cct::refl::Int32 Bar(const cct::refl::Int32& bar1, const cct::refl::Int32& bar2, const cct::refl::Int32& bar3)
		{
			return 22;
		}

		CCT_METHOD("Delegate = \"CustomDelegatePtr\"")
		cct::refl::Int32 CustomDelegate(const cct::refl::Int32& bar1, const cct::refl::Int32& bar2, const cct::refl::Int32& bar3);

		CCT_OBJECT(SampleBar);
	private:
		CCT_MEMBER("Test=\"test\"")
		refl::Int32 _bar;
		
	};
}
