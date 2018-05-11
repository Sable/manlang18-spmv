import numpy as np
import sys
import csv
import matplotlib.pyplot as plt

def autolabels(rects, name):
  for i, rect in enumerate(rects):
    height = rect.get_height()
    ax.text(rect.get_x() + rect.get_width()/2, 1.02* height, '%s'%(name[i]), ha='center', va='bottom', label='number of matrices')


fig, ax = plt.subplots()
arg = sys.argv
#ax.set_title('Performance of our C vs our JS across optimal format in C(same in JS)(Higher is better)')
width = 0.20
with open(arg[1]) as csvfile:
  readCSV = csv.reader(csvfile, delimiter=',')
  names = []
  gflops = []
  num = []
  for row in readCSV:
    names.append(row[0])
    gflops.append(float(row[1]))
    num.append(int(row[2]))
  ind = np.arange(len(names))
  rect1 = ax.bar(ind, gflops, width, edgecolor="none", color = 'deeppink', label='Chrome best-vs-best'); 
with open(arg[2]) as csvfile:
  readCSV = csv.reader(csvfile, delimiter=',')
  names = []
  gflops = []
  num = []
  for row in readCSV:
    names.append(row[0])
    gflops.append(float(row[1]))
    num.append(int(row[2]))
  ind = np.arange(len(names))
  rect2 = ax.bar(ind+width, gflops, width, edgecolor="none", color = 'pink', label='Chrome best-vs-same'); 
with open(arg[3]) as csvfile:
  readCSV = csv.reader(csvfile, delimiter=',')
  names = []
  gflops = []
  num = []
  for row in readCSV:
    names.append(row[0])
    gflops.append(float(row[1]))
    num.append(int(row[2]))
  ind = np.arange(len(names))
  rect3 = ax.bar(ind+width*2, gflops, width, edgecolor="none", color = 'b', label='Firefox best-vs-best'); 
with open(arg[4]) as csvfile:
  readCSV = csv.reader(csvfile, delimiter=',')
  names = []
  gflops = []
  num = []
  for row in readCSV:
    names.append(row[0])
    gflops.append(float(row[1]))
    num.append(int(row[2]))
  ind = np.arange(len(names))
  rect4 = ax.bar(ind+width*3, gflops, width, edgecolor="none", color = 'deepskyblue', label='Firefox best-vs-same'); 
ax.axhline(y=1, color='r', linestyle='-', label='baseline C')
ax.legend(fontsize = 30, loc='best', fancybox=True, framealpha=0.5);
ax.set_ylabel('JavaScript Slowdown Compared to C', fontsize=30)
ax.set_xlabel('Sparse Matrix Storage Format in C', fontsize=30)
ax.set_xticks(ind+2*width);
labels = ['{0}\n({1:1})'.format(i,j) for i,j in zip(names, num)]
ax.set_xticklabels(labels);
#ax.set_ylim(0,12);
plt.xticks(fontsize=30) 
plt.yticks(fontsize=30) 
plt.show()
