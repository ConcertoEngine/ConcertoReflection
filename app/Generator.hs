module Generator where
import Data.Char (toUpper)
import Types
import Generator.EnumGenerator
import Generator.ClassGenerator

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
       ++ concatMap (generatePredeclareEnumToString api) enums
       ++ concatMap (generatePredeclareEnumFromString api) enums
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

generate :: Package -> IO ()
generate (Package name version description includes namespaces classes enums) = do
        let hpp = "//This file was automaticly generated, do not edit\n\n"
                ++ "#pragma once\n"
                ++ "#include <Concerto/Core/Types.hpp>\n"
                ++ generateDefines name
                ++ generateHpp name includes classes enums
                ++ "\n"
                ++ concatMap (generateNamespaceHpp name) namespaces

        let cpp = "//This file was automaticly generated, do not edit\n\n"
                ++ "#include <Concerto/Core/Assert.hpp>\n"
                ++ "#include \"" ++ name ++ ".hpp\"\n"
                ++ generateCpp name includes classes enums
                ++ "\n"
                ++ concatMap generateNamespaceCpp namespaces
        writeFile (name ++ ".hpp") hpp
        writeFile (name ++ ".cpp") cpp
