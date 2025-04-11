#pragma once
#include <Concerto/Reflection/Defines.hpp>
#include <Concerto/Core/Types.hpp>

#include "Concerto/Reflection/Object.hpp"


namespace cct::sample
{
	struct [[cct::Package(version="1.0", description="Sample Package description", serialize=["JSON", "YML"])]] SamplePackage {};

	
	enum class [[cct::Enum]] SampleEnum : cct::UInt32
	{
		Foo = 0,
		Bar = 1
	};

	class [[cct::Class(attributes={Test="test"})]] SampleBar : public cct::refl::Object
	{
	public:
		[[cct::Method(attributes={Test="test"})]]
		int* Foo(const cct::refl::Int32& bar)
		{
			_bar = bar;
		}

		[[cct::Method(attributes={Test="test"})]]
		cct::refl::Int32 Bar(const cct::refl::Int32& bar1, const cct::refl::Int32& bar2, const cct::refl::Int32& bar3)
		{
		 	return 22;
		}
		//CCT_OBJECT(SampleBar);
	private:
		[[cct::Member(attributes={Test="test"})]]
		refl::Int32 _bar;
	};
}
