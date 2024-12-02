//
// Created by arthur on 10/11/2024.
//


#include <Concerto/Core/Logger.hpp>

#include "SamplePackage.hpp"

int main()
{
	auto package = CreatePackage();
	package->LoadNamespaces();
	package->InitializeNamespaces();
	package->InitializeClasses();
	cct::Logger::Info("class count: {}", package->GetClassCount());

	for (std::size_t i = 0; i < package->GetNamespaceCount(); ++i)
	{
		auto nameSpace = package->GetNamespace(i);

		for (std::size_t j = 0; j < nameSpace->GetClassCount(); ++j)
		{
			auto klass = nameSpace->GetClass(j);
			std::unique_ptr<SampleFoo> object = klass->CreateDefaultObject<SampleFoo>();
		}
	}

	return 0;
}
