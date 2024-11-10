module Generator.ClassGenerator where
import Types
import Data.Char (toUpper)

capitalizeFirst :: String -> String
capitalizeFirst ""     = ""
capitalizeFirst (x:xs) = toUpper x : xs

removeMemberPrefix :: String -> String
removeMemberPrefix ('_':xs) = xs
removeMemberPrefix xs       = xs

generateClassMembersGetterHpp :: ClassMember -> String
generateClassMembersGetterHpp (ClassMember name typeName _) = "\tinline const " ++ typeName ++ "& Get" ++ capitalizeFirst (removeMemberPrefix name) ++ "() const;\n\tinline " ++ typeName ++ "& Get" ++ capitalizeFirst (removeMemberPrefix name) ++ "();\n"

generateClassMembersGetterCpp :: String -> ClassMember -> String
generateClassMembersGetterCpp klassName (ClassMember name typeName _) = "inline const " ++ typeName ++ "& " ++ klassName ++ "::Get" ++ capitalizeFirst (removeMemberPrefix name) ++ "() const\n"
                                                                ++"{\n\treturn " ++ name ++ ";\n}\n\n"
                                                                ++ "inline " ++ typeName ++ "& " ++ klassName ++ "::Get" ++ capitalizeFirst (removeMemberPrefix name) ++ "()\n"
                                                                ++"{\n\treturn " ++ name ++ ";\n}\n"

generateClassSetterHpp :: ClassMember -> String
generateClassSetterHpp (ClassMember name typeName _) = "\tinline void" ++ " Set" ++ capitalizeFirst (removeMemberPrefix name) ++ "(const " ++ typeName ++ "& value);\n"

generateClassSetterCpp :: String -> ClassMember -> String
generateClassSetterCpp klassName (ClassMember name typeName _) = "inline void " ++ klassName ++ "::Set" ++ capitalizeFirst (removeMemberPrefix name) ++ "(const " ++ typeName ++ "& value)\n"
                                                        ++ "{\n\t" ++ name ++ " = value;\n}\n"

generateClassMembers :: ClassMember -> String
generateClassMembers (ClassMember name typeName isPublic) = "\t" ++ if isPublic then "public: " else "private: " ++ typeName ++ " " ++ name ++ ";\n"

generateMethodParameters :: Param -> String
generateMethodParameters (Param name type') = type' ++ " " ++ name

generateMethod :: Method -> String
generateMethod (Method name returnType _ params) = "\t" ++ returnType ++ " " ++ name ++ "(" ++ concatMap generateMethodParameters params ++ ");\n"

generateClassHpp :: String -> Class -> String
generateClassHpp api (Class name True methods members) = "class " ++ api ++ " " ++ name ++ "\n{\n"
                                                        ++ "public:\n" ++ concatMap generateClassMembersGetterHpp members ++ "\n"
                                                        ++ concatMap generateClassSetterHpp members ++ "\n"
                                                        ++ concatMap generateMethod methods ++ "\n"
                                                        ++ "private:\n"
                                                        ++ concatMap generateClassMembers members ++ "};\n"
generateClassHpp _ _ = "" --class is defined by the user, no need to generate it

generateClassCpp :: Class -> String
generateClassCpp (Class name True _ members) = concatMap (generateClassMembersGetterCpp name) members ++ "\n"
                                                    ++ concatMap (generateClassSetterCpp name) members
generateClassCpp _ = "" --class is defined by the user, no need to generate it



