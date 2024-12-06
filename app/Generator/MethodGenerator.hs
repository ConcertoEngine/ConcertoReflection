module Generator.MethodGenerator where
import Types

generateMethodParameters :: Param -> String
generateMethodParameters (Param name type') = ", " ++ type' ++ " " ++ name

generateMethodCallParameters :: [Param] -> String
generateMethodCallParameters [] = ""
generateMethodCallParameters [x] = paramName x
generateMethodCallParameters (x:xs) = paramName x ++ ", " ++ generateMethodCallParameters xs

generateMethodLocalVariables :: Int -> Param -> String
generateMethodLocalVariables index (Param name type') = "\t\t" ++ type' ++ "& " ++ name ++ " = parameters[" ++ show index ++ "].As<" ++ type' ++ "&>();\n"

generateMethodReturnType :: String -> String -> String
generateMethodReturnType "void" funcCall = "\t\t" ++ funcCall ++ ";\n\t\treturn {};\n"
generateMethodReturnType type' funcCall = "\t\tdecltype(" ++ funcCall ++ ") result = " ++ funcCall ++ ";\n\t\treturn cct::Any::Make<" ++ type' ++">(result);\n"

generateMethodPredeclaration :: String -> Method -> String
generateMethodPredeclaration klassName (Method name _ _ _) = "class " ++ klassName ++ name ++ "Method;\n"

generateMethod :: String -> Method -> String
generateMethod klassName (Method name returnType _ params) = "class " ++ klassName ++ name ++ "Method : public Method\n{\n"
                                                        ++ "public:\n"
                                                        ++ "\tusing Method::Method;\n"
                                                        ++ "\tcct::Any Invoke(cct::refl::Object& self, std::span<cct::Any> parameters) const override\n"
                                                        ++ "\t{\n"
                                                        ++ "\t\tCCT_ASSERT(parameters.size() == " ++ show (length params) ++ ", \"Invalid parameter size\");\n"
                                                        ++ concat (zipWith generateMethodLocalVariables [0..] params)
                                                        ++ generateMethodReturnType returnType ("static_cast<" ++ klassName ++"&>(self)." ++ name ++ "(" ++ generateMethodCallParameters params ++ ")")
                                                        ++ "\t}\n"
                                                        ++ "};\n"