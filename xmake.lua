add_rules("mode.debug", "mode.release")
add_repositories("Concerto-xrepo https://github.com/ConcertoEngine/xmake-repo.git main")
add_requires("concerto-core", {configs = {debug = is_mode("debug"), with_symbols = true}})
add_requires("catch2")

if is_plat("windows") then
    set_runtimes(is_mode("debug") and "MDd" or "MD")
end

rule("xml")
    set_extensions(".xml")
    on_build_file(function (target, sourcefile)
        os.execv("cabal", {"run", "Reflection", "--", sourcefile, target:targetdir() .. "/Generated/"})
        target:add("headerfiles", target:targetdir() .. "/(Generated/**.hpp)")
        target:add("includedirs", target:targetdir() .. "/Generated/", {public = true})
        target:add("files", target:targetdir() .. "/Generated/**.cpp")
	end)

target("concerto-reflection")
    set_kind("shared")
    set_languages("cxx20")
    add_files("Src/*.cpp")
    add_defines("CONCERTO_REFLECTION_BUILD")
    add_includedirs("Include/", { public = true })
    add_headerfiles("Include/(Concerto/Reflection/**.hpp)")
    add_packages("concerto-core", { public = true })

target("concerto-reflection-tests")
    set_kind("binary")
    set_languages("cxx20")
    add_files("Tests/*.cpp", "sample.xml")
    add_packages("catch2")
    add_deps("concerto-reflection")