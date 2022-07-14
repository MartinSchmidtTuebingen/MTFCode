#!/usr/bin/python3

import sys
import os
import json
import subprocess
from shlex import split

config = {}
systematics = {}

def downloadData(remotePattern, targetpath):
    os.makedirs(targetpath, exist_ok = True)
    downloadCommand = "rsync -a --ignore-existing " + remotePattern + " " + targetpath + " --progress"
    subprocess.call(split(downloadCommand))

def main():
    global config
    global systematics
    if len(sys.argv) < 2:
        print('Provide name of analysis folder')
        exit()

    analysisFolder=sys.argv[1]
    systematicDay=sys.argv[2]
  
    print("Folder:" + analysisFolder)
    print("Day for systematics:" + systematicDay)
  
    #### Load config file ###
    with open(analysisFolder + "/config.json", "r") as configFile:
        config = json.loads(configFile.read())
        
    systematics = config["systematics"]
    config = config["config"]
    config["analysisFolder"] = analysisFolder + "/"
    
    #### Download data ###
    ### Reference data
    downloadData(config["remoteBasePath"] + config["referenceRemotePath"] + "*results*reg1*idSpectra*.root", config["analysisFolder"] + 'Data')
    
    ### Systematics
    for name,systematic in systematics.items():
        downloadData(config["remoteBasePath"] + systematic["sourcePath"], config["analysisFolder"] + systematic["savePath"])
    
    #subprocess.call(split("aliroot -l"))

if __name__ == "__main__":
    main()
