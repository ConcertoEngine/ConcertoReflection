add_rules("mode.debug", "mode.release")
add_repositories("Concerto-xrepo https://github.com/ConcertoEngine/xmake-repo.git main")

add_requires("concerto-core", "pugixml", "eventpp", "catch2", {configs = {debug = is_mode("debug"), with_symbols = true}})

add_rules("plugin.vsxmake.autoupdate")

if is_plat("windows") then
    set_runtimes(is_mode("debug") and "MDd" or "MD")
end

option("std_lib", {showmenu = true,  default = true})

target("concerto-pkg-generator")
    set_kind("binary")
    set_languages("cxx20")
    add_files("Src/PackageGenerator/*.cpp")
    add_headerfiles("Include/(Concerto/PackageGenerator/**.hpp)", "Include/(Concerto/PackageGenerator/**.inl)")
    add_includedirs("Include/", { public = true })
    add_packages("concerto-core", "pugixml")
target_end()

if has_config("std_lib") then
    add_requires("concerto-reflection", {configs = {debug = is_mode("debug"), with_symbols = true, std_lib = true}})

    target("concerto-reflection")
        set_kind("shared")
        set_languages("cxx20")
        add_files("Src/Reflection/*.cpp", "Src/Reflection/*.xml")
        add_defines("CCT_REFLECTION_BUILD")
        add_includedirs("Include/", { public = true })
        add_headerfiles("Include/(Concerto/Reflection/**.hpp)", "Include/(Concerto/Reflection/**.inl)")
        add_packages("concerto-core", { public = true })
        add_rules("xml")
    target_end()

    target("concerto-reflection-tests")
        set_kind("binary")
        set_languages("cxx20")
        add_files("Tests/*.cpp", "Tests/*.xml")
        add_packages("catch2")
        add_deps("concerto-reflection")
        add_rules("xml")
        add_includedirs("Tests/", { public = true })
        add_headerfiles("Tests/**.hpp")
        if is_plat("windows") then
            add_cxflags("/Zc:preprocessor")
        end
    target_end()
end