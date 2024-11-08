module Types where

data Param = Param {
                paramName :: String,
                paramType :: String
            } deriving (Show)

data Method = Method {
                methodName :: String,
                methodReturnType :: String,
                methodIsPublic :: Bool,
                methodParameters :: [(String, String)]
            } deriving (Show)

data Class = Class {
                className :: String,
                classMethods :: [Method],
                classMembers :: [ClassMember]
            } deriving (Show)

data ClassMember = ClassMember {
                    classMemberName :: String,
                    classMemberType :: String,
                    classMemberIsPublic :: Bool,
                    classMemberHasAccessors :: Bool
                } deriving (Show)

data EnumMember = EnumMember {
                    enumMemberName :: String,
                    enumMemberValue :: Int
                } deriving (Show)

data Enumeration = Enum {
                    enumName :: String,
                    enumBase :: String,
                    enumMembers :: [EnumMember]
                } deriving (Show)

data Namespace = Namespace {
                    namespaceName :: String,
                    namespaceEnums :: [Enumeration],
                    namespaceClasses :: [Class]
                } deriving (Show)

data Include = Include {
                    includeFile :: String,
                    includeIsPublic :: Bool
                } deriving (Show)

data File = File {
                includes :: [Include],
                namespaces :: [Namespace],
                classes :: [Class],
                enums :: [Enumeration]
            } deriving (Show)
