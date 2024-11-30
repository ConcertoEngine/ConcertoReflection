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
            local generatedFile = path.join(target:autogendir(), "Reflection", path.basename(filepath))
            target:add("headerfiles", target:autogendir() .. "/(Reflection/" .. path.basename(filepath) ..".hpp)")
            target:add("includedirs", path.join(target:autogendir(), "Reflection"), {public = true})
            target:add("files", generatedFile .. ".cpp", {always_added = true})
            target:add("files", filepath)
            target:add("defines", path.basename(filepath):upper() .. "_BUILD")
        end
    end)

    before_buildcmd_file(function (target, batchcmds, xmlFile, opt)
        local outputFolder = path.join(target:autogendir(), "Reflection")
        local outputCppFile = path.join(outputFolder, path.basename(xmlFile) .. ".cpp")
        local outputHppFile = path.join(outputFolder, path.basename(xmlFile) .. ".hpp")

        batchcmds:show_progress(opt.progress, "${color.build.object}compiling.reflection %s", xmlFile)
        batchcmds:vrunv("cabal", {"run", "Reflection", "--", xmlFile, path.join(target:autogendir(), "Reflection")})

        batchcmds:add_depfiles(xmlFile)
		--batchcmds:add_depvalues() todo add version from cabal
		batchcmds:set_depmtime(os.mtime(outputCppFile))
		batchcmds:set_depcache(target:dependfile(outputCppFile))
    end)

target("concerto-reflection")
    set_kind("shared")
    set_languages("cxx20")
    add_files("Src/Reflection/*.cpp")
    add_defines("CONCERTO_REFLECTION_BUILD")
    add_includedirs("Include/", { public = true })
    add_headerfiles("Include/(Concerto/Reflection/**.hpp)", "Include/(Concerto/Reflection/**.inl)")
    add_packages("concerto-core", { public = true })

target("concerto-reflection-tests")
    set_kind("binary")
    set_languages("cxx20")
    add_files("Tests/*.cpp")
    add_packages("catch2")
    add_deps("concerto-reflection")
    add_rules("xml")
    add_includedirs("Tests/", { public = true })
    add_headerfiles("Tests/**.hpp")
    if is_plat("windows") then
        add_cxflags("/Zc:preprocessor")
    end