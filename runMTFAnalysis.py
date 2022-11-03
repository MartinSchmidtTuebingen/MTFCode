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
    parser.add_argument('-t','--tasks', type=str, help='Choose tasks to perform:Sys|Eff', required=False, default=defaultTasks) 
    parser.add_argument('-c','--continueRun', type=int, help='Continue after task to perform', required=False, default=0)
    
    args = parser.parse_args()
    
    analysisFolder = args.folder
    analysisFolder = str(pathlib.Path(analysisFolder).resolve()) + '/'
    print("Folder:" + analysisFolder)
  
    download = args.download != 0
    
    configFile = analysisFolder + "config.json"
    
    #### Load config file ###
    with open(configFile, "r") as configFile:
        completeConfig = json.loads(configFile.read())
        
    systematics = completeConfig["systematics"]
    fastSimulationConfig = completeConfig["fastSimulation"]
    config = completeConfig["config"]
    config["analysisFolder"] = analysisFolder
    
    #### Make result folders ###      
    for key,resFolder in foldersToCreate.items():
        os.makedirs(config["analysisFolder"] + resFolder, exist_ok = True)
        
    for mcPathName in ["mcPath", "pathMCsysErrors"]:
        mcPath = config[mcPathName]
        if not mcPath.startswith('/'):
            mcPath = config["analysisFolder"] + '/' + mcPath
            config[mcPathName] = mcPath
        os.makedirs(mcPath, exist_ok = True)
        
    if download:
      #### Download data ###
      ### Reference data
      print("Download data:")
      downloadData(config["remoteBasePath"] + config["referenceRemotePath"], config["analysisFolder"] + 'Data')
      
      ### Systematics
      for name,systematic in systematics.items():
          downloadData(config["remoteBasePath"] + systematic["sourcePath"], config["analysisFolder"] + systematic["savePath"], systematic["excludePattern"] if "excludePattern" in systematic else "")
          
      # Now we have to exit because rsync does not work with ali environment. This is caught in the call script runAnalysis.sh by calling this python script again
      exit()
      
    defaultTasksList = defaultTasks.split(',')
    tasksToPerform = args.tasks.lower().split(',')
    continueRun = args.continueRun != 0
    if len(tasksToPerform) == 1 and continueRun:
        tasksToPerform = defaultTasksList[defaultTasksList.index(tasksToPerform[0]):]
        
    print("Perform tasks: " + str(tasksToPerform))
    
    if 'createsys' in tasksToPerform:
        systematicValues = produceSystematicValues(systematics['systematicFile'])
        decision = input("Update config.json? Y/N")
        if decision == "Y":
            updateSystematicValuesInConfig(systematicValues, completeConfig, configFile)
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
                runSystematicProcess(config['systematics' + name], systematics, config, jetString, systematicDay) 
            if 'eff' in tasksToPerform:
                runCalculateEfficiency(jetString, config, systematicDay, systematicDay)
                
    #### Subtract Underlying event ###
    if 'ue' in tasksToPerform:
        subtractUnderlyingEvent(config, systematicDay)
        
        
        
def downloadData(remotePattern, targetpath, excludePattern = ""):
    os.makedirs(targetpath, exist_ok = True)
    downloadCommand = "rsync -a --ignore-existing " + remotePattern + " " + targetpath + " --progress"
    if excludePattern != "":
      downloadCommand = downloadCommand + " --exclude=" + excludePattern
    subprocess.call(split(downloadCommand))
        

if __name__ == "__main__":
    main()
