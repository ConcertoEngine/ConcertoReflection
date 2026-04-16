target("concerto-cpp-plugin", function()
    set_kind("shared")
    set_languages("c11")
    add_rpathdirs("$ORIGIN")
    add_files("*.c")
    add_includedirs("../../../Src", { public = true })
    add_deps("concerto-plugin-api")

    if is_plat("windows") then
        add_packages("concerto-core-mt", { public = true })
        set_runtimes("MT")
    else
        add_packages("concerto-core", { public = true })
    end

    if is_mode("debug") then
        set_symbols("debug")
    end
    if has_config("profiling") then
        add_deps("concerto-reflection-profiler")
    end
end)
