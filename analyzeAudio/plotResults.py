import matplotlib.pyplot as plt
from pyAudioAnalysis import audioBasicIO
import numpy
import cPickle
import audioTrainTest
# [features, classNames, fileNames] = audioFeatureExtraction.dirsWavFeatureExtraction(['train/bumps', 'train/door',
#                                                                                      'train/steps', 'train/speech',
#                                                                                      'train/specificDoor',
#                                                                                      'train/background'],
#                                                                                     0.25, 0.25, 0.02, 0.02)

[Fs1, x1] = audioBasicIO.readAudioFile("backgr5.wav");
x1 = audioBasicIO.stereo2mono(x1)
[Fs2, x2] = audioBasicIO.readAudioFile("boots10.wav");
x2 = audioBasicIO.stereo2mono(x2)
[Fs3, x3] = audioBasicIO.readAudioFile("door12.wav");
x3 = audioBasicIO.stereo2mono(x3)
[Fs4, x4] = audioBasicIO.readAudioFile("drop_key1.wav");
x4 = audioBasicIO.stereo2mono(x4)
[Fs5, x5] = audioBasicIO.readAudioFile("eng6.wav");
x5 = audioBasicIO.stereo2mono(x5)
[Fs6, x6] = audioBasicIO.readAudioFile("man_scream1.wav");
x6 = audioBasicIO.stereo2mono(x6)
[Fs7, x7] = audioBasicIO.readAudioFile("door19.wav");
x7 = audioBasicIO.stereo2mono(x7)
print Fs1
print x1

# print(Fs1*0.040)
# print(len(x1))
# print(Fs1,Fs2,Fs3,Fs4,Fs5,Fs6,Fs7)
# F1 = audioFeatureExtraction.stFeatureExtraction(x1, Fs1, 0.040*Fs1, 0.020*Fs1);
# F2 = audioFeatureExtraction.stFeatureExtraction(x2, Fs2, 0.040*Fs2, 0.020*Fs2);
# F3 = audioFeatureExtraction.stFeatureExtraction(x3, Fs3, 0.040*Fs3, 0.020*Fs3);
# F4 = audioFeatureExtraction.stFeatureExtraction(x4, Fs4, 0.040*Fs4, 0.020*Fs4);
# F5 = audioFeatureExtraction.stFeatureExtraction(x5, Fs5, 0.040*Fs5, 0.020*Fs5);
# F6 = audioFeatureExtraction.stFeatureExtraction(x6, Fs6, 0.040*Fs6, 0.020*Fs6);
# F7 = audioFeatureExtraction.stFeatureExtraction(x7, Fs7, 0.040*Fs7, 0.020*Fs7);
#
#
# plt.plot(x7);plt.show
#
# plt.subplot(7,1,1); plt.plot(F1[0,:]); plt.xlabel('Frame no'); plt.ylabel('background');plt.title("ZCR")
# plt.subplot(7,1,2); plt.plot(F2[0,:]); plt.xlabel('Frame no'); plt.ylabel('steps');
# plt.subplot(7,1,3); plt.plot(F3[0,:]); plt.xlabel('Frame no'); plt.ylabel('door');
# plt.subplot(7,1,4); plt.plot(F4[0,:]); plt.xlabel('Frame no'); plt.ylabel('bumps');
# plt.subplot(7,1,5); plt.plot(F5[0,:]); plt.xlabel('Frame no'); plt.ylabel('speech');
# plt.subplot(7,1,6); plt.plot(F6[0,:]); plt.xlabel('Frame no'); plt.ylabel('scream');
# plt.subplot(7,1,7); plt.plot(F7[0,:]); plt.xlabel('Frame no'); plt.ylabel('creaking door');plt.show()

# plt.subplot(7,1,1); plt.plot(F1[2,:]); plt.xlabel('Frame no'); plt.ylabel('background');plt.title("Entropy of Energy")
# plt.subplot(7,1,2); plt.plot(F2[2,:]); plt.xlabel('Frame no'); plt.ylabel('steps');
# plt.subplot(7,1,3); plt.plot(F3[2,:]); plt.xlabel('Frame no'); plt.ylabel('door');
# plt.subplot(7,1,4); plt.plot(F4[2,:]); plt.xlabel('Frame no'); plt.ylabel('bumps');
# plt.subplot(7,1,5); plt.plot(F5[2,:]); plt.xlabel('Frame no'); plt.ylabel('speech');
# plt.subplot(7,1,6); plt.plot(F6[2,:]); plt.xlabel('Frame no'); plt.ylabel('scream');
# plt.subplot(7,1,7); plt.plot(F7[2,:]); plt.xlabel('Frame no'); plt.ylabel('creaking door');plt.show()

# plt.subplot(7,1,1); plt.plot(F1[3,:]); plt.xlabel('Frame no'); plt.ylabel('background');plt.title("Spectral Centroid")
# plt.subplot(7,1,2); plt.plot(F2[3,:]); plt.xlabel('Frame no'); plt.ylabel('steps');
# plt.subplot(7,1,3); plt.plot(F3[3,:]); plt.xlabel('Frame no'); plt.ylabel('door');
# plt.subplot(7,1,4); plt.plot(F4[3,:]); plt.xlabel('Frame no'); plt.ylabel('bumps');
# plt.subplot(7,1,5); plt.plot(F5[3,:]); plt.xlabel('Frame no'); plt.ylabel('speech');
# plt.subplot(7,1,6); plt.plot(F6[3,:]); plt.xlabel('Frame no'); plt.ylabel('scream');
# plt.subplot(7,1,7); plt.plot(F7[3,:]); plt.xlabel('Frame no'); plt.ylabel('creaking door');plt.show()

# plt.subplot(7,1,1); plt.plot(F1[4,:]); plt.xlabel('Frame no'); plt.ylabel('background');plt.title("Spectral Spread")
# plt.subplot(7,1,2); plt.plot(F2[4,:]); plt.xlabel('Frame no'); plt.ylabel('steps');
# plt.subplot(7,1,3); plt.plot(F3[4,:]); plt.xlabel('Frame no'); plt.ylabel('door');
# plt.subplot(7,1,4); plt.plot(F4[4,:]); plt.xlabel('Frame no'); plt.ylabel('bumps');
# plt.subplot(7,1,5); plt.plot(F5[4,:]); plt.xlabel('Frame no'); plt.ylabel('speech');
# plt.subplot(7,1,6); plt.plot(F6[4,:]); plt.xlabel('Frame no'); plt.ylabel('scream');
# plt.subplot(7,1,7); plt.plot(F7[4,:]); plt.xlabel('Frame no'); plt.ylabel('creaking door');plt.show()

# plt.subplot(7,1,1); plt.plot(F1[5,:]); plt.xlabel('Frame no'); plt.ylabel('background');plt.title("Spectral Entropy")
# plt.subplot(7,1,2); plt.plot(F2[5,:]); plt.xlabel('Frame no'); plt.ylabel('steps');
# plt.subplot(7,1,3); plt.plot(F3[5,:]); plt.xlabel('Frame no'); plt.ylabel('door');
# plt.subplot(7,1,4); plt.plot(F4[5,:]); plt.xlabel('Frame no'); plt.ylabel('bumps');
# plt.subplot(7,1,5); plt.plot(F5[5,:]); plt.xlabel('Frame no'); plt.ylabel('speech');
# plt.subplot(7,1,6); plt.plot(F6[5,:]); plt.xlabel('Frame no'); plt.ylabel('scream');
# plt.subplot(7,1,7); plt.plot(F7[5,:]); plt.xlabel('Frame no'); plt.ylabel('creaking door');plt.show()

# plt.subplot(7,1,1); plt.plot(F1[6,:]); plt.xlabel('Frame no'); plt.ylabel('background');plt.title("Spectral Flux")
# plt.subplot(7,1,2); plt.plot(F2[6,:]); plt.xlabel('Frame no'); plt.ylabel('steps');
# plt.subplot(7,1,3); plt.plot(F3[6,:]); plt.xlabel('Frame no'); plt.ylabel('door');
# plt.subplot(7,1,4); plt.plot(F4[6,:]); plt.xlabel('Frame no'); plt.ylabel('bumps');
# plt.subplot(7,1,5); plt.plot(F5[6,:]); plt.xlabel('Frame no'); plt.ylabel('speech');
# plt.subplot(7,1,6); plt.plot(F6[6,:]); plt.xlabel('Frame no'); plt.ylabel('scream');
# plt.subplot(7,1,7); plt.plot(F7[6,:]); plt.xlabel('Frame no'); plt.ylabel('creaking door');plt.show()

# plt.subplot(7,1,1); plt.plot(F1[7,:]); plt.xlabel('Frame no'); plt.ylabel('background');plt.title("Spectral Rolloff")
# plt.subplot(7,1,2); plt.plot(F2[7,:]); plt.xlabel('Frame no'); plt.ylabel('steps');
# plt.subplot(7,1,3); plt.plot(F3[7,:]); plt.xlabel('Frame no'); plt.ylabel('door');
# plt.subplot(7,1,4); plt.plot(F4[7,:]); plt.xlabel('Frame no'); plt.ylabel('bumps');
# plt.subplot(7,1,5); plt.plot(F5[7,:]); plt.xlabel('Frame no'); plt.ylabel('speech');
# plt.subplot(7,1,6); plt.plot(F6[7,:]); plt.xlabel('Frame no'); plt.ylabel('scream');
# plt.subplot(7,1,7); plt.plot(F7[7,:]); plt.xlabel('Frame no'); plt.ylabel('creaking door');plt.show()
#
