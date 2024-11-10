module Generator.EnumGenerator where
import Types

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
