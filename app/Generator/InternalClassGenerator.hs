module Generator.InternalClassGenerator where
import Types
import Generator.MethodGenerator


generateInternalClassMembers :: ClassMember -> String
generateInternalClassMembers (ClassMember name typeName _ namespace) = "\tAddMemberVariable(\"" ++ name ++ "\"sv, cct::refl::GetClassByName(\"" ++ namespace ++ "\"sv, \"" ++ typeName ++ "\"sv));\n"

generateInternalMethodParameters :: Param -> String
generateInternalMethodParameters (Param _ type') = "cct::refl::GetClassByName(\"\"sv, \"" ++ type' ++ "\"sv), "

generateInternalMethod :: String -> Method -> String
generateInternalMethod klass (Method name returnType _ params) = "\tauto* " ++ name ++ "Method = new " ++ klass ++ name ++ "Method(\"" ++ name ++ "\"sv, cct::refl::GetClassByName(\"" ++ returnType ++ "\"sv), { " ++ concatMap generateInternalMethodParameters params ++ " });\n"
                                                              ++ "\tAddMemberFunction(std::unique_ptr<cct::refl::Method>(" ++ name ++ "Method));\n"

generateInternalClass :: String -> Class -> String
generateInternalClass namespaceName (Class name _ baseClass methods members) = "class Internal" ++ name ++ "Class;\n"
                                                    ++ concatMap (Generator.MethodGenerator.generateMethod name) methods
                                                    ++ "class Internal" ++ name ++ "Class : public cct::refl::Class\n{\n"
                                                    ++ "public:\n"
                                                    ++  "Internal" ++ name ++ "Class() : cct::refl::Class(nullptr, \"" ++ name ++ "\"s, nullptr)\n"
                                                    ++ "{\n}\n"
                                                    ++ "void Initialize() override\n{\n"
                                                    ++ "\tSetNamespace(GetNamespaceByName(\"" ++ namespaceName ++"\"sv));\n"
                                                    ++ "\tSetBaseClass(GetClassByName(\"" ++ baseClass ++ "\"sv));\n"
                                                    ++  concatMap generateInternalClassMembers members
                                                    ++  concatMap (generateInternalMethod name) methods
                                                    ++ "}\n"
                                                    ++ "std::unique_ptr<cct::refl::Object> CreateDefaultObject() const override { return std::unique_ptr<cct::refl::Object>(new " ++ name ++ "); }\n"
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
generateInternalNamespaceCreation (Namespace name _ classes (x:xs)) = generateInternalNamespaceCreation x
                                                        ++ concatMap generateInternalNamespaceCreation xs
                                                        ++  "class Internal" ++ name ++ "Namespace : public cct::refl::Namespace\n{\n"
                                                        ++ "public:\n"
                                                        ++  "Internal" ++ name ++ "Namespace() : cct::refl::Namespace(\"" ++ name ++ "\"s)\n"
                                                        ++ "{\n}\n"
                                                        ++ "void LoadNamespaces() override\n{\n" ++ concatMap generateAddNamespaceInternalPackage (x:xs) ++ "\tfor (auto& namespace_ : _namespaces)\n\t\tnamespace_->LoadNamespaces();\n}\n"
                                                        ++ "void LoadClasses() override\n{\n" ++ "for(auto& n : _namespaces)\n\tn->LoadClasses();\n" ++ concatMap generateInternalClassCreation classes ++ "}\n"
                                                        ++ "void InitializeClasses() override\n{\n\tfor (auto& klass : _classes)\n\t{\n\t\tklass->Initialize();\n\t}\n}\n"
                                                        ++ "static std::shared_ptr<cct::refl::Namespace> CreateNamespaceInstance(){return std::make_shared<Internal" ++ name ++ "Namespace>();}\n"
                                                        ++  "};\n\n"
generateInternalNamespaceCreation (Namespace name _ classes []) = "class Internal" ++ name ++ "Namespace : public cct::refl::Namespace\n{\n"
                                                        ++ "public:\n"
                                                        ++  "Internal" ++ name ++ "Namespace() : cct::refl::Namespace(\"" ++ name ++ "\"s)\n"
                                                        ++ "{\n}\n"
                                                        ++ "void LoadNamespaces() override\n{\tfor (auto& namespace_ : _namespaces)\n\t\tnamespace_->LoadNamespaces();\n}\n"
                                                        ++ "void LoadClasses() override\n{\n" ++ "\tfor(auto& n : _namespaces)\n\t\tn->LoadClasses();\n" ++ concatMap generateInternalClassCreation classes ++ "}\n"
                                                        ++ "void InitializeClasses() override\n{\n\tfor (auto& klass : _classes)\n\t{\n\t\tklass->Initialize();\n\t}\n}\n"
                                                        ++ "static std::shared_ptr<cct::refl::Namespace> CreateNamespaceInstance(){return std::make_shared<Internal" ++ name ++ "Namespace>();}\n"
                                                        ++  "};\n\n"

generateAddNamespaceInternalPackage :: Namespace -> String
generateAddNamespaceInternalPackage (Namespace name _ _ _) = "\tAddNamespace(Internal" ++ name ++ "Namespace::CreateNamespaceInstance());\n"

generateInternalPackage :: Package -> String
generateInternalPackage (Package name version description includes namespaces classes enums) = do
                                                "class Internal" ++ name ++ "Package : public cct::refl::Package\n{\n"
                                              ++ "public:\n"
                                              ++  "Internal" ++ name ++ "Package() : cct::refl::Package(\"" ++ name ++ "\"s)\n"
                                              ++ "{\n}\n"
                                              ++ "void LoadNamespaces() override\n{\n"
                                              ++  concatMap generateAddNamespaceInternalPackage namespaces
                                              ++ "\tfor (auto& namespace_ : _namespaces)\n\t\tnamespace_->LoadNamespaces();\n"
                                              ++ "}\n"
                                              ++ "void InitializeNamespaces() override\n{\n"
                                              ++  concatMap (\klass -> "\tNamespace::GetGlobalNamespace()->" ++ generateInternalClassCreation klass ++ "\n") classes
                                              ++  "\tfor (auto& namespace_ : _namespaces)\n\t{\n\t\tnamespace_->LoadClasses();\n\t}\n"
                                              ++ "}\n"
                                              ++ "void InitializeClasses() override\n{\n"
                                              ++ "\tfor (auto& namespace_ : _namespaces)\n\t{\n\t\tnamespace_->InitializeClasses();\n\t}\n}\n"
                                              ++ "};\n\n"