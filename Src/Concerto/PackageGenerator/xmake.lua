
target("concerto-plugin-api", function()
    set_kind("shared")
    set_languages("cxx20")
    add_rpathdirs("$ORIGIN")
    add_files("./Plugin/PluginApi.cpp")
    add_headerfiles("./Plugin/PluginApi.h")
    add_headerfiles("./Defines.hpp")
    add_includedirs("../../../Src", { public = true })
    if is_plat("windows") then
        add_packages("concerto-core-mt", { public = true })
    else
        add_packages("concerto-core", { public = true })
    end
    add_packages("toml11", { public = true })
    add_defines("CRP_PLUGIN_API_BUILD")

    if is_mode("debug") then
        set_symbols("debug")
    end
    if is_plat("windows") then
        set_runtimes("MT")
    end
    if has_config("profiling") then
        add_deps("concerto-reflection-profiler", { public = true })
    end
end)

target("concerto-pkg-generator", function()
    set_kind("binary")
    set_languages("cxx20")
    add_rpathdirs("$ORIGIN")
    local files = { ".", "ClangParser", "Plugin" }
    for _, dir in ipairs(files) do
        add_files_to_target("./" .. dir, false)
    end
    remove_files("./Plugin/PluginApi.cpp")
    add_includedirs("../../../Src", { public = true })
    if is_plat("windows") then
        add_packages("concerto-core-mt", { public = true })
    else
        add_packages("concerto-core", { public = true })
    end
    add_packages("toml11", { public = true })
    add_packages("libllvm", "cxxopts")
    add_deps("concerto-plugin-api")
    set_policy("build.fence", true)
    add_defines("CCT_PKGGENERATOR_BUILD")

    if is_mode("debug") then
        set_symbols("debug")
    end
    if is_plat("windows") then
        set_runtimes("MT")
    end
    -- if has_config("profiling") then
    --     add_deps("concerto-reflection-profiler")
    -- end

    on_config(function(package)
        import("core.project.project")
        local llvm = project.required_package("libllvm")
        assert(llvm, "libllvm not found!")
        local llvm_lib = path.join(llvm:installdir(), "lib")

        local concerto_core = project.required_package("concerto-core" .. (is_plat("windows") and "-mt" or ""))
        assert(concerto_core, "concerto-core not found!")
        local concerto_core_lib = path.join(concerto_core:installdir(), "lib")
        package:add("rpathdirs", llvm_lib, concerto_core_lib)
    end)
end)