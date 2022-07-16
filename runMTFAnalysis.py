#!/usr/bin/python3

import os
import json
import subprocess
from shlex import split
import argparse

from helperFunctions import *

# Add tasks to default if they should be done in the analysis run
defaultTasks='sys,eff,ue'

def downloadData(remotePattern, targetpath, excludePattern = ""):
    os.makedirs(targetpath, exist_ok = True)
    downloadCommand = "rsync -a --ignore-existing " + remotePattern + " " + targetpath + " --progress"
    if excludePattern != "":
      downloadCommand = downloadCommand + " --exclude=" + excludePattern
    subprocess.call(split(downloadCommand))

def main():
    parser = argparse.ArgumentParser(description='Script for MTF Analysis')
    
    parser.add_argument('-f','--folder', type=str, help='Analysis Folder', required=True)
    parser.add_argument('-s','--systematic', type=str, help='Day of systematics', required=False)
    parser.add_argument('-d','--download', type=int, help='Download data', required=False, default=0)
    parser.add_argument('-t','--tasks', type=str, help='Choose tasks to perform:Sys|Eff', required=False, default=defaultTasks) 
    parser.add_argument('-c','--continueRun', type=int, help='Continue after task to perform', required=False, default=0)
    
    args = parser.parse_args()
    
    analysisFolder = args.folder
    download = False if args.download == 0 else True
  
    print("Folder:" + analysisFolder)
    
    if download:
        print("Download data:")
  
    #### Load config file ###
    with open(analysisFolder + "/config.json", "r") as configFile:
        config = json.loads(configFile.read())
        
    systematics = config["systematics"]
    config = config["config"]
    config["analysisFolder"] = analysisFolder + "/"
    
    if download:
      #### Download data ###
      ### Reference data
      downloadData(config["remoteBasePath"] + config["referenceRemotePath"], config["analysisFolder"] + 'Data')
      
      ### Systematics
      for name,systematic in systematics.items():
          downloadData(config["remoteBasePath"] + systematic["sourcePath"], config["analysisFolder"] + systematic["savePath"], systematic["excludePattern"] if "excludePattern" in systematic else "")
          
      # Now we have to exit because rsync does not work with ali environment. This is caught in the call script runAnalysis.sh by calling this python script again
      exit()
      
    defaultTasksList = defaultTasks.split(',')
    tasksToPerform = args.tasks.lower().split(',')
    continueRun = False if args.continueRun == 0 else True 
    print(defaultTasks.split(',').index(tasksToPerform[0]))
    if len(tasksToPerform) == 1 and continueRun:
        tasksToPerform = defaultTasksList[defaultTasksList.index(tasksToPerform[0]):]
        
    print("Perform tasks: " + str(tasksToPerform))
    
    systematicDay = args.systematic
    
    if systematicDay != None:
      print("Day for systematics:" + systematicDay)
      
    #### Make result folders ###      
    for key,resFolder in foldersToCreate.items():
        os.makedirs(analysisFolder + resFolder, exist_ok = True)
    
    #### Run individual systematic Error Estimation.
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

if __name__ == "__main__":
    main()
