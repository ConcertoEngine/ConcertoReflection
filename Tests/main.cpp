//
// Created by arthur on 10/11/2024.
//


#include <Concerto/Core/Logger.hpp>

#include "SamplePackage.hpp"
#include "Concerto/Reflection/Class.hpp"
#include "Concerto/Reflection/MemberVariable.hpp"
#include "Concerto/Reflection/Method.hpp"

int main()
{
	auto package = CreatePackage();
	package->LoadNamespaces();
	package->InitializeNamespaces();
	package->InitializeClasses();
	cct::Logger::Info("class count: {}", package->GetClassCount());
	return 0;
}
