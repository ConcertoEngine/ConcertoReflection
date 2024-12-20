//
// Created by arthur on 20/12/2024.
//
#define CATCH_CONFIG_RUNNER
#include <catch2/catch_test_macros.hpp>

#include "CorePackage.hpp"
#include "Concerto/Reflection/GlobalNamespace.hpp"
#include "Concerto/Reflection/PackageLoader.hpp"

SCENARIO("Package loader")
{
	using namespace std::string_view_literals;

	GIVEN("A package")
	{
		auto pkg = CreateCorePackage();
		REQUIRE(pkg);

		cct::refl::PackageLoader packageLoader;
		packageLoader.AddPackage(std::move(pkg));
		packageLoader.LoadPackages();

		THEN("It must have the namespace 'cct'")
		{
			CHECK(pkg->GetNamespaceCount() == 1);
			auto* cctNamespace = pkg->GetNamespace("cct"sv);
			REQUIRE(cctNamespace);
			CHECK(cctNamespace->GetName() == "cct"sv);
		}

		AND_THEN("The GlobalNamespace must also have the namespace 'cct'")
		{
			auto* cctNamespaceG = cct::refl::GlobalNamespace::Get().GetNamespaceByName("cct");
			REQUIRE(cctNamespaceG);
			CHECK(cctNamespaceG->GetName() == "cct"sv);
		}

		WHEN("The package is destroyed")
		{
			pkg = nullptr;

			THEN("GlobalNamespace should be empty")
			{
				CHECK(cct::refl::GlobalNamespace::Get().GetNamespaceByName("cct") == nullptr);
				CHECK(cct::refl::GlobalNamespace::Get().GetClassCount() == 0);
				CHECK(cct::refl::GlobalNamespace::Get().GetNamespaceCount() == 0);
				CHECK(cct::refl::Object::GetClass() == nullptr);
			}
		}
	}
}
