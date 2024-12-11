//
// Created by arthur on 09/12/2024.
//
#define CATCH_CONFIG_RUNNER
#include <catch2/catch_test_macros.hpp>

#include "CorePackage.hpp"

SCENARIO("Package can be created")
{
	using namespace std::string_view_literals;
	GIVEN("The core package")
	{
		WHEN("The package is initialized")
		{
			auto pkg = CreateCorePackage();
			REQUIRE(pkg);
			CHECK(pkg->GetName() == "Core"sv);

			pkg->LoadNamespaces();
			pkg->InitializeNamespaces();
			pkg->InitializeClasses();
			cct::refl::Namespace* cctNamespace;
			THEN("It must have the namespace cct")
			{
				CHECK(pkg->GetNamespaceCount() == 1);
				cctNamespace = pkg->GetNamespace("cct"sv);
				REQUIRE(cctNamespace);
				CHECK(cctNamespace->GetName() == "cct"sv);
			}
			cct::refl::Namespace* reflNamespace;
			AND_THEN("The namespace refl must be in the namespace cct")
			{
				CHECK(cctNamespace->GetNamespaceCount() == 1);
				reflNamespace = cctNamespace->GetNamespace("refl"sv);
				REQUIRE(reflNamespace);
				CHECK(reflNamespace->GetName() == "refl"sv);
			}
			AND_THEN("It must have Object class")
			{
				CHECK(reflNamespace->GetClassCount() >= 1);
				auto objClass = reflNamespace->GetClass("Object"sv);
				REQUIRE(objClass);
				CHECK(objClass->GetName() == "Object"sv);
				CHECK(objClass->GetNamespaceName() == "refl"sv);
			}
			pkg = nullptr;
		}
	}
}