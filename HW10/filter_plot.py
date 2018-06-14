#!/usr/bin/env python

import serial
import matplotlib.pyplot as plt

def main():
    port = serial.Serial('/dev/ttyACM0',9600,timeout=60)
    port.write('r')

    raw = []
    maf = []
    iir = []
    fir = []

    n = 100
    for i in range(n):
        read = port.readline()
        data = read.split() #Split string
        raw.append(float(data[1]))
        maf.append(float(data[2]))
        iir.append(float(data[3]))
        fir.append(float(data[4]))

    port.close()

    plt.figure(1)
    plt.plot(raw, label='Raw', color='k')
    plt.plot(maf, label='MAF', color='r')
    plt.plot(iir, label='IIR', color='g')
    plt.plot(fir, label='FIR', color='b')
    plt.legend()
    plt.show()

if __name__ == '__main__':
    main()