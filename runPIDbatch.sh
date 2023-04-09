#!/bin/bash

# 1st parameter e.g. = "/lustre/nyx/alice/users/maschmid/analysis/10d_e.pass2_merged/bhess_PID_Jets_Inclusive.root"
# 2nd parameter is charge mode: 0 = all, -1 = neg, +1 = pos
# 3rd parameter is TOF patching: 0 = off, 1 = on
# 4th parameter is MC: 0 = data, 1 = MC
# 5th parameter is the low entrality
# 6th parameter is the high centrality
# 7th parameter the prePID: 0 without, 1 including (1 is standard)
# 8th parameter is the jet mode: 0=pt, 1=z, 2=xi, 3=R, 4=jT
# 9th parameter is the low jetPt
# 10th parameter is the high jetPt
# 11th parameter is the list/directory name in file (if different to base of file name)

prePID=kTRUE
if [ "$7" = "0" ]; then
  prePID=kFALSE
fi 

mode=0 
modeString=$( tr '[:upper:]' '[:lower:]' <<<"$8" ) # Convert string to lowercase
if [ "$modeString" = "pt" ]; then
  mode=0
elif
  [ "$modeString" = "z" ]; then
  mode=1
elif  
  [ "$modeString" = "xi" ]; then
  mode=2
elif  
  [ "$modeString" = "r" ]; then
  mode=3
elif
  [ "$modeString" = "jt" ]; then
  mode=4
fi


lowJetPt=-1  
if [[ ${9} != "" ]]; then
  lowJetPt=${9}  
fi 

highJetPt=-1  
if [[ ${10} != "" ]]; then
  highJetPt=${10}    
fi 

listName=${11}

source .aliEnv

aliroot "runPIDiterative.C(\"$1\", 1.8, 0.15, 50., $4, 2, 0, $prePID, $4, $mode, $2, $5, $6, $lowJetPt, $highJetPt, 1, 1, \"$listName\", kTRUE, kFALSE, 1, 1.0, $3)" -b -q
