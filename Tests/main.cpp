//
// Created by arthur on 10/11/2024.
//

#include <array>
#include <span>
#include <Concerto/Core/Logger.hpp>

#include "CorePackage.hpp"
#include "SamplePackage.hpp"



void Init(std::span<std::unique_ptr<cct::refl::Package>> packages)
{
	for (auto& p : packages)
		p->LoadNamespaces();
	for (auto& p : packages)
		p->InitializeNamespaces();
	for (auto& p : packages)
		p->InitializeClasses();
}

int main()
{
	using namespace cct;
	std::array packages = {
		CreateCorePackage(),
		CreateSamplePackage()
	};

	Init(packages);

	//refl::Namespace::GetGlobalNamespace()->LoadClasses();
	//refl::Namespace::GetGlobalNamespace()->LoadNamespaces();

	auto x = refl::Namespace::GetGlobalNamespace();
	auto klass = refl::Namespace::GetGlobalNamespace()->GetClass("cct::refl::Object");
			//std::unique_ptr<SampleFoo> object = klass->CreateDefaultObject<SampleFoo>();
	
	return 0;
}
