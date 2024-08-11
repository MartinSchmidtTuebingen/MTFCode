#!/bin/bash

remoteHost=$1
remoteBasePath=$2
analysisDir=$3
analysisFile=$4
identifier=$5
centString=$6
jetString=$7
modeString=$8
prePIDMode=$9

analysisFileName=$(basename $analysisFile)

ssh $remoteHost mkdir -p $remoteBasePath/$analysisDir
if ! ssh -q $remoteHost "test -e $remoteBasePath/$analysisDir/$analysisFileName";then
    echo "Analysis file does not exist, uploading..."
    scp $analysisFile $remoteHost://$remoteBasePath/$analysisDir
else
    echo "Analysis file $analysisFile already exists in remote, using existing file"
fi

ssh $remoteHost $remoteBasePath/runPIDAnalysis.sh $analysisDir $(basename $analysisFile) $identifier \"$centString\" \"$jetString\" \"$modeString\" \"$prePIDMode\"
