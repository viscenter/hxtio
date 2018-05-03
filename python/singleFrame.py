import numpy as np
import struct
import os
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import cv2

from readframe import readframe, orderframe, fastorder

filename = r"C:\XFCT\ScrollSample_30deg88keVMono_80mm200mm_400umPH_60mins_170716_155317.bin";

with open(filename, "rb") as fp:
    frameCount = 0;
#    M1 = np.zeros((80,80))
    M2 = np.zeros((80,80))
    histo = np.zeros( 4096 )
    while True:
        buf = fp.read(80*80*2)
        if buf:
            frame = np.asarray(struct.unpack("H"*80*80, buf))
#            M1 += orderframe(frame);
            M2 += fastorder(frame);
#            print "Pixels with entries", (frame>20).sum()
            for i in range (0, 6400):
                histo[frame[i]/16] += 1
        else:
            break;
        frameCount += 1;
        if (frameCount % 1000 == 0):
            print "Frame count is", frameCount
        if (frameCount == 10000):
            break
            

#    plt.matshow(M1);
#    plt.show(block = False)

print "Final frame count is", frameCount
plt.matshow(M2);
plt.show(block = False)
plt.close()

plt.semilogy(np.linspace(0, 4096, 256), histo[0:256], 'r', label = "Spectrum");
#plt.plot(np.linspace(0, nBins2/8, nBins2/2), histo2, 'b', label = "Background");
#plt.plot(np.linspace(0, nBins3/8, nBins3/2), histo3, 'g', label = "Gold");
plt.xlabel("Bin")
plt.ylabel("Relative intensity")
plt.legend(loc = "upper right")
plt.title("Full Data")
plt.show(block = True)

raise SystemExit
