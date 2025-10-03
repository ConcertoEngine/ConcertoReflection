//
// Created by arthur on 04/07/2025.
//
#define CATCH_CONFIG_RUNNER
#include <catch2/catch_test_macros.hpp>

#include <ConcertoReflectionPackage.gen.hpp>
#include <ConcertoReflectionTestsPackage.gen.hpp>
#include <Concerto/Reflection/GlobalNamespace/GlobalNamespace.hpp>
#include <Concerto/Reflection/PackageLoader/PackageLoader.hpp>

SCENARIO("MemberVariable")
{
	using namespace std::string_view_literals;
	GIVEN("The core package")
	{
		WHEN("The package is initialized")
		{
			cct::refl::PackageLoader packageLoader;
			REQUIRE(packageLoader.AddPackage(CreateConcertoReflectionPackage()));
			REQUIRE(packageLoader.AddPackage(CreateConcertoReflectionTestsPackage()));
			packageLoader.LoadPackages();

			THEN("We are getting the member variable of 'SampleBar'")
			{
				const cct::refl::Class* sampleBarClass = cct::sample::SampleBar::GetClass();
				REQUIRE(sampleBarClass);

				const size_t memberVariableCount = sampleBarClass->GetMemberVariableCount();
				REQUIRE(memberVariableCount > 0);
				CHECK(memberVariableCount == 1);

				auto sampleBarInstance = sampleBarClass->CreateDefaultObject();
				REQUIRE(sampleBarInstance);

				const cct::refl::Object* sampleBar1 = cct::sample::SampleBar::GetClass()->GetMemberVariable("m_bar", *sampleBarInstance);
				REQUIRE(sampleBar1);
				CHECK(sampleBar1->GetDynamicClass() == cct::refl::Int32::GetClass());

				const cct::refl::Object* sampleBar2 = sampleBarInstance->GetDynamicClass()->GetMemberVariable(0, *sampleBarInstance);
				CHECK(sampleBar1 == sampleBar2);

				sampleBar2 = sampleBarInstance->GetMemberVariable("m_bar");
				CHECK(sampleBar1 == sampleBar2);

				sampleBar2 = sampleBarInstance->GetMemberVariable(0);
				CHECK(sampleBar1 == sampleBar2);

				const cct::refl::MemberVariable* sampleBarMemberVariable1 = cct::sample::SampleBar::GetClass()->GetMemberVariable("m_bar");
				REQUIRE(sampleBarMemberVariable1);
				CHECK(sampleBarMemberVariable1->GetName() == "m_bar"sv);
				CHECK(sampleBarMemberVariable1->GetIndex() == 0);
				CHECK(sampleBarMemberVariable1->GetType() == cct::refl::Int32::GetClass());

				const cct::refl::MemberVariable* sampleBarMemberVariable2 = cct::sample::SampleBar::GetClass()->GetMemberVariable(0);
				CHECK(sampleBarMemberVariable1 == sampleBarMemberVariable2);
			}
		}
	}
}