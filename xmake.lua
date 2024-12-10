add_rules("mode.debug", "mode.release")
add_repositories("Concerto-xrepo https://github.com/ConcertoEngine/xmake-repo.git main")
add_repositories("local-repo build")

add_requires("concerto-core", "pugixml", "eventpp", "concerto-pkg-generator", {configs = {debug = is_mode("debug"), with_symbols = true}})

add_requires("catch2")
add_rules("plugin.vsxmake.autoupdate")

if is_plat("windows") then
    set_runtimes(is_mode("debug") and "MDd" or "MD")
end

target("concerto-pkg-generator")
    set_kind("binary")
    set_languages("cxx20")
    add_files("Src/PackageGenerator/*.cpp")
    add_headerfiles("Include/(Concerto/PackageGenerator/**.hpp)", "Include/(Concerto/PackageGenerator/**.inl)")
    add_includedirs("Include/", { public = true })
    add_packages("concerto-core", "pugixml")

rule("find_cct_pkg_generator")
    on_config(function (target)
        import("core.project.project")
        import("lib.detect.find_tool")

        local cctPkgGen = project.required_package("concerto-pkg-generator")
        local dir
        if cctPkgGen then
            dir = path.join(cctPkgGen:installdir(), "bin")
        end
        local program = find_tool("concerto-pkg-generator", {version = false, paths = dir})
        target:data_set("concerto-pkg-generator", program)
    end)

rule("xml")
    set_extensions(".xml")
    add_deps("find_cct_pkg_generator")
    on_config(function (target)
        for _, filepath in ipairs(target:sourcebatches()["xml"].sourcefiles) do
            local generatedFile = path.join(target:autogendir(), "Reflection", path.basename(filepath))
            target:add("headerfiles", target:autogendir() .. "/(Reflection/" .. path.basename(filepath) ..".hpp)")
            target:add("includedirs", path.join(target:autogendir(), "Reflection"), {public = true})
            target:add("files", generatedFile .. ".cpp", {always_added = true})
            target:add("files", filepath)
            target:add("defines", path.basename(filepath):upper() .. "_BUILD")
        end
    end)

    before_buildcmd_file(function (target, batchcmds, xmlFile, opt)
        local cctPkgGen = target:data("concerto-pkg-generator")
        assert(cctPkgGen, "concerto-pkg-generator not found!")
        local outputFolder = path.join(target:autogendir(), "Reflection")
        local outputCppFile = path.join(outputFolder, path.basename(xmlFile) .. ".cpp")
        local outputHppFile = path.join(outputFolder, path.basename(xmlFile) .. ".hpp")
        local exePrefix = target:is_plat("mingw", "windows") and ".exe" or ""

        batchcmds:show_progress(opt.progress, "${color.build.object}compiling.reflection %s", xmlFile)
        batchcmds:vrunv(cctPkgGen.program, {xmlFile, path.join(target:autogendir(), "Reflection")})

        batchcmds:add_depfiles(xmlFile)
		--batchcmds:add_depvalues() todo add version from cabal
		batchcmds:set_depmtime(os.mtime(outputCppFile))
		batchcmds:set_depcache(target:dependfile(outputCppFile))
    end)

target("concerto-reflection")
    set_kind("shared")
    set_languages("cxx20")
    add_files("Src/Reflection/*.cpp", "Src/Reflection/*.xml")
    add_defines("CCT_REFLECTION_BUILD")
    add_includedirs("Include/", { public = true })
    add_headerfiles("Include/(Concerto/Reflection/**.hpp)", "Include/(Concerto/Reflection/**.inl)")
    add_packages("concerto-core", { public = true })
    add_rules("xml")

target("concerto-reflection-tests")
    set_kind("binary")
    set_languages("cxx20")
    add_files("Tests/*.cpp", "Tests/*.xml")
    add_packages("catch2", "eventpp")
    add_deps("concerto-reflection")
    add_rules("xml")
    add_includedirs("Tests/", { public = true })
    add_headerfiles("Tests/**.hpp")
    if is_plat("windows") then
        add_cxflags("/Zc:preprocessor")
    end