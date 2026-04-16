
function add_files_to_target(p)
    for _, dir in ipairs(os.filedirs(p)) do
        relative_dir = path.relative(dir, "Src/")
        if os.isdir(dir) then
            add_files(path.join("Src", relative_dir, "*.cpp"))
            add_files(path.join("Src", relative_dir, "*.refl.hpp"))
            add_headerfiles(path.join("Src", "(" .. relative_dir .. "/*.hpp)"))
            add_headerfiles(path.join("Src", "(" .. relative_dir .. "/*.inl)"))
        else
            local ext = path.extension(relative_dir)
            if ext == ".hpp" or ext == ".inl" then
                add_headerfiles(path.join("Src", "(" .. relative_dir .. ")"))
            elseif ext == ".cpp" then
                add_files(path.join("Src", relative_dir))
            end
        end
    end
end

includes("Xmake/rules/**.lua")

includes("Src/Concerto/Profiler/xmake.lua")
includes("Src/Concerto/CppPlugin/xmake.lua")
includes("Src/Concerto/HeaderPlugin/xmake.lua")
includes("Src/Concerto/PackageGenerator/xmake.lua")
includes("Src/Concerto/Reflection/xmake.lua")
includes("Tests/Reflection/xmake.lua")
