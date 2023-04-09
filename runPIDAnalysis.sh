#!/bin/bash
dirName=$1
fileName=$2
identifier=$3

centString=$4
IFS=";" read -a centArray <<< $centString

jetString=$5
IFS=";" read -a jetArray <<< $jetString

modeString=$6
IFS=";" read -a modeArray <<< $modeString

submitCMD="sbatch --time=03:59:59 --mem=2048"

currentdir=$(dirname $0)

workdir=$(dirname $currentdir)
dir=$currentdir/$dirName

PIDscript=runPIDbatch.sh

for centrality in "${centArray[@]}" #ATTENTION: Centrality counts as 2 parameters!
do 
  IFS="_" read -a centralityArray <<< $centrality
  lowCent=${centralityArray[0]}
  highCent=${centralityArray[1]}
  
  for jetPt in "${jetArray[@]}"
  do
    IFS="_" read -a jetPtArray <<< $jetPt
    lowJetPt=${jetPtArray[0]}
    highJetPt=${jetPtArray[1]}
    
    for mode in "${modeArray[@]}"
    do
      logname=$identifier""_TOF0_Centrality_$lowCent""_$highCent
      if [ "$lowJetPt" != "-1" ];then
        logname=$logname"_JetPt_"$lowJetPt"_"$highJetPt"_Mode_"$mode
      fi 
      
      $submitCMD -D $workdir -J $(whoami)_MTFAnalysis_$identifier""_TOF0_Centrality_$lowCent""_$highCent -o $dir/$logname.out -e $dir/$logname""_error.out $workdir/$PIDscript $dir/$fileName 0 0 0 $lowCent $highCent 1 $mode $lowJetPt $highJetPt
      sleep 30
    done
  done
done 

# mkdir - p $prefix"_PureGauss"
# for centrality in "${centArray[@]}" #ATTENTION: Centrality counts as 2 parameters!
# do 
#   ./runPIDbatch_Automatic_sbatch.sh 0 0 $centrality inclusive inclusive_pg $fileprefix""Jets_Inclusive_PureGauss.root
#   ./runPIDbatch_Automatic_sbatch.sh 0 0 $centrality jets jets_cg $fileprefix""Jets_PureGauss.root
# done 

# 
# mkdir $prefix"_ReferenceAndPrePID"
# 
# cp ./runPIDbatch_Automatic_sbatch.sh ./$prefix"_ReferenceAndPrePID"
# cd ./$prefix"_ReferenceAndPrePID"
# for centrality in "0 100" "0 10" "60 100" #ATTENTION: Centrality counts as 2 parameters!
# do 
#   ./runPIDbatch_Automatic_sbatch.sh 0 0 $centrality inclusive inclusive_ref $fileprefix""Jets_Inclusive$reference.root
#   ./runPIDbatch_Automatic_sbatch.sh 0 0 $centrality jets jets_reference $fileprefix""Jets$reference.root
#   ./runPIDbatch_Automatic_sbatch.sh 0 0 $centrality jets jets_UE_reference $fileprefix""Jets_UE$refUEMethod$reference.root
#   ./runPIDbatch_Automatic_sbatch.sh 0 0 $centrality inclusive inclusive_sys_prePID $fileprefix""Jets_Inclusive$reference.root PrePID
#   ./runPIDbatch_Automatic_sbatch.sh 0 0 $centrality jets jets_sys_prePID $fileprefix""Jets$reference.root PrePID
# done 
# cd ..
# 
# mkdir $prefix"_Systematics_ShapeAndUEMethod"
# cp ./runPIDbatch_Automatic_sbatch.sh ./$prefix"_Systematics_ShapeAndUEMethod"
# cd ./$prefix"_Systematics_ShapeAndUEMethod"
# for centrality in "0 100" "0 10" "60 100" 
# do
#   ./runPIDbatch_Automatic_sbatch.sh 0 0 $centrality inclusive inclusive_sys_shape $fileprefix""Jets_Inclusive$sysshape.root
#   ./runPIDbatch_Automatic_sbatch.sh 0 0 $centrality jets jets_sys_shape $fileprefix""Jets$sysshape.root
#   ./runPIDbatch_Automatic_sbatch.sh 0 0 $centrality jets jets_UE_sys_Method $fileprefix""Jets_UE$sysUEMethod$reference.root
# done 
# cd ..
# 
# for systematic in Splines Eta Sigma 
# do
#   mkdir ./$prefix"_Systematics_$systematic"
#   cp ./runPIDbatch_Automatic_sbatch.sh ./$prefix"_Systematics_$systematic"
#   cd ./$prefix"_Systematics_$systematic"
#   for centrality in "0 100" "0 10" "60 100"
#     do
#       for direction in Up Down
#       do
#         ./runPIDbatch_Automatic_sbatch.sh 0 0 $centrality inclusive inclusive_sys_$systematic$direction $fileprefix""Jets_Inclusive$reference""_Systematics$systematic$direction.root
#         ./runPIDbatch_Automatic_sbatch.sh 0 0 $centrality jets jets_sys_$systematic$direction $fileprefix""Jets$reference""_Systematics$systematic$direction.root
#       done
#     done
#   cd ..
# done
