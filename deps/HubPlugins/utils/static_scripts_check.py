#!/usr/bin/env python3

import sys
import os
import os.path as path
import json
import re

defaultScriptPrefix = "HUB:"
systemModules = [ "core", "zigbee", "zwave", "2gig", "timer", "storage", "cameras", "bluetooth", "plughub", "water_valve", "cloud_devices", "network", "security", "http", "logging", "speaker", "siren", "house_modes", "scenes", "extensions_manager", "native_plugins_utils", "http_server" ]
prohibitedFileNames = [ "aux", "con", "prn", "nul", "com1", "com2", "com3", "com4", "com5", "com6", "com7", "com8", "com9", "lpt1", "lpt2", "lpt3", "lpt4", "lpt5", "lpt6", "lpt7", "lpt8", "lpt9" ]

def isEnglish(s):
    try:
        s.encode(encoding='utf-8').decode('ascii')
    except UnicodeDecodeError:
        return False
    else:
        return True

def isPluginDir(dirName):
    return path.isfile(path.join(dirName, "config.json"))

def checkIncludedScriptPath(pluginDir, isTestFile, requiredFilePath, contentAfterPath, notExistPaths, wrongPathFormat, unknowModuleName, formattedCurrentLine):
    if requiredFilePath:
        if requiredFilePath not in systemModules:
            if requiredFilePath.startswith( defaultScriptPrefix ):
                contentAfterPath = contentAfterPath.strip()
                if not contentAfterPath.startswith( ".." ):
                    scriptFilePath = requiredFilePath[len(defaultScriptPrefix):]
                    pathComponents = scriptFilePath.split( "/" )
                    if pathComponents and len(pathComponents) > 1:
                        if pluginDir.endswith( pathComponents[0] ):
                            if not path.isfile( pluginDir + "/" + scriptFilePath[ len(pathComponents[0]): ] + ".lua" ):
                                notExistPaths.append( formattedCurrentLine )
                        else:
                            wrongPathFormat.append( formattedCurrentLine )
                    else:
                        wrongPathFormat.append( formattedCurrentLine )
            elif not isTestFile:
                unknowModuleName.append( formattedCurrentLine )

def checkScriptsPathsInScript(pluginDir, scriptPath):

    isTitlePrinted = False
    allIncludesPresent = True

    wrongPathFormat = list()
    unknowModuleName = list()
    notRecognizedRequires = list()
    notLoadfilesRequires = list()
    notExistPaths = list()
    nonEnglishLines = list()

    scriptFile = open( scriptPath, "r" )
    isTestFile = scriptPath.startswith( pluginDir + "/scripts/tests/" )

    lineNumber = 1
    for currentLine in scriptFile:
        currentLine = currentLine.replace( "\n", "" )
        formattedCurrentLine = "\t\t" + str( lineNumber ) + ": " + currentLine
        requiredFilePath = ""
        contentAfterPath = ""
        
        if not isEnglish( currentLine ):
            nonEnglishLines.append( formattedCurrentLine )
        
        if "require" in currentLine:
            moduleMatch = re.findall(".*require\\s*\\(*\\s*\"([A-Za-z1-90:/\\\\_]+)\"(.*)", currentLine)
            if moduleMatch:
                checkIncludedScriptPath(pluginDir, isTestFile, moduleMatch[0][0], moduleMatch[0][1], notExistPaths, wrongPathFormat, unknowModuleName, formattedCurrentLine)
            else:
                notRecognizedRequires.append( formattedCurrentLine )

        if "loadfile" in currentLine:
            moduleMatch = re.findall(".*loadfile\\s*\\(*\\s*\"([A-Za-z1-90:/\\\\_]+)\"(.*)", currentLine)
            if moduleMatch:
                checkIncludedScriptPath(pluginDir, isTestFile, moduleMatch[0][0], moduleMatch[0][1], notExistPaths, wrongPathFormat, unknowModuleName, formattedCurrentLine)
            else:
                notLoadfilesRequires.append( formattedCurrentLine )

        lineNumber += 1

    if len( wrongPathFormat ) > 0 or len( unknowModuleName ) > 0 or len(notExistPaths) > 0 or len( nonEnglishLines ) > 0:
        print( scriptPath + ":" )
        
    if len( notExistPaths ) > 0:
        allIncludesPresent = False
        print( "\tNOT EXIST script paths: " )
        for filePath in notExistPaths:
            print( filePath )

    if len( wrongPathFormat ) > 0:
        allIncludesPresent = False
        print( "\tWrong path format: " )
        for wrongPath in wrongPathFormat:
            print( wrongPath )

    if len( unknowModuleName ) > 0:
        allIncludesPresent = False
        print( "\tUnknown module name: " )
        for wrongModule in unknowModuleName:
            print( wrongModule )
            
    if len( nonEnglishLines ) > 0:
        allIncludesPresent = False
        print( "\tThose lines contain non English characters: " )
        for nonEnglishLine in nonEnglishLines:
            print( nonEnglishLine )
            
#    if len( notRecognizedRequires ) > 0:
#        print("\tNot recognized 'require' format:")
#        for requiresInFile in notRecognizedRequires:
#            print( requiresInFile )
            
#    if len( notLoadfilesRequires ) > 0:
#        print("\tNot recognized 'loadfile' format:")
#        for loadfileInFile in notLoadfilesRequires:
#            print( loadfileInFile )

            
    return allIncludesPresent

def checkFileName(dirName, fileName):
    pos = fileName.rfind(".")
    fileNameWithoutExt = fileName if pos == -1 else fileName[0:pos]

    if fileNameWithoutExt.lower() in prohibitedFileNames:
        print( "Prohibited file name: " + dirName + "/" + fileName )
        return False

    return True

def checkPluginScripts(pluginDir):
    result = True
    for dirName, subdirList, fileList in os.walk(pluginDir):
        for currentFile in fileList:
            if not checkFileName(dirName, currentFile):
                result = False

            if currentFile.endswith( ".lua" ):
                currentFile = os.path.abspath( dirName + "/" + currentFile )
                if not checkScriptsPathsInScript( pluginDir, currentFile ):
                    result = False
    return result

def scanPluginsFolders(pluginsDir):
    result = True
    for dirName, subdirList, fileList in os.walk(pluginsDir):
        dirName = os.path.abspath( dirName )
        if isPluginDir(dirName):
            if not checkPluginScripts( dirName ):
                result = False
    return result

def main():
    print( "Scanning for not existing includes was started:" )
    if len(sys.argv) == 1:
        print( "Define folder with plugins:" )
        print( "\tcheck_used_includes.py <PLUGINS_FOLDER>" )
        return -2
    
    result = 0
    for eachArg in sys.argv[1:]:
        if not scanPluginsFolders(eachArg):
            result = -1;

    if result == 0:
        print( "Everything looks good!" )
    else:
        print( "You need to fix mentioned issues!" )

    return result


if __name__ == '__main__':
    sys.exit(main())
