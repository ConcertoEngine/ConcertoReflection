module Generator where
import Data.Char (toUpper)
import Types
import Generator.EnumGenerator
import Generator.ClassGenerator
import Generator.InternalClassGenerator
import System.Directory (createDirectoryIfMissing)

generateHppInclude :: Include -> String
generateHppInclude (Include file True) = "#include \"" ++ file ++ "\"\n"
generateHppInclude _ = ""

generateCppInclude :: Include -> String
generateCppInclude (Include file False) = "#include \"" ++ file ++ "\"\n"
generateCppInclude _ = ""

generateDefines :: String -> String
generateDefines name = "#ifdef " ++ map toUpper name ++ "PACKAGE_BUILD\n#define " ++ map toUpper name ++ "PACKAGE_API CONCERTO_EXPORT\n#else\n#define " ++ map toUpper name ++ "PACKAGE_API CONCERTO_IMPORT\n#endif\n"

generateHpp :: String -> [Include] -> [Class] -> [Enumeration] -> String
generateHpp packageName includes classes enums =
    let api = map toUpper packageName ++ "PACKAGE_API"
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
                ++ "\n"
                ++ concatMap generateEnumToString enums
                ++ "\n"
                ++ concatMap Generator.ClassGenerator.generateClassCpp classes
                ++ "\n"
                ++ concatMap Generator.InternalClassGenerator.generateInternalClass classes

generateNamespaceHpp :: String -> Namespace -> String
generateNamespaceHpp api (Namespace name enums classes) = "namespace " ++ name ++ "\n{\n" ++ generateHpp api [] classes enums ++ "}\n\n"

generateNamespaceCpp :: Namespace -> String
generateNamespaceCpp (Namespace name enums classes) = "namespace " ++ name ++ "\n{\n" ++ generateCpp name [] classes enums ++ "}\n\n"

generate :: Package -> String -> IO ()
generate (Package name version description includes namespaces classes enums) outputDir = do
        let hpp = "//This file was automatically generated, do not edit\n\n"
                ++ "#pragma once\n"
                ++ "#include <string_view>\n"
                ++ "#include <Concerto/Reflection/Defines.hpp>\n\n"
                ++ "#include <Concerto/Reflection/Class.hpp>\n"
                ++ "#include <Concerto/Reflection/Object.hpp>\n"
                ++ "#include <Concerto/Reflection/MemberVariable.hpp>\n"
                ++ "#include <Concerto/Reflection/Method.hpp>\n"
                ++ generateDefines name
                ++ "\n"
                ++ generateHpp name includes classes enums
                ++ "\n"
                ++ concatMap (generateNamespaceHpp name) namespaces

        let cpp = "//This file was automatically generated, do not edit\n\n"
                ++ "#include <Concerto/Core/Assert.hpp>\n"
                ++ "#include \"" ++ name ++ "Package.hpp\"\n"
                ++ "using namespace std::string_view_literals;\n"
                ++ "using namespace std::string_literals;\n\n"
                ++ generateCpp name includes classes enums
                ++ "\n"
                ++ concatMap generateNamespaceCpp namespaces
        createDirectoryIfMissing True outputDir
        writeFile (outputDir ++ "/" ++ name ++ "Package.hpp") hpp
        writeFile (outputDir ++ "/" ++ name ++ "Package.cpp") cpp
