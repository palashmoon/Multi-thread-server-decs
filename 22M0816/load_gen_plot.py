#!/usr/bin/python

import matplotlib.pyplot as plt
import numpy as np
import csv, sys, re, random, os, time

if(len(sys.argv)!=3):
    print("Usage python3 load_gen_plot.py <load-gen-output.csv> <load-gen-iostat.csv>")
    exit()
datafile=sys.argv[1]
Mcsv = csv.reader(open(datafile, 'r'), delimiter=',', quotechar='"')

# Mapping from column name to column index
I = {}

# Matrix of all the rows
M = []

rowNum = 0
for row in Mcsv:
    rowNum += 1
    if (rowNum == 1):
        for i in range(len(row)):
            I[row[i]] = i
        continue
    else:
        M.append(row)

user = list(map(lambda r: int(r[I['User']]), M))
req = list(map(lambda r: int(r[I['Request']]), M))
tp = list(map(lambda r: float(r[I['Throughput']]), M))
avgRTT = list(map(lambda r: float(r[I['AverageRTT']]), M))

ind = np.arange(len(user))
width = 0.30

plt.figure(figsize=(8,6))

plt.plot(user,tp, ls='--',label = 'Throughput', color='blue',marker='x', markersize=5, mew=2, linewidth=2)
plt.xlabel('No of Users')
plt.ylabel('Throughput (request/sec)')
#plt.xscale('log')
#plt.yscale('log')
plt.grid('on')
plt.savefig("load-gen-output-throughput.png", bbox_inches='tight')

plt.clf()

plt.plot(user,avgRTT, ls='--',label = 'Throughput', color='red', marker='o',markersize=5, mew=2, linewidth=2)
plt.xlabel('No of Users')
plt.ylabel('Average RTT (sec)')
#plt.xscale('log')
#plt.yscale('log')
plt.grid('on')
plt.savefig("load-gen-output-avgRTT.png", bbox_inches='tight')

plt.clf()

lines = []
fig, y1 = plt.subplots()
y2 = plt.twinx()

line, = y1.plot(user, tp, label="Throughput", ls='--', color='b',marker='x', mew=2, linewidth=2)
lines.append(line)
line, = y2.plot(user, avgRTT, label="Average RTT", ls='--', color='r',marker='o', mew=2, linewidth=2)
lines.append(line)

y1.set_xlabel('User')
y1.set_ylabel('Throughput (response/sec')
y2.set_ylabel('Average RTT (sec)')
y1.yaxis.label.set_color('b')
y1.tick_params(axis='y', colors='b')
y2.yaxis.label.set_color('r')
y2.tick_params(axis='y', colors='r')
#y1.set_xscale('log')
#y1.set_yscale('log')
#y2.set_yscale('log')
y1.axis(ymax=max(tp),ymin=min(tp))
y2.axis(ymax=max(avgRTT),ymin=min(avgRTT))
y1.grid('on')
y2.grid('on')
plt.legend(lines, [l.get_label() for l in lines], loc=(0.75,0.08), prop={'size':11})
plt.savefig("load-gen-output.png", bbox_inches='tight')

