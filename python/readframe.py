import numpy as np
import struct

def readframe(fp):
    data = np.asarray(struct.unpack("H"*80*80, fp.read(80*80*2)))
    frame = np.zeros((80,80))
    for i in range(0, 80):
        for j in range(0, 20):
            for k in range(0, 4):
                frame[i,j+k*20] = data[i*80+j*4+k];
#    frame = np.reshape(data , (4, 20, 80))
    
    return frame, data    

def orderframe(data):
    frame = np.zeros((80,80))
    for i in range(0, 80):
        for j in range(0, 20):
            for k in range(0, 4):
                frame[i,j+k*20] = data[i*80+j*4+k];
    return frame

def fastorder(data):
    frame3d = np.reshape(data, (80, 20, 4))
    frame = np.empty( (80,80) )
    frame[:,0:20] = frame3d[:,:,0]
    frame[:,20:40] = frame3d[:,:,1]
    frame[:,40:60] = frame3d[:,:,2]
    frame[:,60:80] = frame3d[:,:,3]
    return frame
            
def loadCalib(filename):
    calib = numpy.loadtxt(filename)
    return calib



