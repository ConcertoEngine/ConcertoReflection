module Generator.ClassGenerator where
import Types
import Data.Char (toUpper)

capitalizeFirst :: String -> String
capitalizeFirst ""     = ""
capitalizeFirst (x:xs) = toUpper x : xs

removeMemberPrefix :: String -> String
removeMemberPrefix ('_':xs) = xs
removeMemberPrefix xs       = xs

generateNamespaceString :: String -> String
generateNamespaceString [] = ""
generateNamespaceString namespace = namespace ++ "zz::"

generateClassMembersGetterHpp :: ClassMember -> String
generateClassMembersGetterHpp (ClassMember name typeName _ namespace) = "\tconst " ++ generateNamespaceString namespace ++ typeName ++ "& Get" ++ capitalizeFirst (removeMemberPrefix name) ++ "() const;\n\t" ++ generateNamespaceString namespace ++ typeName ++ "& Get" ++ capitalizeFirst (removeMemberPrefix name) ++ "();\n"

generateClassMembersGetterCpp :: String -> ClassMember -> String
generateClassMembersGetterCpp klassName (ClassMember name typeName _ namespace) = "const " ++ generateNamespaceString namespace ++ typeName ++ "& " ++ klassName ++ "::Get" ++ capitalizeFirst (removeMemberPrefix name) ++ "() const\n"
                                                                ++"{\n\treturn " ++ name ++ ";\n}\n\n"
                                                                ++ generateNamespaceString namespace ++ typeName ++ "& " ++ klassName ++ "::Get" ++ capitalizeFirst (removeMemberPrefix name) ++ "()\n"
                                                                ++"{\n\treturn " ++ name ++ ";\n}\n"

generateClassSetterHpp :: ClassMember -> String
generateClassSetterHpp (ClassMember name typeName _ namespace) = "\tvoid" ++ " Set" ++ capitalizeFirst (removeMemberPrefix name) ++ "(const " ++ generateNamespaceString namespace ++ typeName ++ "& value);\n"

generateClassSetterCpp :: String -> ClassMember -> String
generateClassSetterCpp klassName (ClassMember name typeName _ namespace) = "void " ++ klassName ++ "::Set" ++ capitalizeFirst (removeMemberPrefix name) ++ "(const " ++ generateNamespaceString namespace ++ typeName ++ "& value)\n"
                                                        ++ "{\n\t" ++ name ++ " = value;\n}\n"

generateClassMembers :: ClassMember -> String
generateClassMembers (ClassMember name typeName isPublic namespace) = "\t" ++ if isPublic then "public: " else "private:\n\t" ++ generateNamespaceString namespace ++ typeName ++ " " ++ name ++ ";\n"

generateMethodParameters :: Param -> String
generateMethodParameters (Param name type') = type' ++ " " ++ name

generateMethod :: Method -> String
generateMethod (Method name returnType _ params) = "\t" ++ returnType ++ " " ++ name ++ "(" ++ concatMap generateMethodParameters params ++ ");\n"

generateClassHpp :: String -> Class -> String
generateClassHpp api (Class name True baseClass methods members) = "class " ++ api ++ " " ++ name ++ ": public " ++ baseClass ++ "\n{\n"
                                                        ++ "public:\n" ++ concatMap generateClassMembersGetterHpp members ++ "\n"
                                                        ++ concatMap generateClassSetterHpp members ++ "\n"
                                                        ++ concatMap generateMethod methods ++ "\n"
                                                        ++ "\tCCT_OBJECT(" ++ name ++ ");\n"
                                                        ++ "private:\n"
                                                        ++ concatMap generateClassMembers members ++ "};\n"
generateClassHpp _ _ = "" --class is defined by the user, no need to generate it

generateClassCpp :: Class -> String
generateClassCpp (Class name True baseClass _ members) = concatMap (generateClassMembersGetterCpp name) members ++ "\n"
                                                    ++ concatMap (generateClassSetterCpp name) members ++ "\n"
generateClassCpp _ = "" --class is defined by the user, no need to generate it



