module Generator.InternalClassGenerator where
import Types


generateInternalClassMembers :: ClassMember -> String
generateInternalClassMembers (ClassMember name typeName _ namespace) = "\tAddMemberVariable(\"" ++ name ++ "\"sv, cct::refl::GetClassByName(\"" ++ namespace ++ "\"sv, \"" ++ typeName ++ "\"sv));\n"

generateMethodParameters :: Param -> String
generateMethodParameters (Param _ type') = "cct::refl::GetClassByName(\"\"sv, \"" ++ type' ++ "\"sv), "

generateInternalMethod :: Method -> String
generateInternalMethod (Method name returnType _ params) = "\tAddMemberFunction(\"" ++ name ++ "\"sv, cct::refl::GetClassByName(\"" ++ returnType ++ "\"sv), { " ++ concatMap generateMethodParameters params ++ " });\n"

generateInternalClass :: Class -> String
generateInternalClass (Class name _ baseClass methods members) = "class Internal" ++ name ++ "Class : public cct::refl::Class\n{\n"
                                                    ++ "public:\n"
                                                    ++  "Internal" ++ name ++ "Class() : cct::refl::Class(nullptr, \"" ++ name ++ "\"s, nullptr)\n"
                                                    ++ "{\n}\n"
                                                    ++ "void Initialize()\n{\n"
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

generateInternalClassCreation :: Class -> String
generateInternalClassCreation (Class name _ baseClass _ _) = "\t\tnameSpace->AddClass(\"" ++ name ++ "\"s, cct::refl::GetClassByName(\"" ++ baseClass ++ "\"sv));\n"

generateInternalNamespaceClassCreation :: Namespace -> String
generateInternalNamespaceClassCreation (Namespace name _ classes) = "\t{\n\t\tauto nameSpace = AddNamespace(\"" ++ name ++ "\"s);\n"
                                                                ++ concatMap generateInternalClassCreation classes
                                                                ++ "\t}\n"

generateInternalPackage :: Package -> String
generateInternalPackage (Package name version description includes namespaces classes enums) = do
                                                "class Internal" ++ name ++ "Package : public cct::refl::Package\n{\n"
                                              ++ "public:\n"
                                              ++  "Internal" ++ name ++ "Package() : cct::refl::Package(\"" ++ name ++ "\"s)\n"
                                              ++ "{\n}\n"
                                              ++ "void Initialize()\n{\n"
                                              ++ "\t{\n\t\tauto nameSpace = AddNamespace(\"Global\"s);\n"
                                              ++  concatMap generateInternalClassCreation classes
                                              ++ "\t}\n"
                                              ++  concatMap generateInternalNamespaceClassCreation namespaces
                                              ++ "}\n"
                                              ++ "};\n\n"