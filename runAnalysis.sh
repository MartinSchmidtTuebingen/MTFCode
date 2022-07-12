#!/bin/bash
### ATTENTION: Script has to be executed with source! Otherwise loading the environment does not work as intended ###

folder=$(pwd)

if [[ $1 = "" ]];then
  echo "Help not yet implemented"
elif [[ $1 = "init" ]];then
  echo "Initialize MTF environment. Password maybe needed to make env.sh executable"
  cp $folder/.env.sh.example $folder/env.sh
  chmod 755 $folder/env.sh
  echo "Done"
elif [[ $1 = "create" ]];then
  echo "Create analysis $2"
  analysisName=$2
  mkdir $folder/$analysisName
  cp $folder/env.sh $folder/$analysisName/env.sh
  echo "Created"
else
  analysisName=$1
  analysisFolder=$folder/$analysisName
  day=$2
  if [[ $2 = "" ]];then
    day=$(date +%Y_%m_%d)
  fi
  echo "Process systematic from $day"

  source $analysisFolder/env.sh

  python3 $folder/runMTFAnalysis $analysisFolder $day
fi
