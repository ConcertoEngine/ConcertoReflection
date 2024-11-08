import Text.XML.Light
import Types
import Data.Foldable (find)

-- <?xml version="1.0" encoding="UTF-8"?>

-- <include file="Concerto/Core/Types" public="true"/>
-- <namespace name="cct">

--     <enum name="SampleEnum" base="UInt32">
--         <member name="Foo" value="0"/>
--         <member name="Bar" value="1"/>
--     </enum>

--     <class name="Sample">
--         <method name="Foo" return="void" public="true">
--             <param name="bar" type="Int32"/>
--         </method>

--         <member name="_bar" type="Int32" public="false" accessors="true"/>
--     </class>

-- </namespace>


-- isInclude :: Element -> Bool
-- isInclude (Element "include" _ _ _) = True

findAttr :: QName -> [Attr] -> Maybe String
findAttr qname attrs = do
    let attr = find (\(Attr name _) -> name == qname) attrs
    case attr of
        Just (Attr _ value) -> Just value
        Nothing -> Nothing

parseInclude :: Element -> Include
parseInclude (Element "include" attrs _ _) = do
    let file = Main.findAttr (QName "file" Nothing Nothing) attrs
    let public = Main.findAttr (QName "public" Nothing Nothing) attrs
    return Include { includeFile = file, includeIsPublic = public }


paseClass :: Element -> Class
paseClass (Element "class" attrs children _) = do
    let name = Main.findAttr (QName "name" Nothing Nothing) attrs
    let methods = map parseMethod $ filter isMethod children
    let members = map parseClassMember $ filter isClassMember children
    return Class { className = name, classMethods = methods, classMembers = members }

parseClassMember :: Element -> ClassMember
parseClassMember (Element "member" attrs _ _) = do
    let name = Main.findAttr (QName "name" Nothing Nothing) attrs
    let type_ = Main.findAttr (QName "type" Nothing Nothing) attrs
    let public = Main.findAttr (QName "public" Nothing Nothing) attrs
    let accessors = Main.findAttr (QName "accessors" Nothing Nothing) attrs
    return ClassMember { classMemberName = name, classMemberType = type_, classMemberIsPublic = public, classMemberHasAccessors = accessors }

parseEnum :: Element -> Enumeration
parseEnum (Element "enum" attrs children _) = do
    let name = Main.findAttr (QName "name" Nothing Nothing) attrs
    let base = Main.findAttr (QName "base" Nothing Nothing) attrs
    let members = map parseEnumMember $ filter isEnumMember children
    return Enum { enumName = name, enumBase = base, enumMembers = members }

parseEnumMember :: Element -> EnumMember
parseEnumMember (Element "member" attrs _ _) = do
    let name = Main.findAttr (QName "name" Nothing Nothing) attrs
    let value = Main.findAttr (QName "value" Nothing Nothing) attrs
    return EnumMember { enumMemberName = name, enumMemberValue = read value }

parseNamespace :: Element -> Namespace
parseNamespace (Element "namespace" attrs children _) = do
    let name = Main.findAttr (QName "name" Nothing Nothing) attrs
    let enums = map parseEnum $ filter isEnum children
    let classes = map parseClass $ filter isClass children
    return Namespace { namespaceName = name, namespaceEnums = enums, namespaceClasses = classes }

parseFile :: Element -> File
parseFile (Element _ _ children _) = do
    let includes = map parseInclude $ filter isInclude children
    let namespaces = map parseNamespace $ filter isNamespace children
    let classes = map parseClass $ filter isClass children
    let enums = map parseEnum $ filter isEnum children
    return File { includes = includes, namespaces = namespaces, classes = classes, enums = enums }

main :: IO ()
main = do
        s <- readFile "sample.xml"
        case parseXMLDoc s of
                Nothing -> error "Failed to parse xml"
                Just doc  -> putStrLn (showElement doc)