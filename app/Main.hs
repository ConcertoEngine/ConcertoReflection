module Main where
import Parser
import Text.XML.Light

main :: IO ()
main = do
        s <- readFile "sample.xml"
        case parseXMLDoc s of
                Nothing -> error "Failed to parse xml"
                Just doc  -> case parsePackage doc of
                                Just result -> case result of
                                                Right p -> print p
                                                Left err -> putStr err
                                Nothing -> print "No package found"