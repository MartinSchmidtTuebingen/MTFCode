#!/usr/bin/python3

import sys
import os
import json
import subprocess
from shlex import split

def main():
    if len(sys.argv) < 2:
        print('Provide name of analysis folder')
        exit()

    analysisFolder=sys.argv[1]
    systematicDay=sys.argv[2]
  
    print("Folder:" + analysisFolder)
    print("Day for systematics:" + systematicDay)
  
    #### Load config file ###
    config = {}
    with open(analysisFolder + "/config.json", "r") as configFile:
        config = json.loads(configFile.read())
        
    systematics = config["systematics"]
    config = config["config"]
    print(config)
  
    #subprocess.call(split("aliroot -l"))

if __name__ == "__main__":
    main()
