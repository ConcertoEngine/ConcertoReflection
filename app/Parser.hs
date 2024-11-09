module Parser where

import Text.XML.Light
import Types
import Data.Maybe (mapMaybe)
import Data.Either (partitionEithers)



parseInclude :: Element -> Maybe (Either String Include)
parseInclude (Element (QName "include" Nothing Nothing) attrs _ line) = do
    let fileAttr = lookupAttr (unqual "file") attrs
    let publicAttr = lookupAttr (unqual "public") attrs

    let errorMsg attr = "<include> missing '" ++ attr ++ "' attribute on line: " ++  maybe "X" show line
    let file = maybe (Left $ errorMsg "file") Right fileAttr
    let public = maybe (Right False) (Right . (== "true")) publicAttr

    case file of
        Right f -> Just (Right Include { includeFile = f, includeIsPublic = either id id public })
        Left err -> Just (Left err)
parseInclude _ = Nothing


parseClassMember :: Element -> Maybe (Either String ClassMember)
parseClassMember (Element (QName "member" Nothing Nothing) attrs _ line) = do
    let nameAttr = lookupAttr (unqual "name") attrs
    let typeAttr = lookupAttr (unqual "type") attrs
    let publicAttr = lookupAttr (unqual "public") attrs

    let errorMsg attr = "<member> missing '" ++ attr ++ "' attribute on line: " ++  maybe "X" show line
    let name = maybe (Left $ errorMsg "name") Right nameAttr
    let type' = maybe (Left $ errorMsg "type") Right typeAttr
    let public = maybe (Right False) (Right . (== "true")) publicAttr

    case (name, type') of
        (Right n, Right t) -> Just (Right ClassMember { classMemberName = n, classMemberType = t, classMemberIsPublic = either id id public })
        (Left err, _) -> Just (Left err)
        (_, Left err) -> Just (Left err)
parseClassMember _ = Nothing


parseParam :: Element -> Maybe (Either String Param)
parseParam (Element (QName "param" Nothing Nothing) attrs _ line) = do
    let nameAttr = lookupAttr (unqual "name") attrs
    let typeAttr = lookupAttr (unqual "type") attrs

    let errorMsg attr = "<param> missing '" ++ attr ++ "' attribute on line: " ++  maybe "X" show line
    let name = maybe (Left $ errorMsg "name") Right nameAttr
    let type' = maybe (Left $ errorMsg "type") Right typeAttr

    case (name, type') of
        (Right n, Right t) -> Just (Right Param { paramName = n, paramType = t })
        _ -> Just (Left (either id id name ++ either id id type'))
parseParam _ = Nothing

parseMethod :: Element -> Maybe (Either String Method)
parseMethod (Element (QName "method" Nothing Nothing) attrs elems line) = do
    let nameAttr = lookupAttr (unqual "name") attrs
    let returnAttr = lookupAttr (unqual "return") attrs
    let publicAttr = lookupAttr (unqual "public") attrs
    let (errors, parameters) = partitionEithers (mapMaybe parseParam (onlyElems elems))

    let errorMsg attr = "<method> missing '" ++ attr ++ "' attribute on line: " ++  maybe "X" show line
    let name = maybe (Left $ errorMsg "name") Right nameAttr
    let returnType = maybe (Left $ errorMsg "return") Right returnAttr
    let public = maybe (Left $ errorMsg "public") Right publicAttr

    case (name, returnType, public) of
        (Right n, Right r, Right p) ->
            if null errors
                then Just (Right Method { methodName = n, methodReturnType = r, methodIsPublic = p == "true", methodParameters = parameters })
                else Just (Left (concat errors ++ "\n"))
        _ -> Just (Left $ concat [either id id name, either id id returnType, either id id public])
parseMethod _ = Nothing


parseClass :: Element -> Maybe (Either String Class)
parseClass (Element (QName "class" Nothing Nothing) attrs elems line) = do
    let nameAttr = lookupAttr (unqual "name") attrs
    let (methodErrors, methods) = partitionEithers (mapMaybe parseMethod (onlyElems elems))
    let (memberErrors, members) = partitionEithers (mapMaybe parseClassMember (onlyElems elems))
    let errors = methodErrors ++ memberErrors

    let errorMsg attr = "<class> missing '" ++ attr ++ "' attribute on line: " ++  maybe "X" show line
    let name = maybe (Left $ errorMsg "name") Right nameAttr

    case name of
        Right n ->
            if null errors
                then Just (Right Class { className = n, classMethods = methods, classMembers = members })
                else Just (Left (concat errors ++ "\n"))
        _ -> Just (Left $ either id id name)
parseClass _ = Nothing

parseEnumMember :: Element -> Maybe (Either String EnumMember)
parseEnumMember (Element (QName "member" Nothing Nothing) attrs _ line) = do
    let nameAttr = lookupAttr (unqual "name") attrs
    let valueAttr = lookupAttr (unqual "value") attrs

    let errorMsg attr = "<member> missing '" ++ attr ++ "' attribute on line: " ++  maybe "X" show line
    let name = maybe (Left $ errorMsg "name") Right nameAttr
    let value = maybe (Left $ errorMsg "value") Right valueAttr

    case (name, value) of
        (Right n, Right v) -> Just (Right EnumMember { enumMemberName = n, enumMemberValue = read v })
        _ -> Just (Left (either id id name ++ either id id value))
parseEnumMember _ = Nothing

parseEnum :: Element -> Maybe (Either String Enumeration)
parseEnum (Element (QName "enum" Nothing Nothing) attrs elems line) = do
    let nameAttr = lookupAttr (unqual "name") attrs
    let baseAttr = lookupAttr (unqual "base") attrs
    let (errors, members) = partitionEithers (mapMaybe parseEnumMember (onlyElems elems))

    let errorMsg attr = "<enum> missing '" ++ attr ++ "' attribute on line: " ++  maybe "X" show line
    let name = maybe (Left $ errorMsg "name") Right nameAttr
    let base = maybe (Left $ errorMsg "base") Right baseAttr

    case (name, base) of
        (Right n, Right b) ->
            if null errors
                then Just (Right Enumeration { enumName = n, enumBase = b, enumMembers = members })
                else Just (Left (concat errors ++ "\n"))
        _ -> Just (Left $ either id id name ++ either id id base)
parseEnum _ = Nothing

parseNamespace :: Element -> Maybe (Either String Namespace)
parseNamespace (Element (QName "namespace" Nothing Nothing) attrs elems _) = do
    let nameAttr = lookupAttr (unqual "name") attrs

    let (enumErrors, enumerations) = partitionEithers (mapMaybe parseEnum (onlyElems elems))
    let (klassErros, klasses) = partitionEithers (mapMaybe parseClass (onlyElems elems))
    let errors = enumErrors ++ klassErros

    let errorMsg attr = "<namespace> missing '" ++ attr ++ "' attribute"
    let name = maybe (Left $ errorMsg "name") Right nameAttr

    case name of
        Right n ->
            if null errors
                then Just (Right Namespace { namespaceName = n, namespaceEnums = enumerations, namespaceClasses = klasses })
                else Just (Left (concat errors ++ "\n"))
        _ -> Just (Left $ either id id name)
parseNamespace _ = Nothing

parsePackage :: Element -> Maybe (Either String Package)
parsePackage (Element (QName "package" Nothing Nothing) attrs contents _) = do
    let versionAttr = lookupAttr (unqual "version") attrs
    let descriptionAttr = lookupAttr (unqual "description") attrs

    let elems = onlyElems contents
    let (includeErrors, includes) = partitionEithers (mapMaybe parseInclude elems)
    let (namespaceErrors, namespaces) = partitionEithers (mapMaybe parseNamespace elems)
    let (classErrors, classes) = partitionEithers (mapMaybe parseClass elems)
    let (enumErrors, enums) = partitionEithers (mapMaybe parseEnum elems)
    let errors = includeErrors ++ namespaceErrors ++ classErrors ++ enumErrors

    let errorMsg attr = "<package> missing '" ++ attr ++ "' attribute"
    let name = maybe (Left $ errorMsg "name") Right (lookupAttr (unqual "name") attrs)
    let version = maybe (Left $ errorMsg "version") Right versionAttr
    let description = maybe (Left $ errorMsg "description") Right descriptionAttr

    case (name, version, description) of
        (Right n, Right v, Right d) ->
            if null errors
                then Just (Right Package { packageName = n, packageVersion = v, packageDescription = d, packageIncludes = includes, packageNamespaces = namespaces, packageClasses = classes, packageEnums = enums })
                else Just (Left (concat errors ++ "\n"))
        _ -> Just (Left $ concat [either id id name, either id id version, either id id description])
parsePackage _ = Nothing