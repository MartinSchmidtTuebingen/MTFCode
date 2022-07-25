import pathlib
import subprocess
from shlex import split

obsNameToNumber = {
    'pt': 0,
    'z': 1,
    'xi': 2,
    'r': 3,
    'jt': 4
}

foldersToCreate = {
    'single': 'SingleSystematicResults/',
    'summed': 'SummedSystematics/',
    'eff': 'Efficiencycorrected/',
    'uesub': 'UEsubtractedJetResults/'
}


def callRootMacro(name, arguments):
    folderForMacros = str(pathlib.Path().resolve()) + '/RootMacros/'
    cmd = "aliroot '" + folderForMacros + name + ".C+("

    argStrings = list(())

    for argName, argValue in arguments.items():
        if type(argValue) == str and argValue not in ('kFALSE', 'kTRUE'):
            argStrings.append("\"" + argValue + "\"")
        else:
            argStrings.append(str(argValue))

    cmd += ", ".join(argStrings) + ")' -l -q -b"

    subprocess.call(split(cmd))


def runSystematicProcess(systematicsToProcess, systematics, config, jetString, systematicDay):
    analysisFolder = config['analysisFolder']
    systematicsPropertiesToProcess = dict(filter(
        lambda systematic: systematic[0] in systematicsToProcess, systematics.items()))
    modeString = config['modesInclusiveString'] if jetString.find(
        'Inclusive') != -1 else config['modesJetsString']

    systematicInfoString = getSystematicInfoString(
        systematicsPropertiesToProcess, analysisFolder)

    arguments = {'jetString': jetString, 'chargeString': config['chargeString'], 'referencepath': analysisFolder + '/Data', 'outfilepath': analysisFolder + foldersToCreate['single'], 'referenceFileSchema': config['fileNamePattern'],
                 'stringForSystematicInformation': systematicInfoString, 'centStepsString': config['centString'], 'jetPtStepsString': config['jetPtString'], 'modesInputString': modeString, 'nSigma': config['nSigma']}

    callRootMacro("runSystematicErrorEstimation", arguments)

    outputFilePatternList = list(())
    for sysName, sysVariables in systematicsPropertiesToProcess.items():
        outputFilePatternList.append(
            analysisFolder + foldersToCreate['single'] + sysVariables['outputFilePattern'])

    outPutFiles = "|".join(outputFilePatternList)

    arguments = {'jetString': jetString, 'chargeString': config['chargeString'], 'date': systematicDay, 'referenceFile': analysisFolder + '/Data/' + config['fileNamePattern'], 'centStepsString': config['centString'],
                 'jetPtStepsString': config['jetPtString'], 'modesInputString': modeString, 'outPutFilesToAddUp': outPutFiles, 'outPath': analysisFolder + foldersToCreate['summed'], 'nSigma': config['nSigma']}

    callRootMacro("runAddUpSystematicErrors", arguments)


def getSystematicInfoString(systematics, analysisFolder):
    sysInfoList = list(())
    for sysName, sysVariables in systematics.items():
        inputPath = sysVariables['savePath']
        if not inputPath.startswith('/'):
            inputPath = analysisFolder + '/' + inputPath
        singleInfoString = sysVariables['outputFilePattern'] + \
            '$' + sysVariables['refHistTitle'] + '$' + inputPath
        for inputVariables in sysVariables["inputs"]:
            singleInfoString += '§' + \
                inputVariables["pattern"] + '$' + inputVariables["histTitle"]

        sysInfoList.append(singleInfoString)

    sysInfoString = '|'.join(sysInfoList)
    return sysInfoString


def runCalculateEfficiency(jetString, config, systematicDay, summedDay):
    isJetAnalysis = jetString.find(
        "Jets") != -1 and jetString.find("Inclusive") == -1
    obsValues = config['modesJets'] if isJetAnalysis else config['modesInclusive']
    effFile = config['mcPath'] + config['efficiencyFileNamePattern'].format(jetString)

    # file name pattern for summed systematic errors
    fileNamePattern = "outputSystematicsTotal_SummedSystematicErrors_" + jetString + "_{0}___centrality{1}_{2}_{3}_" + \
        systematicDay + "__" + summedDay + ".root"
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
                    'pathNameData': config['analysisFolder'] + foldersToCreate['summed'] + fileNamePattern.format(obs, lowerCent, upperCent, jetPtString),
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


def subtractUnderlyingEvent(config, systematicDay):
    arguments = {
        'jetFilePattern': config['analysisFolder'] + foldersToCreate['eff'] + "output_EfficiencyCorrection_outputSystematicsTotal_SummedSystematicErrors_Jets_%s__%s%s__" + systematicDay + "__" + systematicDay + ".root",
        'ueFilePattern': config['analysisFolder'] + foldersToCreate['eff'] + "output_EfficiencyCorrection_outputSystematicsTotal_SummedSystematicErrors_Jets_UE_%s__%s%s__" + systematicDay + "__" + systematicDay + ".root",
        'jetPtStepsString': config['jetPtString'],
        'centStepsString': config['centString'],
        'modesInputString': config['modesJetsString'],
        'outputFilePattern': config['analysisFolder'] + foldersToCreate['uesub'] + "output_EfficiencyCorrection_outputSystematicsTotal_SummedSystematicErrors_Jets_%s__%s%s__" + systematicDay + "__" + systematicDay + "_UEsubtractedJetResults.root",
    }

    callRootMacro("SubtractUnderlyingEvent", arguments)


def fitFastSimulationFactors(config, fastSimulationConfig):
    #### Produce fast simulation parameters ###
    parameters = fastSimulationConfig['parameters']
    parameterList = list(())
            
    for spec,params in parameters.items():
        for charge, fitparams in params.items():
            parameterList.append('-'.join(fitparams))

    parametersString = ';'.join(parameterList)
    arguments = {
        'effFile': config['mcPath'] + config['efficiencyFileNamePattern'].format('Jets_Inclusive'),
        'outputfile': config['mcPath'] + "fastSimulationParameters" + "_" + config['MCRunName'] + ".root",
        'parameters': parametersString
    }
    callRootMacro("FitFastSimulationFactors", arguments)

def createCorrectionFactorsFullMC(config):
    #### Produce Corrections factors of full MC run
    arguments = {
        'pathNameEfficiency': config['mcPath'] + config['efficiencyFileNamePattern'].format('Jets'),
        'outfileName': config['mcPath'],
    }
    # TODO: Give observables, jetPts as argument, check if it can be merged with writeOutCorrectionFiles
    callRootMacro("createFileForBbBCorrections", arguments)

def writeCorrectionFiles(config):
    ### Write correction files ###
    for eff in ["095", "100", "105"]:
        varFolder = "Eff" + eff + "_Res100"
        arguments = {
            'effFile': config['mcPath'] + varFolder + '/' +  config['efficiencyFileNamePattern'].format('Jets'),
            'outFile': config['mcPath'] + "/outCorrections_FastJetSimulation_" + varFolder + ".root"
        }
        # TODO: Give observables, jetPts as argument (also below). Check if this triple call can be simplified
        callRootMacro("writeOutCorrectionFiles", arguments)

    for res in ["080", "120"]:
        varFolder = "Eff100" + "_Res" + res
        arguments = {
            'effFile': config['mcPath'] + varFolder + '/' +  config['efficiencyFileNamePattern'].format('Jets'),
            'outFile': config['mcPath'] + "/outCorrections_FastJetSimulation_" + varFolder + ".root"
        }
        callRootMacro("writeOutCorrectionFiles", arguments)

    for varFolder in ["LowPtDepletion", "LowPtEnhancement"]:
        arguments = {
            'effFile': config['mcPath'] + varFolder + '/' +  config['efficiencyFileNamePattern'].format('Jets'),
            'outFile': config['mcPath'] + "/outCorrections_FastJetSimulation_" + varFolder + ".root"
        }
        callRootMacro("writeOutCorrectionFiles", arguments)

def calculateJetMCCorrectionSysErrors(config):
    #### Produce systematic errors for jet mc correction ###
    arguments = {
        'effFilePath': config['mcPath'],
        'outFilePath': config['pathMCsysErrors']
    }
    # TODO: Give jetPtString etc.
    callRootMacro("calculateSystematicErrorsFromFastSimulation", arguments)
