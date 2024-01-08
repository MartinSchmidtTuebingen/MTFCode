#!/bin/bash
folder=$(pwd)

if [[ $1 = "" || $1 == "-h" || $1 == "--help" ]];then
  echo "Help not yet implemented. See README"
elif [[ $1 = "init" ]];then
  echo "Initialize MTF environment. Password maybe needed to make env.sh executable"
  cp $folder/.env.sh.example $folder/env.sh
  chmod 755 $folder/env.sh
  echo "Done"
  exit
elif [[ $1 = "create" ]];then
  analysisName=$2
  analysisFolder=$folder/$analysisName
  if [ -d "$sanalysisFolder" ];then
    echo "Analysis already exists"
    exit
  fi
  echo "Create analysis $2"
  mkdir $analysisFolder
  cp $folder/env.sh $analysisFolder/env.sh.base
  cp $folder/config.json.example $analysisFolder/config.json
  echo "Created. Rename env.sh.base in $analysisFolder to env.sh and adjust loading environment, if this analysis should run with another AliPhysics instance."
  exit
fi
  
analysisName=$1
analysisFolder=$folder/$analysisName

shift;

download=0
gridDownload=0
doPIDAnalysis=0
day=$(date +%Y_%m_%d)
tasks=""
continueParameter=""
helpParameter=""

while getopts "hdgps:t:c:" option; do
  case $option in
    h) helpParameter="-h";;
    d) download=1;;
    g) gridDownload=1;;
    p) doPIDAnalysis=1;;
    s) day=$OPTARG;;
    t) tasks="-t $OPTARG";;
    c) continueParameter="-c $OPTARG"
  esac
done

if [[ $gridDownload != "0" ]];then
  echo "Download grid data"
  source $folder/env.sh
  python3 $folder/runMTFAnalysis.py -f $analysisFolder -g 1
  exit
fi

if [[ $doPIDAnalysis != "0" ]];then
  echo "Do PID Analysis"
  python3 $folder/runMTFAnalysis.py -f $analysisFolder -p 1
  exit
fi

if [[ $download != "0" ]];then
  echo "Sync data"
  python3 $folder/runMTFAnalysis.py -f $analysisFolder -d 1
  exit
fi

if [ -e $analysisFolder/env.sh ];then
  echo "Source analysis environment"
  source $analysisFolder/env.sh
else
  echo "Source base environment"
  source $folder/env.sh
fi
  
python3 $folder/runMTFAnalysis.py -f $analysisFolder -s $day $tasks $continueParameter $helpParameter
