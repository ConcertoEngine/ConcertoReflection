
if has_config("tests") then
    target("concerto-reflection-tests", function () 
        set_kind("binary")
        set_languages("cxx20")
        add_rpathdirs("$ORIGIN")
        add_files("*.cpp", "*.refl.hpp")
        add_packages("catch2", "toml11")
        add_deps("concerto-reflection")
        add_rules("cct_cpp_reflect")
        add_includedirs(".", { public = true }) -- temporary
        add_includedirs("../", { public = true })
        add_headerfiles("**.hpp")
        add_defines("CCT_REFLECTION_TESTS_BUILD", { public = false })
        add_defines("CCT_ENABLE_ASSERTS")
        if is_plat("windows") then
            add_cxflags("/Zc:preprocessor")
        end

        if is_mode("debug") then
            set_symbols("debug")
        end
        add_cxxflags("cl::/Zc:preprocessor")

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
end
    
