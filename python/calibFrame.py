import numpy as np
import struct
import os
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import cv2

from readframe import readframe, orderframe, fastorder

slopes = fastorder(np.loadtxt(r"C:\XFCT\Convertor\Config\500V_28C_Grads.txt"))
intercepts = fastorder(np.loadtxt(r"C:\XFCT\Convertor\Config\500V_28C_Inters.txt"))
thresh = fastorder(np.loadtxt(r"C:\XFCT\Convertor\Config\500V_28C_Thresh.txt"))

filename = r"C:\XFCT\ScrollSample_30deg88keVMono_80mm200mm_400umPH_60mins_170716_155317.bin";

with open(filename, "rb") as fp:
    frameCount = 0;
#    M1 = np.zeros((80,80))
    M2 = np.zeros((80,80))
    histo = np.zeros( 8192 )
    histoCalib = np.zeros(1000)
    while True:
        buf = fp.read(80*80*2)
        if buf:
            frame = np.asarray(struct.unpack("H"*80*80, buf))
            ordered = fastorder(frame);
            ordered *= slopes
            ordered += intercepts
            M2 += ordered;
            histo[frame/8] += 1
            for i in range (0, 6400):
                if (ordered[i/80,i%80] > 0) and (ordered[i/80,i%80] < 10000):
                    histoCalib[np.uint(ordered[i/80,i%80]*10)] += 1
            
        else:
            break;
        frameCount += 1;
        if (frameCount % 1000 == 0):
            print "Frame count is", frameCount
        if (frameCount == 10000):
            break
            

print "Final frame count is", frameCount

plt.figure(1)
plt.matshow(M2, origin='lower', fignum=1);
plt.show(block = False)

plt.figure(2)
plt.semilogy(np.linspace(0, 8192, 8192), histo[0:8192], 'r', label = "Spectrum");
#plt.plot(np.linspace(0, nBins2/8, nBins2/2), histo2, 'b', label = "Background");
#plt.plot(np.linspace(0, nBins3/8, nBins3/2), histo3, 'g', label = "Gold");
plt.xlabel("Bin")
plt.ylabel("Relative intensity")
plt.legend(loc = "upper right")
plt.title("Full Data")
plt.show(block = False)

plt.figure(3)
plt.semilogy(np.linspace(50000, 100000, 500), histoCalib[500:1000], 'r', label = "Spectrum");
#plt.plot(np.linspace(0, nBins2/8, nBins2/2), histo2, 'b', label = "Background");
#plt.plot(np.linspace(0, nBins3/8, nBins3/2), histo3, 'g', label = "Gold");
plt.xlabel("Bin")
plt.ylabel("Relative intensity")
plt.legend(loc = "upper right")
plt.title("Full Data")
plt.show(block = True)

raise SystemExit
