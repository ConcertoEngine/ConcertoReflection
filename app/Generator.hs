module Generator where
import Data.Char (toUpper)
import Types
import Generator.EnumGenerator
import Generator.ClassGenerator
import System.Directory (createDirectoryIfMissing)

generateHppInclude :: Include -> String
generateHppInclude (Include file True) = "#include \"" ++ file ++ "\"\n"
generateHppInclude _ = ""

generateCppInclude :: Include -> String
generateCppInclude (Include file False) = "#include \"" ++ file ++ "\"\n"
generateCppInclude _ = ""

generateDefines :: String -> String
generateDefines name = "#ifdef " ++ map toUpper name ++ "_BUILD\n#define " ++ map toUpper name ++ "_API CONCERTO_EXPORT\n#else\n#define " ++ map toUpper name ++ "_API CONCERTO_IMPORT\n#endif\n"

generateHpp :: String -> [Include] -> [Class] -> [Enumeration] -> String
generateHpp packageName includes classes enums =
    let api = map toUpper packageName ++ "_API"
    in concatMap generateHppInclude includes
       ++ concatMap generateEnum enums
       ++ "\n"
       ++ concatMap (generatePredeclareEnumToString api) enums
       ++ "\n"
       ++ concatMap (generatePredeclareEnumFromString api) enums
       ++ "\n"
       ++ concatMap (Generator.ClassGenerator.generateClassHpp api) classes

generateCpp :: String -> [Include] -> [Class] -> [Enumeration] -> String
generateCpp _ includes classes enums = do
                concatMap generateCppInclude includes
                ++ concatMap generateEnumFromString enums
                ++ concatMap generateEnumToString enums
                ++ concatMap Generator.ClassGenerator.generateClassCpp classes

generateNamespaceHpp :: String -> Namespace -> String
generateNamespaceHpp api (Namespace name enums classes) = "namespace " ++ name ++ "\n{\n" ++ generateHpp api [] classes enums ++ "}\n"

generateNamespaceCpp :: Namespace -> String
generateNamespaceCpp (Namespace name enums classes) = "namespace " ++ name ++ "\n{\n" ++ generateCpp name [] classes enums ++ "}\n"

generate :: Package -> String -> IO ()
generate (Package name version description includes namespaces classes enums) outputDir = do
        let hpp = "//This file was automatically generated, do not edit\n\n"
                ++ "#pragma once\n"
                ++ "#include <string_view>\n"
                ++ "#include <Concerto/Reflection/Defines.hpp>\n\n"
                ++ "#include <Concerto/Reflection/Class.hpp>\n"
                ++ generateDefines name
                ++ "\n"
                ++ generateHpp name includes classes enums
                ++ "\n"
                ++ concatMap (generateNamespaceHpp name) namespaces

        let cpp = "//This file was automatically generated, do not edit\n\n"
                ++ "#include <Concerto/Core/Assert.hpp>\n"
                ++ "#include \"" ++ name ++ "Package.hpp\"\n"
                ++ generateCpp name includes classes enums
                ++ "\n"
                ++ concatMap generateNamespaceCpp namespaces
        createDirectoryIfMissing True outputDir
        writeFile (outputDir ++ "/" ++ name ++ "Package.hpp") hpp
        writeFile (outputDir ++ "/" ++ name ++ "Package.cpp") cpp
