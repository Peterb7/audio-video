import matplotlib.pyplot as plt
import audioBasicIO
import numpy
import cPickle
import audioTrainTest
# [features, classNames, fileNames] = audioFeatureExtraction.dirsWavFeatureExtraction(['train/bumps', 'train/door',
#                                                                                      'train/steps', 'train/speech',
#                                                                                      'train/specificDoor',
#                                                                                      'train/background'],
#                                                                                     0.25, 0.25, 0.02, 0.02)


audioTrainTest.featureAndTrain(['all/bumps', 'all/door', 'all/steps', 'all/speech', 'all/spcDoor',
                                'all/background', 'all/scream'], 0.5, 0.25,
                               0.25, 0.1, 'svm', 'svm_model')
