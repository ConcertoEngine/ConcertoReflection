if has_config("profiling") then
    add_requires("tracy", {configs = {shared = true, cmake = false, debug = true, vs_runtime = is_mode("debug") and "MDd" or "MD"}})
    
    target("concerto-reflection-profiler", function()
        set_kind("shared")
        add_includedirs("../../../Src", { public = true })
        add_headerfiles("*.hpp")
        add_packages("tracy", {public = true})
        add_packages("concerto-core", {public = false})
        add_defines("CCT_REFL_PROFILING", {public = true})
        add_files("./**.cpp")
        add_rpathdirs("$ORIGIN")
        if is_plat("windows") then
            add_packages("concerto-core-mt", { public = true })
            set_runtimes("MT")
        else
            add_packages("concerto-core", { public = true })
        end
    end)
end
