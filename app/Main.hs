module Main where
import Text.XML.Light
import Parser
import Generator

main :: IO ()
main = do
        s <- readFile "sample.xml"
        case parseXMLDoc s of
                Nothing -> error "Failed to parse xml"
                Just doc  -> case parsePackage doc of
                                Just result -> case result of
                                                Right p -> generate p
                                                Left err -> putStr err
                                Nothing -> print "No package found"