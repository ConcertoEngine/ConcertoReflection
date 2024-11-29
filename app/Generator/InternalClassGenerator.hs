module Generator.InternalClassGenerator where
import Types


generateInternalClassMembers :: ClassMember -> String
generateInternalClassMembers (ClassMember name typeName _ namespace) = "\tAddMemberVariable(\"" ++ name ++ "\"sv, cct::refl::GetClassByName(\"" ++ namespace ++ "\"sv, \"" ++ typeName ++ "\"sv));\n"

generateMethodParameters :: Param -> String
generateMethodParameters (Param _ type') = "cct::refl::GetClassByName(\"\"sv, \"" ++ type' ++ "\"sv), "

generateInternalMethod :: Method -> String
generateInternalMethod (Method name returnType _ params) = "\tAddMemberFunction(\"" ++ name ++ "\"sv, cct::refl::GetClassByName(\"\"sv, \"" ++ returnType ++ "\"sv), { " ++ concatMap generateMethodParameters params ++ " });\n"

generateInternalClass :: Class -> String
generateInternalClass (Class name _ methods members) = do
                                                    "class Internal" ++ name ++ "Class : public cct::refl::Class\n{\n"
                                                    ++ "public:\n"
                                                    ++  "Internal" ++ name ++ "Class() : cct::refl::Class(nullptr, \"" ++ name ++ "\"s, nullptr)\n"
                                                    ++ "{\n"
                                                    ++  concatMap generateInternalClassMembers members
                                                    ++  concatMap generateInternalMethod methods
                                                    ++ "}\n"
                                                    ++ "static std::shared_ptr<const cct::refl::Class> CreateClassInstance()\n"
                                                    ++ "{\n"
                                                    ++  "\tif (" ++ name ++ "::_class != nullptr)\n"
                                                    ++  "\t{\n"
                                                    ++  "\t\tCONCERTO_ASSERT_FALSE(\"Class already created\");\n"
                                                    ++  "\t\treturn " ++ name ++ "::_class;\n"
                                                    ++  "\t}\n"
                                                    ++  "\t" ++ name ++ "::_class" ++ " = std::shared_ptr<Internal" ++ name ++ "Class>();\n"
                                                    ++  "\treturn " ++ name ++ "::_class;\n"
                                                    ++  "}\n"
                                                    ++  "};\n\n"
