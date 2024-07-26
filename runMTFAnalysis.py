#!/usr/bin/python3

import os
import json
import subprocess
from shlex import split
import argparse
import pathlib

from helperFunctions import *

# Add tasks to default if they should be done in the analysis run
defaultTasks='sys,eff,ue'

def main():
    parser = argparse.ArgumentParser(description='Script for MTF Analysis')
    
    parser.add_argument('-f','--folder', type=str, help='Analysis Folder', required=True)
    parser.add_argument('-s','--systematic', type=str, help='Day of systematics', required=False)
    parser.add_argument('-d','--download', type=int, help='Download data', required=False, default=0)
    parser.add_argument('-g','--gridDownload', type=int, help='Download train data from grid', required=False, default=0)
    parser.add_argument('-p','--doPIDAnalysis', type=int, help='Upload data to farm and start analysis', required=False, default=0)
    parser.add_argument('-t','--tasks', type=str, help='Choose tasks to perform:Sys|Eff', required=False, default=defaultTasks) 
    parser.add_argument('-c','--continueRun', type=int, help='Continue after task to perform', required=False, default=0)
    
    args = parser.parse_args()
    
    analysisFolder = args.folder
    analysisFolder = str(pathlib.Path(analysisFolder).resolve()) + '/'
    print("Folder:" + analysisFolder)
  
    download = args.download != 0
    gridDownload = args.gridDownload != 0
    doPIDAnalysis = args.doPIDAnalysis != 0
    
    configFileName = analysisFolder + "config.json"
    
    #### Load config file ###
    with open(configFileName, "r") as configFile:
        completeConfig = json.loads(configFile.read())
        
    active_systematics = {}
    for key,entry in completeConfig["systematics"].items():
        if type(entry) is not dict or entry["isActive"]:
            active_systematics[key] = entry

    fastSimulationConfig = completeConfig["fastSimulation"]
    config = completeConfig["config"]
    config["analysisFolder"] = analysisFolder

    config['centString'] = ";".join(config["centralities"]).replace("_",";")
    config['jetPtString'] = ";".join(config["jetPts"]).replace("_",";")
    config['modesInclusiveString'] = ";".join(config["modesInclusive"])
    config['modesJetsString'] = ";".join(["Jt" if mode == "jT" else mode for mode in config["modesJets"]])
    
    #### Make result folders ###      
    for key,resFolder in foldersToCreate.items():
        os.makedirs(config["analysisFolder"] + resFolder, exist_ok = True)
        
    for mcPathName in ["mcPath", "pathMCsysErrors"]:
        mcPath = config[mcPathName]
        if not mcPath.startswith('/'):
            mcPath = config["analysisFolder"] + '/' + mcPath
            config[mcPathName] = mcPath
        os.makedirs(mcPath, exist_ok = True)
        
    if gridDownload:
        trainFilesDir = config["analysisFolder"] + "trainFiles"
        os.makedirs(trainFilesDir, exist_ok = True)
        downloadedFiles = set(())
        #### Download data from analysis trains ###
        for pidAnalysisInformation in config["pidAnalysesList"]:
            if not pidAnalysisInformation["isActive"]:
                continue
            
            trainName = pidAnalysisInformation["trainName"]
            modTrainName = trainName.replace("/alice/cern.ch/user/a/alitrain/PWGJE/","").replace("/merge","").replace("/","_")
            print(modTrainName)
            saveFileName = f"{trainFilesDir}/AnalysisResults_{modTrainName}.root"
            if modTrainName not in downloadedFiles:
                downloadedFiles.add(modTrainName)
                callScript(f"downloadTrainData.sh {trainName}/AnalysisResults.root {saveFileName}")
                
            dirToExtract = pidAnalysisInformation["dirFileName"]
            rootArguments = {
                "inputFile":saveFileName,
                "dirToExtract": dirToExtract
            }
            pidFileName=trainFilesDir + "/" + dirToExtract + ".root"
            
            callRootMacro("extractDirFromFile", rootArguments)
                
        print("Now start script in new shell with -p as argument for the analysis")        
        exit()
        
    if doPIDAnalysis:
        trainFilesDir = config["analysisFolder"] + "trainFiles"
        for pidAnalysisInformation in config["pidAnalysesList"]:
            if not pidAnalysisInformation["isActive"]:
                continue
            
            dirToExtract = pidAnalysisInformation["dirFileName"]
            pidFileName = trainFilesDir + "/" + dirToExtract + ".root"
            
            remoteHost = config["remoteHost"]
            remoteBasePath = config["remoteBasePath"]
            analysisDirName = pidAnalysisInformation["analysisDirName"]
            jobIdentifier = pidAnalysisInformation["jobIdentifier"]
            
            centralities = pidAnalysisInformation.get("centralities", config["centralities"])
            
            centStringWithUnderscore = ";".join(centralities)
            jetString = "-1_-1"
            modeString = "pt"
            if pidAnalysisInformation["isJet"]:
                jetString = ";".join(config["jetPts"])
                modeString = ";".join(config["modesJets"])

            prePIDMode = "1"
            if "prePIDMode" in pidAnalysisInformation:
                prePIDMode = pidAnalysisInformation["prePIDMode"]
            
            callScript(f"steerPIDAnalysis.sh {remoteHost} {remoteBasePath} {analysisDirName} {pidFileName} {jobIdentifier} {centStringWithUnderscore} {jetString} {modeString} {prePIDMode}")
            
        exit()
        
    if download:
        #### Download data ###
        ### Reference data
        print("Download data:")
        for referencePath in config["referenceRemotePath"]:
            downloadData(config["remoteHost"] + "://" + config["remoteBasePath"] + referencePath, config["analysisFolder"] + 'Data')
        
        ## Systematics
        for name,systematic in active_systematics.items():
            try:
                sourcePath = systematic["sourcePath"]
                savePath = systematic["savePath"]
            
                if not systematic.get("isActive",True):
                    continue
                
                downloadData(config["remoteHost"] + "://" + config["remoteBasePath"] + sourcePath, config["analysisFolder"] + savePath, systematic.get("excludePattern", ""))
            except (TypeError, AttributeError):
                continue
            
        # Now we have to exit because rsync does not work with ali environment. This is caught in the call script runAnalysis.sh by calling this python script again
        exit()
      
    defaultTasksList = defaultTasks.split(',')
    tasksToPerform = args.tasks.lower().split(',')
    continueRun = args.continueRun != 0
    if len(tasksToPerform) == 1 and continueRun:
        tasksToPerform = defaultTasksList[defaultTasksList.index(tasksToPerform[0]):]
        
    print("Perform tasks: " + str(tasksToPerform))
    
    if 'createsys' in tasksToPerform:
        systematicValues = produceSystematicValues(active_systematics['systematicFile'])
        decision = input("Update config.json? Y/N")
        if decision.lower() == "y":
            updateSystematicValuesInConfig(systematicValues, configFileName)
            print("Please update axes labelling manually")
        exit()
        
    if 'fitsim' in tasksToPerform:
        fitFastSimulationFactors(config, fastSimulationConfig)
        exit()
        
    if 'mcjetsys' in tasksToPerform:
        createCorrectionFactorsFullMC(config)
        writeCorrectionFiles(config, fastSimulationConfig)
        calculateJetMCCorrectionSysErrors(config, fastSimulationConfig)
        exit()
    
    systematicDay = args.systematic
    
    if systematicDay != None:
      print("Day for systematics:" + systematicDay)
    
    #### Run error estimation, add up error estimation and calculate efficiency ###
    individualAnalyses = {
        "Inclusive":"Jets_Inclusive",
        "Jets":"Jets",
        "UE":"Jets_UE"
    }
    for name,jetString in individualAnalyses.items():
        if config['do'+name] == 1:
            print('Do ' + name + ' analysis')
            if 'sys' in tasksToPerform:
                runSystematicProcess(config['systematics' + name], active_systematics, config, jetString, systematicDay)
            if 'eff' in tasksToPerform:
                runCalculateEfficiency(jetString, config, systematicDay, systematicDay)
                
    #### Subtract Underlying event ###
    if 'ue' in tasksToPerform:
        subtractUnderlyingEvent(config, systematicDay)
        
        
        
def downloadData(remotePattern, targetpath, excludePattern = ""):
    os.makedirs(targetpath, exist_ok = True)
    downloadCommand = "rsync -a --ignore-existing " + remotePattern + " " + targetpath + " --progress"
    print(downloadCommand)
    if excludePattern != "":
      downloadCommand = downloadCommand + " --exclude=" + excludePattern
    subprocess.call(split(downloadCommand))
        

if __name__ == "__main__":
    main()
