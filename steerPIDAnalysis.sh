#!/bin/bash

remoteHost=$1
remoteBasePath=$2
analysisDir=$3
analysisFile=$4
identifier=$5
centString=$6
jetString=$7
modeString=$8

ssh $remoteHost mkdir -p $remoteBasePath/$analysisDir
scp runPIDAnalysis.sh $remoteHost://$remoteBasePath
scp $analysisFile $remoteHost://$remoteBasePath/$analysisDir
ssh $remoteHost $remoteBasePath/runPIDAnalysis.sh $analysisDir $(basename $analysisFile) $identifier \"$centString\" \"$jetString\" \"$modeString\"
