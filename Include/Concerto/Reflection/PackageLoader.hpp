//
// Created by arthur on 20/12/2024.
//

#ifndef CONCERTO_REFLECTION_PACKAGELOADER_HPP
#define CONCERTO_REFLECTION_PACKAGELOADER_HPP

#include <memory>
#include <vector>

#include <Concerto/Core/DynLib.hpp>

#include "Concerto/Reflection/Package.hpp"
#include "Concerto/Reflection/Defines.hpp"

namespace cct::refl
{
	class CCT_REFLECTION_API PackageLoader
	{
	public:
		PackageLoader() = default;

		PackageLoader(PackageLoader&&) noexcept = default;
		PackageLoader(const PackageLoader&) = delete;

		PackageLoader& operator=(PackageLoader&&) noexcept = default;
		PackageLoader& operator=(const PackageLoader&) = delete;

		void AddPackage(std::unique_ptr<Package> package);
		bool AddDynamicPackage(std::string_view path);
		bool AddDynamicFolder(std::string_view path);

		void AddPackageAndLoad(std::unique_ptr<Package> package);
		bool AddDynamicPackageAndLoad(std::string_view path);
		bool AddDynamicFolderAndLoad(std::string_view path);

		void LoadPackages() const;
	private:
		std::vector<cct::DynLib> _dynamicPackages;
		std::vector<std::unique_ptr<Package>> _packages;
	};
}

#endif //CONCERTO_REFLECTION_PACKAGELOADER_HPP