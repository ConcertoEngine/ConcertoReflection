add_rules("mode.debug", "mode.release", "mode.coverage")
add_rules("plugin.vsxmake.autoupdate")

add_repositories("concerto-xrepo https://github.com/ConcertoEngine/xmake-repo.git main")

add_requires("toml11")
add_requires("libllvm", {configs = {clang = true} })
add_requires("cxxopts")

option("tests", { default = false, description = "Enable unit tests"})
add_defines("CCT_ENABLE_ASSERTS")

if has_config("tests") then
    add_requires("catch2")
end

if is_plat("windows") then
    add_requires("concerto-core", {debug = true, configs = {asserts = true, shared = true, runtimes = is_mode("debug") and "MDd" or "MD" }})
    set_runtimes(is_mode("debug") and "MDd" or "MD")
    add_requires("concerto-core", {debug = true, configs = {asserts = true, shared = true, runtimes = "MT" }, alias = "concerto-core-mt"})
else 
    add_requires("concerto-core", {debug = true, configs = {asserts = true, shared = true}})
end

if is_mode("coverage") then
    if not is_plat("windows") then
        add_links("gcov")
    end
end

includes("Xmake/rules/**.lua")

function add_files_to_target(p)
    for _, dir in ipairs(os.filedirs(p)) do
        relative_dir = path.relative(dir, "Src/")
        if os.isdir(dir) then
            add_files(path.join("Src", relative_dir, "*.cpp"))
            add_files(path.join("Src", relative_dir, "*.refl.hpp"))
            add_headerfiles(path.join("Src", "(" .. relative_dir .. "/*.hpp)"))
            add_headerfiles(path.join("Src", "(" .. relative_dir .. "/*.inl)"))
        else
            local ext = path.extension(relative_dir)
            if ext == ".hpp" or ext == ".inl" then
                add_headerfiles(path.join("Src", "(" .. relative_dir .. ")"))
            elseif ext == ".cpp" then
                add_files(path.join("Src", relative_dir))
            end
        end
    end
end

target("concerto-plugin-api")
    set_kind("shared")
    set_languages("cxx20")
    add_rpathdirs("$ORIGIN")
    add_files("Src/Concerto/PackageGenerator/Plugin/PluginApi.cpp")
    add_headerfiles("Src/(Concerto/PackageGenerator/Plugin/PluginApi.h)")
    add_headerfiles("Src/(Concerto/PackageGenerator/Defines.hpp)")
    add_includedirs("Src/", { public = true })
    if is_plat("windows") then
        add_packages("concerto-core-mt", { public = true })
    else
        add_packages("concerto-core", { public = true })
    end
    add_packages("toml11", { public = true })
    add_defines("CRP_PLUGIN_API_BUILD")

    if is_mode("debug") then
        set_symbols("debug")
    end
    if is_plat("windows") then
        set_runtimes("MT")
    end

target("concerto-pkg-generator")
    set_kind("binary")
    set_languages("cxx20")
    add_rpathdirs("$ORIGIN")
    local files = { ".", "ClangParser", "Plugin" }
    for _, dir in ipairs(files) do
        add_files_to_target("Src/Concerto/PackageGenerator/" .. dir, false)
    end
    remove_files("Src/Concerto/PackageGenerator/Plugin/PluginApi.cpp")
    add_includedirs("Src/", { public = true })
    if is_plat("windows") then
        add_packages("concerto-core-mt", { public = true })
    else
        add_packages("concerto-core", { public = true })
    end
    add_packages("toml11", { public = true })
    add_packages("libllvm", "cxxopts")
    add_deps("concerto-plugin-api")
    set_policy("build.fence", true)
    add_defines("CCT_PKGGENERATOR_BUILD")

    if is_mode("debug") then
        set_symbols("debug")
    end
    if is_plat("windows") then
        set_runtimes("MT")
    end

    on_config(function(package)
        import("core.project.project")
        local llvm = project.required_package("libllvm")
        assert(llvm, "libllvm not found!")
        local llvm_lib = path.join(llvm:installdir(), "lib")

        local concerto_core = project.required_package("concerto-core" .. (is_plat("windows") and "-mt" or ""))
        assert(concerto_core, "concerto-core not found!")
        local concerto_core_lib = path.join(concerto_core:installdir(), "lib")
        package:add("rpathdirs", llvm_lib, concerto_core_lib)
    end)

-- Header Plugin
target("concerto-header-plugin")
    set_kind("shared")
    set_languages("c11")
    add_rpathdirs("$ORIGIN")
    add_files("Src/Concerto/HeaderPlugin/*.c")
    add_includedirs("Src/", { public = true })
    add_deps("concerto-plugin-api")

    if is_plat("windows") then
        add_packages("concerto-core-mt", { public = true })
        set_runtimes("MT")
    else
        add_packages("concerto-core", { public = true })
    end

    if is_mode("debug") then
        set_symbols("debug")
    end

-- Cpp Plugin
target("concerto-cpp-plugin")
    set_kind("shared")
    set_languages("c11")
    add_rpathdirs("$ORIGIN")
    add_files("Src/Concerto/CppPlugin/*.c")
    add_includedirs("Src/", { public = true })
    add_deps("concerto-plugin-api")

    if is_plat("windows") then
        add_packages("concerto-core-mt", { public = true })
        set_runtimes("MT")
    else
        add_packages("concerto-core", { public = true })
    end

    if is_mode("debug") then
        set_symbols("debug")
    end

target("concerto-reflection")
    set_kind("$(kind)")
    set_languages("cxx20")
    add_rpathdirs("$ORIGIN")
    add_defines("CCT_REFLECTION_BUILD", { public = false })
    add_defines("CCT_ENABLE_ASSERTS")
    add_includedirs("Src/", { public = true })
    if is_kind("static") then
        add_defines("CCT_REFLECTION_STATIC", { public = true })
    end
    local files = {
        ".",
        "Class",
        "Enumeration",
        "EnumIterator",
        "EnumValue",
        "GenericClass",
        "GlobalNamespace",
        "MemberVariable",
        "Method",
        "Namespace",
        "Object",
        "Package",
        "PackageLoader",
        "Registry",
        "TemplateClass",
        "Signal",
    }
    for _, dir in ipairs(files) do
        add_files_to_target("Src/Concerto/Reflection/" .. dir, true)
    end
    add_deps("concerto-pkg-generator")
    add_deps("concerto-header-plugin", {plugin = "pkg-generator"})
    add_deps("concerto-cpp-plugin", {plugin = "pkg-generator"})
    add_packages("concerto-core", { public = true })
    add_rules("cct_cpp_reflect")

    if is_mode("debug") then
        set_symbols("debug")
    end
    set_policy("build.across_targets_in_parallel", false)
    add_cxxflags("cl::/Zc:preprocessor")


if has_config("tests") then
    target("concerto-reflection-tests")
        set_kind("binary")
        set_languages("cxx20")
        add_rpathdirs("$ORIGIN")
        add_files("Src/Tests/*.cpp", "Src/Tests/*.refl.hpp")
        add_packages("catch2", "toml11")
        add_deps("concerto-reflection")
        add_rules("cct_cpp_reflect")
        add_includedirs(".", { public = true }) -- temporary
        add_includedirs("Src/Tests/", { public = true })
        add_headerfiles("Src/Tests/**.hpp")
        add_defines("CCT_REFLECTION_TESTS_BUILD", { public = false })
        add_defines("CCT_ENABLE_ASSERTS")
        if is_plat("windows") then
            add_cxflags("/Zc:preprocessor")
        end

        if is_mode("debug") then
            set_symbols("debug")
        end
        add_cxxflags("cl::/Zc:preprocessor")

    on_config(function(package)
        import("core.project.project")
        local llvm = project.required_package("libllvm")
        assert(llvm, "libllvm not found!")
        local llvm_lib = path.join(llvm:installdir(), "lib")

        local concerto_core = project.required_package("concerto-core" .. (is_plat("windows") and "-mt" or ""))
        assert(concerto_core, "concerto-core not found!")
        local concerto_core_lib = path.join(concerto_core:installdir(), "lib")
        package:add("rpathdirs", llvm_lib, concerto_core_lib)
    end)
end
