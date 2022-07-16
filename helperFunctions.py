import pathlib
import subprocess
from shlex import split

obsNameToNumber = {
  'pt' : 0,
  'z'  : 1,
  'xi' : 2,
  'r'  : 3,
  'jt' : 4
}

foldersToCreate = {
  'single':'SingleSystematicResults/',
  'summed':'SummedSystematics/',
  'eff':'Efficiencycorrected/',
  'uesub':'UEsubtractedJetResults/'
}

def callRootMacro(name, arguments):
  folderForMacros=str(pathlib.Path().resolve()) + '/RootMacros/'
  cmd = "aliroot '" + folderForMacros + name + ".C+("

  argStrings = list(())
  
  for argName, argValue in arguments.items():
    if type(argValue) == str and argValue not in ('kFALSE','kTRUE'):
      argStrings.append("\"" + argValue + "\"")
    else:
      argStrings.append(str(argValue))
    
  
  cmd += ", ".join(argStrings) + ")' -l -q -b" 

  subprocess.call(split(cmd))

def runSystematicProcess(systematicsToProcess, systematics, config, jetString, systematicDay):
  analysisFolder = config['analysisFolder']
  systematicsPropertiesToProcess = dict(filter(lambda systematic : systematic[0] in systematicsToProcess, systematics.items()))
  modeString =  config['modesInclusiveString'] if jetString.find('Inclusive') != -1 else config['modesJetsString']
  
  systematicInfoString = getSystematicInfoString(systematicsPropertiesToProcess, analysisFolder)
  
  arguments = {'jetString': jetString, 'chargeString': config['chargeString'], 'referencepath': analysisFolder + '/Data', 'outfilepath': analysisFolder + foldersToCreate['single'], 'referenceFileSchema': config['fileNamePattern'], 'stringForSystematicInformation': systematicInfoString, 'centStepsString' : config['centString'], 'jetPtStepsString' : config['jetPtString'], 'modesInputString' : modeString, 'nSigma': config['nSigma']}
    
  callRootMacro("runSystematicErrorEstimation", arguments)  
  
  outputFilePatternList = list(())
  for sysName,sysVariables in systematicsPropertiesToProcess.items():
    outputFilePatternList.append(analysisFolder + foldersToCreate['single'] + sysVariables['outputFilePattern'])
    
  outPutFiles = "|".join(outputFilePatternList)
    
  arguments = {'jetString': jetString, 'chargeString': config['chargeString'], 'date': systematicDay, 'referenceFile': analysisFolder + '/Data/' + config['fileNamePattern'], 'centStepsString' : config['centString'], 'jetPtStepsString' : config['jetPtString'], 'modesInputString' : modeString, 'outPutFilesToAddUp': outPutFiles, 'outPath': analysisFolder + foldersToCreate['summed'], 'nSigma': config['nSigma']}
  
  callRootMacro("runAddUpSystematicErrors", arguments)
  
def getSystematicInfoString(systematics, analysisFolder):
  sysInfoList = list(())
  for sysName,sysVariables in systematics.items():
    inputPath = sysVariables['savePath']
    if not inputPath.startswith('/'):
      inputPath = analysisFolder + '/' + inputPath
    singleInfoString = sysVariables['outputFilePattern'] + '$' + sysVariables['refHistTitle'] + '$' + inputPath
    for inputVariables in sysVariables["inputs"]:
      singleInfoString += '§' + inputVariables["pattern"] + '$' + inputVariables["histTitle"]
    
    sysInfoList.append(singleInfoString)
  
  sysInfoString = '|'.join(sysInfoList)
  return sysInfoString
  
def runCalculateEfficiency(jetString, config, systematicDay, summedDay):
  isJetAnalysis = jetString.find("Jets") != -1 and jetString.find("Inclusive") == -1
  obsValues = config['modesJets'] if isJetAnalysis else config['modesInclusive']
  effFile = config['analysisFolder'] + "/Data/MCData/" + config['efficiencyFileNamePattern'].format(jetString)
  
  # file name pattern for summed systematic errors
  fileNamePattern="outputSystematicsTotal_SummedSystematicErrors_" + jetString + "_{0}___centrality{1}_{2}_{3}_" + systematicDay + "__" + summedDay + ".root"
  jetPtStringPattern = "jetPt{0}_{1}_" if isJetAnalysis else ""
  
  for cent in config['centralities']:
    centList = cent.split("_")
    lowerCent = int(centList[0])
    upperCent = int(centList[1])
  
    for jetPt in config['jetPts']:
      ptList = jetPt.split("_")
      lowJetPt = float(ptList[0]) if isJetAnalysis else -1
      upperJetPt = float(ptList[1]) if isJetAnalysis else -1
      jetPtString = jetPtStringPattern.format(lowJetPt, upperJetPt)
      for obs in obsValues:
        arguments = {
          'effFile': effFile,
          'pathNameData': config['analysisFolder'] + foldersToCreate['summed'] + fileNamePattern.format(obs,lowerCent, upperCent, jetPtString),
          'pathMCsysErrors': config['pathMCsysErrors'],
          'savePath': config['analysisFolder'] + foldersToCreate['eff'],
          'correctGeantFluka': config['correctGeantFluka'],
          'newGeantFluka': config['newGeantFluka'],
          'scaleStrangeness': config['scaleStrangeness'],
          'applyMuonCorrection': config['applyMuonCorrection'],
          'chargeMode': config['charge'],
          'lowerCentData': lowerCent,
          'upperCentData': upperCent,
          'lowerCent': lowerCent,
          'upperCent': upperCent,
          'lowerJetPt': lowJetPt,
          'upperJetPt': upperJetPt,
          'iObs': obsNameToNumber[obs.lower()],
          'constCorrAboveThreshold': config['constCorrAboveThreshold'],
          'rebinEffObs': config['rebinEffObs'],
          'etaAbsCut': config['etaAbsCut'],
          'eps_trigger': config['eps_trigger'],
          'sysErrTypeMC': config['sysErrTypeMC'],
          'normaliseToNInel': 'kFALSE' if isJetAnalysis else 'kTRUE',
          'pathMCUEFile': config['pathMCUEFile'] if jetString.find("UE") != -1 else ''
        }
        
        callRootMacro('calcEfficiency', arguments)
        
      if not isJetAnalysis:
        break
