rule("find_cct_pkg_generator")
    before_build(function (target)
        import("core.project.project")
        import("lib.detect.find_tool")

        local envs
		if is_plat("windows") then
			local msvc = target:toolchain("msvc")
			if msvc and msvc:check() then
				envs = msvc:runenvs()
			end
		elseif is_plat("mingw") then
			local mingw = target:toolchain("mingw")
			if mingw and mingw:check() then
				envs = mingw:runenvs()
			end
		end

        local cctPkgGen = project.required_package("concerto-reflection")
        local dir
        if cctPkgGen then
            dir = path.join(cctPkgGen:installdir(), "bin")
        else
            cctPkgGen = project.target("concerto-pkg-generator")
            if cctPkgGen then
                dir = cctPkgGen:targetdir()
            end
        end

        local osenvs = os.getenvs()
        envs = envs or {}
        if cctPkgGen then
			for env, values in pairs(cctPkgGen:get("envs")) do
				local flatval = path.joinenv(values)
				local oldenv = envs[env] or osenvs[env]
				if not oldenv or oldenv == "" then
					envs[env] = flatval
				elseif not oldenv:startswith(flatval) then
					envs[env] = flatval .. path.envsep() .. oldenv
				end
			end
        end
        local program = find_tool("concerto-pkg-generator", {version = false, paths = dir, envs = envs})
        target:data_set("concerto-pkg-generator", program)
        target:data_set("concerto-pkg-generator-envs", envs)
    end)
