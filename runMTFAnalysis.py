#!/usr/bin/python3

import os
import json
import subprocess
from shlex import split
import argparse

config = {}
systematics = {}

def downloadData(remotePattern, targetpath):
    os.makedirs(targetpath, exist_ok = True)
    downloadCommand = "rsync -a --ignore-existing " + remotePattern + " " + targetpath + " --progress"
    subprocess.call(split(downloadCommand))

def main():
    global config
    global systematics
        
    parser = argparse.ArgumentParser(description='Script for MTF Analysis')
    
    parser.add_argument('-f','--folder', type=str, help='Analysis Folder', required=True)
    parser.add_argument('-s','--systematic', type=str, help='Day of systematics',required=False)
    parser.add_argument('-d','--download', type=int, help='Download data',required=False)
    
    args = parser.parse_args()

    analysisFolder = args.folder
    systematicDay = args.systematic
    download = False if args.download == 0 or args.download is None else True
  
    print("Folder:" + analysisFolder)
    if systematicDay != None:
      print("Day for systematics:" + systematicDay)
    
    print("Download data:" + str(download))
  
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
          downloadData(config["remoteBasePath"] + systematic["sourcePath"], config["analysisFolder"] + systematic["savePath"])
          
      # Now we have to exit because rsync does not work with ali environment. This is caught in the call script runAnalysis.sh by calling this python script again
      exit()
    
    subprocess.call(split("aliroot -l"))

if __name__ == "__main__":
    main()
