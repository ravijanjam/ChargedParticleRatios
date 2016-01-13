from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing('analysis')

''' Declare VarParsing options to be used from commandline '''
''' ====================================================== '''

options.register('inputFile', 
				'', 
				VarParsing.multiplicity.singleton,
				VarParsing.varType.string,
				"Maximum file size in Kb")

options.register('maxNumEvents', 
				0, 
				VarParsing.multiplicity.singleton,
				VarParsing.varType.int,
				"Maximum file size in Kb")

options.register('outputFileName', 
				'', 
				VarParsing.multiplicity.singleton,
				VarParsing.varType.string,
				"Maximum file size in Kb")

''' Add this line if varparsing needs to work '''
options.parseArguments()
''' ====================================================== '''
''' end of VarParsing options declaration '''

print options.inputFile, options.outputFileName, options.maxNumEvents

''' 
cmsRun varParsingTest.py maxEvents=10 outputFileName='output.root' inputFile='file.root'

''' 
