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
generateDefines name = "#ifdef " ++ map toUpper name ++ "PACKAGE_BUILD\n#define " ++ map toUpper name ++ "PACKAGE_API CCT_EXPORT\n#else\n#define " ++ map toUpper name ++ "PACKAGE_API CCT_IMPORT\n#endif\n"
--generateDefines name = "#define " ++ map toUpper name ++ "PACKAGE_API\n"

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
       ++ "\n"

generateCpp :: String -> [Include] -> [Class] -> [Enumeration] -> String
generateCpp namespaceName _ classes enums = do
                concatMap generateEnumFromString enums
                ++ "\n"
                ++ concatMap generateEnumToString enums
                ++ "\n"
                ++ concatMap Generator.ClassGenerator.generateClassCpp classes
                ++ "\n"
                ++ concatMap (\klasses -> Generator.InternalClassGenerator.generateInternalClass namespaceName klasses) classes

generateNamespaceHpp :: String -> Namespace -> String
generateNamespaceHpp api (Namespace name enums classes []) = "namespace " ++ name ++ " {\n"  ++ generateHpp api [] classes enums ++ "\n}\n"
generateNamespaceHpp api (Namespace name enums classes (x:xs)) = "namespace " ++ name ++ " {\n" ++ (generateNamespaceHpp api x  ++ concatMap (generateNamespaceHpp api) xs) ++ generateHpp api [] classes enums ++ "\n}\n"

generateFullNamespace :: String -> String -> String
generateFullNamespace [] name = name
generateFullNamespace namespace name = namespace ++ "::" ++ name

generateNamespaceCpp :: String -> Namespace -> String
generateNamespaceCpp fullNameSpace (Namespace name enums classes []) = "namespace " ++ name ++ " {\n" ++ generateCpp (generateFullNamespace fullNameSpace name) [] classes enums ++ "\n}\n"
generateNamespaceCpp fullNameSpace (Namespace name enums classes (x:xs)) = "namespace " ++ name ++ " {\n" ++ (generateNamespaceCpp (generateFullNamespace fullNameSpace name) x ++ concatMap (generateNamespaceCpp (generateFullNamespace fullNameSpace name)) xs) ++ generateCpp name [] classes enums ++ "\n}\n"

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
                ++ "#include <Concerto/Reflection/Package.hpp>\n"
                ++ generateDefines name
                ++ "\n"
                ++ generateHpp name includes classes enums
                ++ "\n"
                ++ concatMap (generateNamespaceHpp name) namespaces
                ++ (map toUpper name ++ "PACKAGE_API") ++ " std::unique_ptr<cct::refl::Package> Create" ++ name ++ "Package();\n"

        let cpp = "//This file was automatically generated, do not edit\n\n"
                ++ "#include <Concerto/Core/Assert.hpp>\n"
                ++ "#include \"" ++ name ++ "Package.hpp\"\n"
                ++ "using namespace std::string_view_literals;\n"
                ++ "using namespace std::string_literals;\n\n"
                ++ "using namespace cct;\n"
                ++ "using namespace cct::refl;\n\n"
                ++ concatMap generateCppInclude includes
                ++ generateCpp name includes classes enums
                ++ "\n"
                ++ concatMap (generateNamespaceCpp "") namespaces
                ++ concatMap generateInternalNamespaceCreation namespaces
                ++ generateInternalPackage (Package name version description includes namespaces classes enums)
                ++ "\n"
                ++ "std::unique_ptr<cct::refl::Package> Create" ++ name ++ "Package() { return std::make_unique<Internal" ++ name ++ "Package>(); }\n"
        createDirectoryIfMissing True outputDir
        writeFile (outputDir ++ "/" ++ name ++ "Package.hpp") hpp
        writeFile (outputDir ++ "/" ++ name ++ "Package.cpp") cpp
