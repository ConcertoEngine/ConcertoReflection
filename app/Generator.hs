module Generator where
import Parser
import Types

generateHppInclude :: Include -> String
generateHppInclude (Include file True) = "#include \"" ++ file ++ "\"\n"
generateHppInclude _ = ""

generateCppInclude :: Include -> String
generateCppInclude (Include file True) = "#include \"" ++ file ++ "\"\n"
generateCppInclude _ = ""

generateEnumMembers :: [EnumMember] -> String -> String
generateEnumMembers [] _ = ""
generateEnumMembers [EnumMember name value] members = members ++ "\t" ++ name ++ " = " ++ show value ++ "\n"
generateEnumMembers ((EnumMember name value):xs) members = members ++ "\t" ++ name ++ " = " ++ show value ++ ",\n" ++ generateEnumMembers xs members

generateEnum :: Enumeration -> String
generateEnum (Enumeration name base members) = "enum class " ++ name ++ " : " ++ base ++ "\n{\n" ++ generateEnumMembers members  "" ++ "\n};\n"

generateEnumMembersFromString :: String -> EnumMember -> String
generateEnumMembersFromString enumerationName (EnumMember name _) = "\tif(str == \"" ++ name ++ "sv\")\n\t\treturn " ++ enumerationName ++ "::" ++ name ++ ";\n"

generateEnumFromString :: Enumeration -> String
generateEnumFromString (Enumeration name _ members) = name ++ " " ++ name ++ "FromString(std::string_view str)\n{\n\tusing namespace std::string_view_literals;\n" ++ concatMap (generateEnumMembersFromString name) members ++ "\tCONCERTO_ASSERT_FALSE(\"Invalid enum value\");\n\treturn " ++ name ++ "::" ++ enumMemberName (head members) ++ ";\n}\n"

generateEnumMembersToString :: String -> EnumMember -> String
generateEnumMembersToString enumerationName (EnumMember name _) = "\tcase " ++ enumerationName ++ "::" ++ name ++ ":\n\t\treturn \"" ++ name ++ "\"sv;\n"

generateEnumToString :: Enumeration -> String
generateEnumToString (Enumeration name _ members) = "std::string_view " ++ name ++ "ToString(" ++ name ++ " value)\n{\n\tusing namespace std::string_view_literals;\n\tswitch(value)\n\t{\n" ++ concatMap (generateEnumMembersToString  name) members ++ "\t}\n}\n"

-- generateHpp :: [Include] -> [Class] -> [Enumeration] -> String
-- generateHpp includes classes enums = do
--         let hpp = "//This file was automaticly generated, do not edit\n\n"


-- generate :: Package -> (String, String)
-- generate (Package name version description includes namespaces classes enums) = do
--         let hpp = "//This file was automaticly generated, do not edit\n\n"
--         let cpp = "//This file was automaticly generated, do not edit\n\n"
