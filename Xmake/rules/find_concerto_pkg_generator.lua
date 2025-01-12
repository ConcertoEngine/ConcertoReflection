rule("find_cct_pkg_generator")
    before_build(function (target)
        import("core.project.project")
        import("lib.detect.find_tool")

        local cctPkgGen = project.required_package("concerto-reflection")
        local dir
		local envs
        if cctPkgGen then
            dir = path.join(cctPkgGen:installdir(), "bin")
            envs = {}

            for _, lib_file in ipairs(cctPkgGen:get("libfiles")) do
                local installDir = path.absolute(path.new(lib_file):directory():directory())
                if os.host() == "linux" or os.host() == "macosx" then
                    envs.LD_LIBRARY_PATH = installDir .. path.envsep() .. (envs.LD_LIBRARY_PATH or "")
                elseif os.host() == "windows" then
                    local installDir = path.join(cctPkgGen:installdir(), "bin")
                    envs.PATH = installDir .. path.envsep() .. (envs.PATH or "")
                end
            end

        else
            cctPkgGen = project.target("concerto-pkg-generator")

            if cctPkgGen then
                dir = cctPkgGen:installdir("bin")
                envs = {}
                for _, pkg in ipairs(cctPkgGen:orderpkgs()) do
                    if os.host() == "linux" or os.host() == "macosx" then
                        local installDir = path.join(pkg:installdir(), "lib")
                        envs.LD_LIBRARY_PATH = installDir .. path.envsep() .. (envs.LD_LIBRARY_PATH or "")
                    elseif os.host() == "windows" then
                        local installDir = path.join(pkg:installdir(), "bin")
                        envs.PATH = installDir .. path.envsep() .. (envs.PATH or "")
                    end
                end
            else
                raise("Concerto Reflection package not found")
            end

        end

        local program = find_tool("concerto-pkg-generator", {version = false, paths = dir, envs = envs})
        target:data_set("concerto-pkg-generator", program)
        target:data_set("concerto-pkg-generator-envs", envs)
    end)
