rule("xml_reflect")
    add_deps("find_cct_pkg_generator")
    set_extensions(".hpp")

    on_config(function (target)
        local targetName = target:name():gsub("-(%a)", function(c) return c:upper() end):gsub("^%a", string.upper)
        local generatedCpp = path.join(target:autogendir(), targetName .. "Package.gen.cpp")

        target:add("headerfiles", path.join(target:autogendir(), "(" .. targetName .. "Package.gen.hpp)"))
        target:add("files", generatedCpp, {always_added = true})
        target:add("includedirs", target:autogendir(), {public = true})
        target:add("defines", path.basename(targetName):upper() .. "_BUILD")
    end)

    before_buildcmd_files(function (target, batchcmds, headers, opt)
        local cctPkgGen = target:data("concerto-pkg-generator")
        assert(cctPkgGen, "concerto-pkg-generator not found!")
        local envs = target:data("concerto-pkg-generator-envs")
        local outputCppFile = path.join(target:autogendir(), target:name() .. "gen.cpp")

        batchcmds:show_progress(opt.progress, "${color.build.object}compiling.reflection")
        local targetName = target:name():gsub("-(%a)", function(c) return c:upper() end):gsub("^%a", string.upper)
        local args = { path.join(target:autogendir(), targetName) }
        for _, header in ipairs(headers.sourcefiles) do
            table.insert(args, header)
        end
    
        batchcmds:vrunv(cctPkgGen.program, args, {envs = envs})
        batchcmds:add_depfiles(xmlFile)
        --batchcmds:add_depvalues() todo add version from cabal
        batchcmds:set_depmtime(os.mtime(outputCppFile))
        batchcmds:set_depcache(target:dependfile(outputCppFile))
    end)
