rule("generate_compile_commands_before_reflect")
    set_kind("project")

    on_load(function (target)
        target:set("toolchains", "llvm@llvm")
    end)

    before_build(function (target)
        import("core.base.task")
        task.run("project", {kind = "compile_commands", outputdir = target:autogendir()})
    end)

rule("xml_reflect")
    add_deps("find_cct_pkg_generator", "generate_compile_commands_before_reflect")
    on_config(function (target)
        local targetName = target:name():gsub("-(%a)", function(c) return c:upper() end):gsub("^%a", string.upper)
        local generatedCpp = path.join(target:autogendir(), targetName .. ".cpp")

        target:add("headerfiles", path.join(target:autogendir(), "(" .. targetName .. "/ " .. targetName .. ".gen.hpp)"))
        target:add("includedirs", path.join(target:autogendir(), targetName), {public = true})
        target:add("defines", path.basename(targetName):upper() .. "_BUILD")
    end)

    before_buildcmd_files(function (target, batchcmds, sourcebatch, opt)
        local cctPkgGen = target:data("concerto-pkg-generator")
        assert(cctPkgGen, "concerto-pkg-generator not found!")
        local envs = target:data("concerto-pkg-generator-envs")
        local outputCppFile = path.join(target:autogendir(), target:name() .. ".cpp")

        batchcmds:show_progress(opt.progress, "${color.build.object}compiling.reflection")
        batchcmds:vrunv(cctPkgGen.program, {target:autogendir(), target:autogendir()}, {envs = envs})

        batchcmds:add_depfiles(xmlFile)
        --batchcmds:add_depvalues() todo add version from cabal
        batchcmds:set_depmtime(os.mtime(outputCppFile))
        batchcmds:set_depcache(target:dependfile(outputCppFile))
    end)
