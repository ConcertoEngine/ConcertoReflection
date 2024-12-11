//
// Created by arthur on 09/12/2024.
//
#define CATCH_CONFIG_RUNNER
#include <catch2/catch_test_macros.hpp>

#include "CorePackage.hpp"
#include "Concerto/Reflection/GlobalNamespace.hpp"

SCENARIO("Class metadata verification")
{
	using namespace std::string_view_literals;
	GIVEN("The core package")
	{
		WHEN("The package is initialized")
		{
			auto pkg = CreateCorePackage();
			REQUIRE(pkg);

			pkg->LoadNamespaces();
			pkg->InitializeNamespaces();
			pkg->InitializeClasses();

			CHECK(pkg->GetClassCount() == 5);
			CHECK(pkg->GetNamespaceCount() == 1);

			THEN("We are getting the class Object")
			{
				const auto* klass = cct::refl::GetClassByName("cct::refl::Object");
				REQUIRE(klass);
				CHECK(klass == cct::refl::Object::GetClass());
				CHECK(klass->GetName() == "Object"sv);
				CHECK(klass->GetBaseClass() == nullptr);
				CHECK(klass->GetMemberVariableCount() == 0);
				CHECK(klass->GetMethodCount() == 0);
			}

			THEN("We are getting the class Int32")
			{
				const auto* klass = cct::refl::GetClassByName("cct::refl::Int32");
				REQUIRE(klass);
				CHECK(klass == cct::refl::Int32::GetClass());
				CHECK(klass->GetName() == "Int32"sv);
				CHECK(klass->GetBaseClass() == cct::refl::Object::GetClass());
				CHECK(klass->GetMemberVariableCount() == 0);
				CHECK(klass->GetMethodCount() == 0);
			}
		}
	}
}