#!/bin/bash
folder=$(pwd)

if [[ $1 = "" || $1 == "-h" || $1 == "--help" ]];then
  echo "Help not yet implemented. See README"
elif [[ $1 = "init" ]];then
  echo "Initialize MTF environment. Password maybe needed to make env.sh executable"
  cp $folder/.env.sh.example $folder/env.sh
  chmod 755 $folder/env.sh
  echo "Done"
elif [[ $1 = "create" ]];then
  analysisName=$2
  analysisFolder=$folder/$analysisName
  if [ -d $sanalysisFolder ];then
    echo "Analysis already exists"
    exit
  fi
  echo "Create analysis $2"
  mkdir $analysisFolder
  cp $folder/env.sh $analysisFolder/env.sh.base
  echo "Created. Rename env.sh.base in $analysisFolder to env.sh and adjust loading environment, if this analysis should run with another AliPhysics instance."
else
  analysisName=$1
  analysisFolder=$folder/$analysisName
  
#   day=$2
#   if [[ $2 = "" ]];then
#     day=$(date +%Y_%m_%d)
#   fi
#   echo "Process systematic from $day"

  if [ -e $analysisFolder/env.sh ];then
    echo "Source analysis environment"
    source $analysisFolder/env.sh
  else
    echo "Source base environment"
    source $folder/env.sh
  fi

  python3 $folder/runMTFAnalysis.py $analysisFolder
fi
