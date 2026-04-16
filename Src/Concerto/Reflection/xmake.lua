add_rules("mode.debug", "mode.release", "mode.coverage")
add_rules("plugin.vsxmake.autoupdate")

add_repositories("concerto-xrepo https://github.com/ConcertoEngine/xmake-repo.git main")

add_requires("toml11")
add_requires("libllvm", {configs = {clang = true} })
add_requires("cxxopts")

option("tests", { default = false, description = "Enable unit tests"})
option("profiling", { description = "Build with tracy profiler", default = false })

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

target("concerto-reflection", function()
    set_kind("$(kind)")
    set_languages("cxx20")
    add_rpathdirs("$ORIGIN")
    add_defines("CCT_REFLECTION_BUILD", { public = false })
    add_defines("CCT_ENABLE_ASSERTS")
    add_includedirs("../../../Src/", { public = true })
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
        "String",
        "Vector",
    }
    for _, dir in ipairs(files) do
        add_files_to_target("./" .. dir, true)
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
end)
