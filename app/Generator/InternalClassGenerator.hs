module Generator.InternalClassGenerator where
import Types


generateInternalClassMembers :: ClassMember -> String
generateInternalClassMembers (ClassMember name typeName _ namespace) = "\tAddMemberVariable(\"" ++ name ++ "\"sv, cct::refl::GetClassByName(\"" ++ namespace ++ "\"sv, \"" ++ typeName ++ "\"sv));\n"

generateMethodParameters :: Param -> String
generateMethodParameters (Param _ type') = "cct::refl::GetClassByName(\"\"sv, \"" ++ type' ++ "\"sv), "

generateInternalMethod :: Method -> String
generateInternalMethod (Method name returnType _ params) = "\tAddMemberFunction(\"" ++ name ++ "\"sv, cct::refl::GetClassByName(\"" ++ returnType ++ "\"sv), { " ++ concatMap generateMethodParameters params ++ " });\n"

generateInternalClass :: String -> Class -> String
generateInternalClass namespaceName (Class name _ baseClass methods members) = "class Internal" ++ name ++ "Class : public cct::refl::Class\n{\n"
                                                    ++ "public:\n"
                                                    ++  "Internal" ++ name ++ "Class() : cct::refl::Class(nullptr, \"" ++ name ++ "\"s, nullptr)\n"
                                                    ++ "{\n}\n"
                                                    ++ "void Initialize() override\n{\n"
                                                    ++ "\tSetNamespace(GetNamespaceByName(\"" ++ namespaceName ++"\"sv));\n"
                                                    ++ "\tSetBaseClass(GetClassByName(\"" ++ baseClass ++ "\"sv));\n"
                                                    ++  concatMap generateInternalClassMembers members
                                                    ++  concatMap generateInternalMethod methods
                                                    ++ "}\n"
                                                    ++ "static std::shared_ptr<cct::refl::Class> CreateClassInstance()\n"
                                                    ++ "{\n"
                                                    ++  "\tif (" ++ name ++ "::_class != nullptr)\n"
                                                    ++ "\t{\n"
                                                    ++ "\t\tCCT_ASSERT_FALSE(\"Class already created\");\n"
                                                    ++ "\t\treturn " ++ name ++ "::_class;\n"
                                                    ++ "\t}\n"
                                                    ++ "\t" ++ name ++ "::_class" ++ " = std::make_shared<Internal" ++ name ++ "Class>();\n"
                                                    ++ "\treturn " ++ name ++ "::_class;\n"
                                                    ++ "}\n"
                                                    ++ "};\n\n"
                                                    ++ "std::shared_ptr<cct::refl::Class> " ++ name ++ "::_class = nullptr;\n\n"

generateInternalClassCreation :: Class -> String
generateInternalClassCreation (Class name _ baseClass _ _) = "\tAddClass(Internal" ++ name ++ "Class::CreateClassInstance());\n"

generateInternalNamespaceCreation :: Namespace -> String
generateInternalNamespaceCreation (Namespace name _ classes) = "class Internal" ++ name ++ "Namespace : public cct::refl::Namespace\n{\n"
                                                        ++ "public:\n"
                                                        ++  "Internal" ++ name ++ "Namespace() : cct::refl::Namespace(\"" ++ name ++ "\"s)\n"
                                                        ++ "{\n}\n"
                                                        ++ "void LoadClasses() override\n{\n" ++  concatMap generateInternalClassCreation classes ++ "}\n"
                                                        ++ "void InitializeClasses() override\n{\n\tfor (auto& klass : _classes)\n\t{\n\t\tklass->Initialize();\n\t}\n}\n"
                                                        ++ "static std::shared_ptr<cct::refl::Namespace> CreateNamespaceInstance(){return std::make_shared<Internal" ++ name ++ "Namespace>();}\n"
                                                        ++  "};\n\n"

generateInternalPackage :: Package -> String
generateInternalPackage (Package name version description includes namespaces classes enums) = do
                                                "class Internal" ++ name ++ "Package : public cct::refl::Package\n{\n"
                                              ++ "public:\n"
                                              ++  "Internal" ++ name ++ "Package() : cct::refl::Package(\"" ++ name ++ "\"s)\n"
                                              ++ "{\n}\n"
                                              ++ "void LoadNamespaces() override\n{\n"
                                              ++  concatMap (\(Namespace namespaceName _ classes) -> "\tAddNamespace(Internal" ++ namespaceName ++ "Namespace::CreateNamespaceInstance());\n") namespaces
                                              ++ "}\n"
                                              ++ "void InitializeNamespaces() override\n{\n"
                                              ++  concatMap (\klass -> "\tGetNamespaceByName(\"Global\"sv)->" ++ generateInternalClassCreation klass ++ "\n") classes
                                              ++  "\tfor (auto& namespace_ : _namespaces)\n\t{\n\t\tnamespace_->LoadClasses();\n\t}\n"
                                              ++ "}\n"
                                              ++ "void InitializeClasses() override\n{\n"
                                              ++ "\tfor (auto& namespace_ : _namespaces)\n\t{\n\t\tnamespace_->InitializeClasses();\n\t}\n}\n"
                                              ++ "};\n\n"