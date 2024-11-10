module Generator where
import Parser
import Types
import Data.Char (toUpper)

generateHppInclude :: Include -> String
generateHppInclude (Include file True) = "#include \"" ++ file ++ "\"\n"
generateHppInclude _ = ""

generateCppInclude :: Include -> String
generateCppInclude (Include file False) = "#include \"" ++ file ++ "\"\n"
generateCppInclude _ = ""

generateEnumMembers :: [EnumMember] -> String -> String
generateEnumMembers [] _ = ""
generateEnumMembers [EnumMember name value] members = members ++ "\t" ++ name ++ " = " ++ show value ++ "\n"
generateEnumMembers ((EnumMember name value):xs) members = members ++ "\t" ++ name ++ " = " ++ show value ++ ",\n" ++ generateEnumMembers xs members

generateEnum :: Enumeration -> String
generateEnum (Enumeration name base members) = "enum class " ++ name ++ " : " ++ base ++ "\n{\n" ++ generateEnumMembers members  "" ++ "\n};\n"

generateEnumMembersFromString :: String -> EnumMember -> String
generateEnumMembersFromString enumerationName (EnumMember name _) = "\tif(str == \"" ++ name ++ "\"sv)\n\t\treturn " ++ enumerationName ++ "::" ++ name ++ ";\n"

generateEnumFromString :: Enumeration -> String
generateEnumFromString (Enumeration name _ members) = name ++ " " ++ name ++ "FromString(std::string_view str)\n{\n\tusing namespace std::string_view_literals;\n" ++ concatMap (generateEnumMembersFromString name) members ++ "\tCONCERTO_ASSERT_FALSE(\"Invalid enum value\");\n\treturn " ++ name ++ "::" ++ enumMemberName (head members) ++ ";\n}\n"

generateEnumMembersToString :: String -> EnumMember -> String
generateEnumMembersToString enumerationName (EnumMember name _) = "\tcase " ++ enumerationName ++ "::" ++ name ++ ":\n\t\treturn \"" ++ name ++ "\"sv;\n"

generateEnumToString :: Enumeration -> String
generateEnumToString (Enumeration name _ members) = "std::string_view " ++ name ++ "ToString(" ++ name ++ " value)\n{\n\tusing namespace std::string_view_literals;\n\tswitch(value)\n\t{\n" ++ concatMap (generateEnumMembersToString  name) members ++ "\t}\n}\n"

generatePredeclareEnumToString :: String -> Enumeration -> String
generatePredeclareEnumToString api (Enumeration name _ _) = api ++ " std::string_view " ++ name ++ "ToString(" ++ name ++ " value);\n"

generatePredeclareEnumFromString :: String -> Enumeration -> String
generatePredeclareEnumFromString api (Enumeration name _ _) = api ++ " " ++ name ++ " " ++ name ++ "FromString(std::string_view str);\n"

generateDefines :: String -> String
generateDefines name = "#ifdef " ++ map toUpper name ++ "_BUILD\n#define " ++ map toUpper name ++ "_API CONCERTO_EXPORT\n#else\n#define " ++ map toUpper name ++ "_API CONCERTO_IMPORT\n#endif\n"

generateHpp :: String -> [Include] -> [Class] -> [Enumeration] -> String
generateHpp packageName includes classes enums =
    let api = map toUpper packageName ++ "_API"
    in concatMap generateHppInclude includes
       ++ concatMap generateEnum enums
       ++ concatMap (generatePredeclareEnumToString api) enums
       ++ concatMap (generatePredeclareEnumFromString api) enums

generateCpp :: String -> [Include] -> [Class] -> [Enumeration] -> String
generateCpp packageName includes classes enums = do
                concatMap generateCppInclude includes
                ++ concatMap generateEnumFromString enums
                ++ concatMap generateEnumToString enums

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
