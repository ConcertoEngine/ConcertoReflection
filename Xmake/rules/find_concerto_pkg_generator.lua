rule("find_cct_pkg_generator")
    before_build(function (target)
        import("core.project.project")
        import("lib.detect.find_tool")

        local cctPkgGen = project.required_package("concerto-reflection")
        local dir
		local envs
        if cctPkgGen then
            dir = path.join(cctPkgGen:installdir(), "bin")
        else
            cctPkgGen = project.target("concerto-pkg-generator")
            if cctPkgGen then
                dir = cctPkgGen:targetdir()
                envs = cctPkgGen:get("runenvs")
            end
        end
        local program = find_tool("concerto-pkg-generator", {version = false, paths = dir, envs = envs})
        target:data_set("concerto-pkg-generator", program)
        target:data_set("concerto-pkg-generator-envs", envs)
    end)
