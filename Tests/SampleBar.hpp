#pragma once
#include <Concerto/Reflection/Defines.hpp>
#include <Concerto/Core/Types.hpp>
#include <Concerto/Core/FunctionRef.hpp>

#include "Concerto/Reflection/Object.hpp"


struct [[cct::Package(version = "1.0", description = "Sample Package description", serialize = ["JSON", "YML"])]] ConcertoReflectionTests {};

static cct::FunctionRef<cct::refl::Int32(const cct::refl::Int32& bar1, const cct::refl::Int32& bar2, const cct::refl::Int32& bar3)> CustomDelegatePtr = [](const cct::refl::Int32& bar1, const cct::refl::Int32& bar2, const cct::refl::Int32& bar3)
{
	return bar1;
};

namespace cct::sample
{
	enum class [[cct::Enum]] SampleEnum : cct::UInt32
	{
		Foo = 0,
		Bar = 1
	};

	class [[cct::Class(Attributes={Test="test"})]] SampleBar : public cct::refl::Object
	{ 
	public:
		[[cct::Method(Attributes={Test="test"})]]
		int* Foo(const cct::refl::Int32& bar)
		{
			_bar = bar;
			return nullptr;
		}

		[[cct::Method(Attributes={Test="test"})]]
		cct::refl::Int32 Bar(const cct::refl::Int32& bar1, const cct::refl::Int32& bar2, const cct::refl::Int32& bar3)
		{
		 	return 22;
		}

		[[cct::Method(Attributes = { Delegate = "CustomDelegatePtr" })]]
		cct::refl::Int32 CustomDelegate(const cct::refl::Int32& bar1, const cct::refl::Int32& bar2, const cct::refl::Int32& bar3);

		CCT_OBJECT(SampleBar);
	private:
		[[cct::Member(Attributes={Test="test"})]]
		refl::Int32 _bar;
		
	};
}
