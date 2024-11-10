module Main where
import Text.XML.Light
import Parser
import Generator
import System.Environment (getArgs)

main :: IO ()
main = do
        -- generator.exe sample.xml -./output
        args <- getArgs
        if length args /= 2
            then do putStr "Missing argument usage : generator.exe input.xml outputDir\n"
        else do
                let input = head args
                let output = args !! 1
                xml <- readFile input
                case parseXMLDoc xml of
                        Nothing -> putStr "Failed to parse xml\n"
                        Just doc  -> case parsePackage doc of
                                        Just result -> case result of
                                                        Right p -> generate p output
                                                        Left err -> putStr err
                                        Nothing -> print "No package found"