#pragma once
#include <Concerto/Reflection/Defines.hpp>
#include <Concerto/Core/Types.hpp>
#include <Concerto/Core/FunctionRef.hpp>
#include <Concerto/Reflection/Object.hpp>
#include "Defines.hpp"


struct CCT_PACKAGE("version = \"1.0\"", "description = \"Sample Package description\"", "serialize = [\"JSON\", \"YML\"]") ConcertoReflectionTests {};

namespace cct::sample
{
	enum class CCT_REFL_TESTS_ENUM() SampleEnum : cct::UInt32
	{
		Foo = 0,
		Bar = 1
	};

	class CCT_REFL_TESTS_CLASS("Test=\"test\"") SampleBar : public cct::refl::Object
	{ 
	public:
		CCT_REFL_TESTS_METHOD("Test=\"test\"")
		int* Foo(const cct::refl::Int32 & bar)
		{
			_bar = bar;
			return nullptr;
		}

		CCT_REFL_TESTS_METHOD("Test=\"test\"")
			cct::refl::Int32 Bar(const cct::refl::Int32 & bar1, const cct::refl::Int32 & bar2, const cct::refl::Int32 & bar3)
		{
			return 42;
		}

		CCT_REFL_TESTS_METHOD("Delegate = \"m_customDelegatePtr\"")
		cct::refl::Int32 CustomDelegate();

		CCT_OBJECT(SampleBar);
	private:
		CCT_REFL_TESTS_MEMBER("Test=\"test\"")
		refl::Int32 _bar;

		cct::FunctionRef<cct::refl::Int32()> m_customDelegatePtr = []()
		{
			return refl::Int32();
		};
		
	};
}
