import pathlib
from pathlib import Path
import subprocess
from shlex import split
import json

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


def callRootMacro(name, arguments, doNotQuit = False, doNotRunInBackground = False):
    folderForMacros = str(pathlib.Path().resolve()) + '/RootMacros/'
    macroPath = folderForMacros + name + ".C"
    filePath = Path(macroPath)
    if not filePath.is_file():
        print("Macro " + macroPath + " not found! Fix code!")
        exit()
        
    cmd = "aliroot '" + macroPath + "+("

    argStrings = list(())

    for argName, argValue in arguments.items():
        if type(argValue) == str and argValue not in ('kFALSE', 'kTRUE'):
            argStrings.append("\"" + argValue + "\"")
        else:
            argStrings.append(str(argValue))

    cmd += ", ".join(argStrings) + ")' -l"
    if not doNotRunInBackground:
      cmd += " -b"
    if not doNotQuit:
      cmd += " -q"
      
    subprocess.run(split(cmd))
    
def callScript(argumentString):
    argumentString = str(pathlib.Path().resolve()) + "/" + argumentString
    subprocess.run(split(argumentString))

def produceSystematicValues(systematicsFile):
    systematicValues = {}
    systematicValues['Splines'] = {"down":[],"up":[]}
    print("Low pT spline systematics")
    arguments = {
        'fileName': systematicsFile,
        'canvasName': "csplineAccuracyLowP"
    }
    callRootMacro("GetCanvasOutOfFile", arguments, True, True)
    deviationUp = float(input("Check the boundary of the data. Ignore the rises of TPC+TOF kaons and protons above 1.8 GeV. Ignore the values for 4 GeV or higher with high error bars. Deviation up and down should be set to the same value (though not required). Now enter deviation up"))
    deviationDown = abs(float(input("Now enter deviation down")))
    systematicValues['Splines']["down"].append(deviationDown)
    systematicValues['Splines']["up"].append(deviationUp)
    
    print("High pT spline systematics. Taken from graphs")
    arguments = {
        'fileName': systematicsFile,
        'canvasName': "Residuals"
    }
    callRootMacro("GetCanvasOutOfFile", arguments, True, True)
    arguments = {
        'fileName': systematicsFile,
        'canvasName': "Comparison"
    }
    callRootMacro("GetCanvasOutOfFile", arguments, True, True)
    deviationUp = float(input("Determine the deviation up for values of high betaGamma. Enter it now"))
    deviationDown = abs(float(input("Now enter deviation down")))
    systematicValues['Splines']["down"].append(deviationDown)
    systematicValues['Splines']["up"].append(deviationUp)
    
    print("Low pT eta systematics")
    systematicValues['Eta'] = {"down":[],"up":[]}
    arguments = {
        'fileName': systematicsFile,
        'canvasName': "cEtaSystematicslowP"
    }
    callRootMacro("GetCanvasOutOfFile", arguments, True, True)
    deviationUp = float(input("Check the boundary of the data. Deviation up and down should be set to the same value (though not required). Now enter deviation up"))
    deviationDown = abs(float(input("Now enter deviation down")))
    systematicValues['Eta']["down"].append(deviationDown )
    systematicValues['Eta']["up"].append(deviationUp)
    
    print("High pT eta systematics")
    arguments = {
        'fileName': systematicsFile,
        'canvasName': "cEtaSystematicshighP"
    }
    callRootMacro("GetCanvasOutOfFile", arguments, True, True)
    deviationUp = float(input("Check the boundary of the data. Deviation up and down should be set to the same value (though not required). Now enter deviation up"))
    deviationDown = abs(float(input("Now enter deviation down")))
    systematicValues['Eta']["down"].append(deviationDown) 
    systematicValues['Eta']["up"].append(deviationUp)
    
    print("Resolution systematics")
    systematicValues['Sigma'] = {"down":[],"up":[]}
    arguments = {
        'fileName': systematicsFile,
        'canvasName': "cResolutionSystematics"
    }
    callRootMacro("GetCanvasOutOfFile", arguments, True, True)
    deviationUp = float(input("Check the boundary of the data. Deviation up and down should be set to the same value (though not required). Now enter deviation up"))
    deviationDown = abs(float(input("Now enter deviation down")))
    systematicValues['Sigma']["down"].append(deviationDown)
    systematicValues['Sigma']["up"].append(deviationUp)
    
    print("Multiplicity systematics")
    systematicValues['Mult'] = {"down":[],"up":[]}
    arguments = {
        'fileName': systematicsFile,
        'canvasName': "cMultCorrectionAccuracy"
    }
    callRootMacro("GetCanvasOutOfFile", arguments, True, True)
    deviationUp = float(input("Check the boundary of the data. Deviation up and down should be set to the same value (though not required). Ignore large outliers. Now enter deviation up"))
    deviationDown = abs(float(input("Now enter deviation down")))
    systematicValues['Mult']["down"].append(deviationDown)   
    systematicValues['Mult']["up"].append(deviationUp)
    
    print(systematicValues)
    return systematicValues
  
  
def updateSystematicValuesInConfig(systematicValues, configFileName):
    with open(configFileName, "r") as configFile:
        completeConfig = json.loads(configFile.read())
        
    for systematicName,values in completeConfig["systematics"].items():
        if systematicName not in systematicValues:
            continue
        
        singleSystematicValues = systematicValues[systematicName]
        completeConfig["systematics"][systematicName]["values"] = singleSystematicValues
            
    configAsJson = json.dumps(completeConfig, indent=4)
    
    with open(configFileName, "w") as configFile:
        configFile.write(configAsJson)
        

def runSystematicProcess(systematicsToProcess, systematics, config, jetString, systematicDay):
    analysisFolder = config['analysisFolder']
    # Filter only systematics active in this analysis (inclusive, jet or ue)
    systematicsPropertiesToProcess = dict(filter(
        lambda systematic: systematic[0] in systematicsToProcess, systematics.items()))

    if len(systematicsPropertiesToProcess) == 0:
        print("WARNING: No Systematics to process for " + jetString)
        return

    modeString = config['modesInclusiveString'] if jetString.find(
        'Inclusive') != -1 else config['modesJetsString']

    systematicInfoString = getSystematicInfoString(
        systematicsPropertiesToProcess, analysisFolder)


    referenceFileSchema = config['fileNamePattern']

    for sysName, sysVariables in systematicsPropertiesToProcess.items():
        try:
            referenceFileSchema = sysVariables['referenceFileNamePattern']
            break
        except KeyError:
            pass

    arguments = {'jetString': jetString, 'chargeString': config['chargeString'], 'referencepath': analysisFolder + '/Data', 'outfilepath': analysisFolder + foldersToCreate['single'], 'referenceFileSchema': referenceFileSchema,
                 'stringForSystematicInformation': systematicInfoString, 'centStepsString': config['centString'], 'jetPtStepsString': config['jetPtString'], 'modesInputString': modeString, 'nSigma': config['nSigma']}

    callRootMacro("runSystematicErrorEstimation", arguments)

    outputFilePatternList = list(())
    for sysName, sysVariables in systematicsPropertiesToProcess.items():
        outputFilePatternList.append(
            analysisFolder + foldersToCreate['single'] + sysVariables['outputFilePattern'])

    outPutFiles = "|".join(outputFilePatternList)

    arguments = {'jetString': jetString, 'chargeString': config['chargeString'], 'date': systematicDay, 'referenceFile': analysisFolder + '/Data/' + referenceFileSchema, 'centStepsString': config['centString'],
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
    # For now, we do not use a specific ue efficiency file
    effFile = config['mcPath'] + config['efficiencyFileNamePattern'].format(jetString.replace("_UE",""))

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
                if obs == "jT":
                    obs = "Jt";
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
    #### Produce fast simulation parameters. Runs in a loop until fit looks good. ###
    parameters = fastSimulationConfig['parameters']
    parameterList = list(())
            
    for spec,params in parameters.items():
        for charge, fitparams in params.items():
            parameterList.append('-'.join(fitparams))

    parametersString = ';'.join(parameterList)
    
    outputFile = config['mcPath'] + "fastSimulationParameters" + "_" + config['MCRunName'] + ".root"
    arguments = {
        'effFile': config['mcPath'] + config['efficiencyFileNamePattern'].format('Jets_Inclusive'),
        'outputfile': outputFile,
        'parameters': parametersString
    }
    callRootMacro("FitFastSimulationFactors", arguments)
    print("Check Output! Do the parametrized efficiencies follow the full mc efficiency? If uncertain, check also the single qa canvases in " + outputFile + ". If yes, edit the config.json. Leave aliroot with .q (maybe you have to press <ENTER> before.")
    arguments = {
        'fileName': outputFile,
        'canvasName': "cEfficiencies"
    }
    callRootMacro("GetCanvasOutOfFile", arguments, True, True)
    decision = input("Now press N/<AnyKey> to answer if the fit fits! If you press N, make certain you have already changed the config.json. For Y, the result file file will be uploaded to alien.")
    if decision.lower() == "n":
        #### Reload config file ###
        analysisFolder = config["analysisFolder"]
        mcPath = config["mcPath"]
        mcSysErrorPath = config["pathMCsysErrors"]
        
        with open(analysisFolder + "config.json", "r") as configFile:
            config = json.loads(configFile.read())
            
        fastSimulationConfig = config["fastSimulation"]
        config = config["config"]
        config["analysisFolder"] = analysisFolder
        config["mcPath"] = mcPath
        config["pathMCsysErrors"] = mcSysErrorPath
        fitFastSimulationFactors(config, fastSimulationConfig)
    else:
        subprocess.call(split("alien.py cp file://" + outputFile + " " + fastSimulationConfig["fastEffFileRemotePath"]))
        return
    
    

def createCorrectionFactorsFullMC(config):
    #### Produce Corrections factors of full MC run
    arguments = {
        'pathNameEfficiency': config['mcPath'] + config['efficiencyFileNamePattern'].format('Jets'),
        'outfileName': config['mcPath'] + "/corrections_{}.root".format(config['MCRunName']),
        'jetPtStepsString': config['jetPtString'],
        'modesInputString': config['modesJetsString']
    }
    # TODO: Give observables, jetPts as argument
    callRootMacro("createFileForBbBCorrections", arguments)

def writeCorrectionFiles(config, fastSimulationConfig):
    ### Write correction files ###
    correctionNames = [fastSimulationConfig["genericFastSimulation"]["folder"]]
    for varName,varInfo in fastSimulationConfig["variations"].items():
        if varInfo["active"]:
            for inputInfo in varInfo["inputs"]:
                correctionNames.append(inputInfo["folder"])
            
    for correctionName in correctionNames:
        arguments = {
            'effFile': config['mcPath'] + correctionName + '/' +  config['efficiencyFileNamePattern'].format('Jets'),
            'outFile': config['mcPath'] + "/outCorrections_FastJetSimulation_{}.root".format(correctionName),
            'jetPtStepsString': config['jetPtString'],
            'modesInputString': config['modesJetsString']
        }
        callRootMacro("writeOutCorrectionFiles", arguments)

def calculateJetMCCorrectionSysErrors(config, fastSimulationConfig):
    #### Produce systematic errors for jet mc correction ###
    originalMCFile = "corrections_{}.root".format(config['MCRunName'])
    genericFastSimulationFile = "outCorrections_FastJetSimulation_{}.root".format(fastSimulationConfig["genericFastSimulation"]["folder"])
    for varName,varInfo in fastSimulationConfig["variations"].items():
        if varInfo["active"]:
            arguments = {
                'effFilePath': config['mcPath'],
                'outFilePath': config['pathMCsysErrors'],
                'originalMCFile': originalMCFile,
                'genericFastSimulationFile': genericFastSimulationFile,
                'variationShortName': varInfo["short"],
                'variationLegendEntry': varInfo["legendEntryName"],
                'simpleCalculationSysError': 'kTRUE' if varInfo["simpleSysErrorCalculation"] == "1" else 'kFALSE',
                'variationDownFile': "outCorrections_FastJetSimulation_{}.root".format(varInfo["inputs"][0]["folder"]),
                'variationDownName': varInfo["inputs"][0]["name"],
                'variationUpFile': "outCorrections_FastJetSimulation_{}.root".format(varInfo["inputs"][1]["folder"]),
                'variationUpName': varInfo["inputs"][1]["name"],
                'jetPtStepsString': config['jetPtString'],
                'modesInputString': config['modesJetsString']
            }
            callRootMacro("calculateSystematicErrorsFromFastSimulation", arguments)
