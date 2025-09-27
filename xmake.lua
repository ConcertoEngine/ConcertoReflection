add_rules("mode.debug", "mode.release", "mode.coverage")
add_rules("plugin.vsxmake.autoupdate")

add_repositories("concerto-xrepo https://github.com/ConcertoEngine/xmake-repo.git main")

add_requires("concerto-core", {configs = {asserts = true, shared = false, debug = is_mode("debug"), with_symbols = true}})
add_requires("toml11", {configs = {debug = is_mode("debug"), with_symbols = is_mode("debug")}})
add_requires("libllvm", {configs = {clang = true} })
add_requires("cxxopts")

option("tests", { default = false, description = "Enable unit tests"})
add_defines("CCT_ENABLE_ASSERTS")
if has_config("tests") then
    add_requires("catch2")
end

if is_plat("windows") then
    set_runtimes(is_mode("debug") and "MDd" or "MD")
end

if is_mode("coverage") then
    if not is_plat("windows") then
        add_links("gcov")
    end
end

includes("Xmake/rules/**.lua")

target("concerto-pkg-generator")
    set_kind("binary")
    set_languages("cxx20")
    add_files("Src/PackageGenerator/*.cpp")
    add_headerfiles("Include/(Concerto/PackageGenerator/**.hpp)")
    add_includedirs("Include/", { public = true })
    add_packages("concerto-core", "toml11", "libllvm", "cxxopts")
    set_policy("build.fence", true)
    add_defines("CCT_PKG_GENERATOR_BUILD", "CCT_WITH_CLANG_TOOLING")
    if is_mode("debug") then
        set_symbols("debug")
    end
    if is_plat("windows") then
        set_runtimes("MT")
    end


target("concerto-reflection")
    set_kind("shared")
    set_languages("cxx20")
    add_files("Src/Reflection/*.cpp", "Include/Concerto/Reflection/**.hpp")
    add_defines("CCT_REFLECTION_BUILD")
    add_includedirs("Include/", { public = true })
    add_headerfiles("Include/(Concerto/Reflection/**.hpp)", "Include/(Concerto/Reflection/**.inl)")
    add_packages("concerto-core", { public = true })
    add_rules("cpp_reflect")
    add_deps("concerto-pkg-generator")

    if is_mode("debug") then
        set_symbols("debug")
    end
    set_policy("build.across_targets_in_parallel", false)


if has_config("tests") then
    target("concerto-reflection-tests")
        set_kind("binary")
        set_languages("cxx20")
        add_files("Tests/*.cpp", "Tests/*.hpp")
        add_packages("catch2")
        add_deps("concerto-reflection")
        add_rules("cpp_reflect")
        add_includedirs("Tests/", { public = true })
        add_headerfiles("Tests/**.hpp")

        if is_plat("windows") then
            add_cxflags("/Zc:preprocessor")
        end

        if is_mode("debug") then
            set_symbols("debug")
        end
end
