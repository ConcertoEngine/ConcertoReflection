rule("cpp_reflect")
    add_deps("find_cct_pkg_generator")
    set_extensions(".hpp")

    on_config(function (target)
        local targetName = target:name():gsub("-(%a)", function(c) return c:upper() end):gsub("^%a", string.upper)
        local generatedCpp = path.join(target:autogendir(), targetName .. "Package.gen.cpp")

        target:add("headerfiles", path.join(target:autogendir(), "(" .. targetName .. "Package.gen.hpp)"))
        target:add("files", generatedCpp, {always_added = true})
        target:add("includedirs", target:autogendir(), {public = true})
        target:add("defines", path.basename(targetName):upper() .. "_BUILD", { public = false })
    end)

    before_buildcmd_files(function (target, batchcmds, headers, opt)
        import("core.project.project")
        import("core.language.language")
        import("core.tool.compiler")
        import("core.tool.toolchain")

        local cctPkgGen = target:data("concerto-pkg-generator")
        assert(cctPkgGen, "concerto-pkg-generator not found!")
        local envs = target:data("concerto-pkg-generator-envs")
        local outputCppFile = path.join(target:autogendir(), target:name() .. "gen.cpp")

        batchcmds:show_progress(opt.progress, "${color.build.object}compiling.reflection")
        local targetName = target:name():gsub("-(%a)", function(c) return c:upper() end):gsub("^%a", string.upper)
        local args = { target:autogendir() }

        table.insert(args, "-DCCT_REFLECTION_PKG_GENERATOR_BUILD")
        for _, header in ipairs(headers.sourcefiles) do
            table.insert(args, "-s" .. header)
        end

        function process_target(t, args, is_root)
            for _, define in ipairs(t:get("defines")) do
                local conf = (t:extraconf("defines") or {})[define]
                if not conf or conf.public ~= false or is_root then
                    table.insert(args, "-D" .. define)
                end
            end

            for _, include in ipairs(t:get("includedirs")) do
                table.insert(args, "-I" .. include)
            end
            for _, dep in ipairs(t:get("deps")) do
                local project_target = project.target(dep)
                if project_target then
                    process_target(project_target, args, false)
                end
            end

            for pkgname, pkgconf in pairs(t:extraconf("packages") or {}) do
                local required_package = project.required_package(pkgname)
                if required_package then
                    process_package(required_package, args)
                end
            end
        end

        function process_package(p, args)
            for _, inc in ipairs(p:get("sysincludedirs") or {}) do
                table.insert(args, "-I" .. inc)
            end
            for _, define in ipairs(p:get("defines")) do
                local conf = (p:extraconf("defines") or {})[define]
                if not conf or conf.public ~= false then
                    table.insert(args, "-D" .. define)
                end
            end
        end

        process_target(target, args, true)
        
        for _, header in ipairs(target:get("headerfiles")) do
            for _, file_path in ipairs(os.filedirs(header)) do
                file_path = file_path:gsub("[%(%)]", "")
                table.insert(args, "-H" .. file_path)
            end
        end
        
        local libllvm = project.required_package("libllvm")
        assert(libllvm, "libllvm not found!")
        local llvm_include = path.join(libllvm:installdir(), "lib", "clang", libllvm:version():major(), "include")
        table.insert(args, "-I" .. llvm_include)

        batchcmds:vrunv(cctPkgGen.program, args, {envs = envs})
        batchcmds:add_depfiles(xmlFile)
        --batchcmds:add_depvalues() todo add version from cabal
        batchcmds:set_depmtime(os.mtime(outputCppFile))
        batchcmds:set_depcache(target:dependfile(outputCppFile))
    end)