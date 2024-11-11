add_rules("mode.debug", "mode.release")
add_repositories("Concerto-xrepo https://github.com/ConcertoEngine/xmake-repo.git main")
add_requires("concerto-core", {configs = {debug = is_mode("debug"), with_symbols = true}})
add_requires("catch2")
add_rules("plugin.vsxmake.autoupdate")

if is_plat("windows") then
    set_runtimes(is_mode("debug") and "MDd" or "MD")
end

rule("xml")
    set_extensions(".xml")
    on_config(function (target)
        for _, filepath in ipairs(os.files("Tests/**.xml")) do
            local generatedFile = path.join(target:targetdir(), "Generated", path.basename(filepath))
            target:add("headerfiles", target:targetdir() .. "/(Generated/" .. path.basename(filepath) ..".hpp)")
            target:add("includedirs", path.join(target:targetdir(), "Generated"), {public = true})
            target:add("files", generatedFile .. ".cpp", {always_added = true})
            target:add("files", filepath)
        end
    end)
    before_build_file(function (target, sourcefile)
        os.execv("cabal", {"run", "Reflection", "--", sourcefile, path.join(target:targetdir(), "Generated")})
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
    add_files("Tests/*.cpp")
    add_packages("catch2")
    add_deps("concerto-reflection")
    add_rules("xml")
    if is_plat("windows") then
        add_cxflags("/Zc:preprocessor")
    end