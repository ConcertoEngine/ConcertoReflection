add_rules("mode.debug", "mode.release", "mode.coverage")
add_rules("plugin.vsxmake.autoupdate")

add_repositories("concerto-xrepo https://github.com/ConcertoEngine/xmake-repo.git main")

package("concerto-core")
    on_fetch(function (package)
        local concerto_core = "C:/Users/Arthur/Documents/Git/ConcertoEngine/ConcertoCore/install"
        return { linkdirs = { path.join(concerto_core, "lib") }, includedirs = { path.join(concerto_core, "include"), links = { "concerto-core" } } }
    end)
package_end()

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

function add_files_to_target(p, hpp_as_files)
    for _, dir in ipairs(os.filedirs(p)) do
        relative_dir = path.relative(dir, "Src/")
        if os.isdir(dir) then
            add_files(path.join("Src", relative_dir, "*.cpp"))
            if hpp_as_files then
                add_files(path.join("Src", relative_dir, "*.hpp"))
            end
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

target("concerto-pkg-generator")
    set_kind("binary")
    set_languages("cxx20")
    
    local files = { ".", "ClangParser", "CppGenerator", "HeaderGenerator", "FileGenerator" }
    for _, dir in ipairs(files) do
        add_files_to_target("Src/Concerto/PackageGenerator/" .. dir, false)
    end

    add_includedirs("Src/", { public = true })
    add_packages("concerto-core", "toml11", "libllvm", "cxxopts")
    set_policy("build.fence", true)
    add_defines("CCT_PKG_GENERATOR_BUILD")
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
        local lib_dir = path.join(llvm:installdir(), "lib")
        package:add("rpathdirs", lib_dir)
    end)


target("concerto-reflection")
    set_kind("shared")
    set_languages("cxx20")
    add_defines("CCT_REFLECTION_BUILD", { public = false })
    add_defines("CCT_ENABLE_ASSERTS")
    add_includedirs("Src/", { public = true })
    local files = {
        ".",
        "Class",
        "GlobalNamespace",
        "MemberVariable",
        "Method",
        "Namespace",
        "Object",
        "Package",
        "PackageLoader",
        "Registry",
    }
    for _, dir in ipairs(files) do
        add_files_to_target("Src/Concerto/Reflection/" .. dir, true)
    end
    add_deps("concerto-pkg-generator")
    add_packages("concerto-core", { public = true })
    add_rules("cpp_reflect")

    if is_mode("debug") then
        set_symbols("debug")
    end
    set_policy("build.across_targets_in_parallel", false)


if has_config("tests") then
    target("concerto-reflection-tests")
        set_kind("binary")
        set_languages("cxx20")
        add_files("Src/Tests/*.cpp", "Src/Tests/*.hpp")
        add_packages("catch2")
        add_deps("concerto-reflection")
        add_rules("cpp_reflect")
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
end
