import os, alsaaudio, audioop
import audioTrainTest as aT
import audioAnalysisRecordAlsa
import datetime
import sys
from PyQt4 import QtGui
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt4agg import NavigationToolbar2QT as NavigationToolbar
import matplotlib.pyplot as plt
import matplotlib
import random
import numpy
matplotlib.use('TkAgg')


def detect(funType, midTermBufferSizeSec, modelName, modelType):
	if funType == '-recordAndClassifySegments':  # record input
		# duration = int(midTermBufferSizeSec)
		modelName = modelName
		if modelType not in ["svm", "knn"]:
			raise Exception("ModelType has to be either svm or knn!")
		if not os.path.isfile(modelName):
			raise Exception("Input modelName not found!")
		audioAnalysisRecordAlsa.recordAnalyzeAudio(midTermBufferSizeSec, modelName, modelType)
	else:
		print "Error.\nSyntax: " + funType + " -recordAndClassifySegments <duration> <outputWafFile> <modelName> <modelType>"



detect('-recordAndClassifySegments', 1.5, 'svm_model', 'svm')
