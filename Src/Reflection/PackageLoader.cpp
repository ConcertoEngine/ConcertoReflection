//
// Created by arthur on 20/12/2024.
//

#include <filesystem>

#include "Concerto/Reflection/Class.hpp"
#include "Concerto/Reflection/GlobalNamespace.hpp"
#include "Concerto/Reflection/Namespace.inl"
#include "Concerto/Reflection/Method.hpp"
#include "Concerto/Reflection/MemberVariable.hpp"
#include "Concerto/Reflection/Package.hpp"
#include "Concerto/Reflection/PackageLoader.hpp"

namespace cct::refl
{
	void PackageLoader::AddPackage(std::unique_ptr<Package> package)
	{
		_packages.emplace_back(std::move(package));
	}

	bool PackageLoader::AddDynamicPackage(std::string_view path)
	{
		cct::DynLib lib;
		if (!lib.Load(path))
			return false;
		auto func = lib.GetFunction<std::unique_ptr<Package>>("CreatePackage");
		if (!func)
			return false;
		AddPackage(func());
		_dynamicPackages.emplace_back(std::move(lib));
		return true;
	}

	bool PackageLoader::AddDynamicFolder(std::string_view path)
	{
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			if (entry.is_directory())
				continue;
			if (entry.path().extension() != CONCERTO_DYNLIB_EXTENSION)
				continue;
			if (!AddDynamicPackage(entry.path().string()))
				return false;
		}
		return true;
	}

	void PackageLoader::AddPackageAndLoad(std::unique_ptr<Package> package)
	{
		package->LoadNamespaces();
		package->InitializeNamespaces();
		package->InitializeClasses();
		AddPackage(std::move(package));
	}

	bool PackageLoader::AddDynamicPackageAndLoad(std::string_view path)
	{
		if (!AddDynamicPackage(path))
			return false;
		auto& package = _packages.back();
		package->LoadNamespaces();
		package->InitializeNamespaces();
		package->InitializeClasses();
		return true;
	}

	bool PackageLoader::AddDynamicFolderAndLoad(std::string_view path)
	{
		const std::size_t packageCount = _packages.size();
		if (!AddDynamicFolder(path))
			return false;
		const std::size_t newPackageCount = _packages.size();

		for (std::size_t i = packageCount; i < newPackageCount; ++i)
			_packages[i]->LoadNamespaces();
		for (std::size_t i = packageCount; i < newPackageCount; ++i)
			_packages[i]->InitializeNamespaces();
		for (std::size_t i = packageCount; i < newPackageCount; ++i)
			_packages[i]->InitializeClasses();
		return true;
	}

	void PackageLoader::LoadPackages() const
	{
		for (const auto& pkg : _packages)
			pkg->LoadNamespaces();
		for (const auto& pkg : _packages)
			pkg->InitializeNamespaces();
		for (const auto& pkg : _packages)
			pkg->InitializeClasses();
	}
}
