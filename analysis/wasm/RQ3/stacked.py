import numpy as np
import sys
import csv
import matplotlib.pyplot as plt

def autolabels(rects, name):
  for i, rect in enumerate(rects):
    height = rect.get_height()
    ax.text(rect.get_x() + rect.get_width()/2, 1.02* height, '%s'%(name[i]), ha='center', va='bottom', rotation=90)
fig, ax = plt.subplots()
arg = sys.argv
colors = ['midnightblue', 'maroon', 'darkgreen', 'darkorchid', 'deeppink', 'darkorange', 'y', 'gold'] 
patterns = [ "/" , "\\" , "|" , "." , "+" , "x", "o", "*", "-", "0" ]
width = 0.35
num = []
COO = []
CSR = []
DIA = []
ELL = []
COO_CSR = []
COO_CSR_ELL = []
ALL = []
Others = []
names = []
with open(arg[1]) as csvfile:
  readCSV = csv.reader(csvfile, delimiter=',')
  for row in readCSV:
    names.append(row[0])
    COO.append(int(row[3]))
    CSR.append(int(row[5]))
    DIA.append(int(row[7]))
    ELL.append(int(row[9]))
    COO_CSR.append(int(row[11]))
    COO_CSR_ELL.append(int(row[13]))
    ALL.append(int(row[15]))
    Others.append(int(row[17]))
  ind = np.arange(len(COO))
rect1 = ax.bar(ind, COO, width, edgecolor="white", color = colors[0], hatch=patterns[0], label='COO'); 
rect2 = ax.bar(ind, CSR, width, bottom=COO, edgecolor="white", color = colors[1], hatch=patterns[1], label='CSR'); 
rect3 = ax.bar(ind, DIA, width, bottom=np.array(COO)+np.array(CSR), edgecolor="white", color = colors[2], hatch=patterns[2], label='DIA'); 
rect4 = ax.bar(ind, ELL, width, bottom=np.array(COO)+np.array(CSR)+np.array(DIA), edgecolor="white", color = colors[3], hatch=patterns[3], label='ELL'); 
rect5 = ax.bar(ind, COO_CSR, width, bottom=np.array(COO)+np.array(CSR)+np.array(DIA)+np.array(ELL), edgecolor="white", color = colors[4], hatch=patterns[4], label='COO-CSR'); 
rect6 = ax.bar(ind, COO_CSR_ELL, width, bottom=np.array(COO)+np.array(CSR)+np.array(DIA)+np.array(ELL)+np.array(COO_CSR), edgecolor="white", color = colors[5], hatch=patterns[5], label='COO-CSR-ELL'); 
rect7 = ax.bar(ind, ALL, width, bottom=np.array(COO)+np.array(CSR)+np.array(DIA)+np.array(ELL)+np.array(COO_CSR)+np.array(COO_CSR_ELL), edgecolor="white", color = colors[6], hatch=patterns[6], label='all'); 
rect8 = ax.bar(ind, Others, width, bottom=np.array(COO)+np.array(CSR)+np.array(DIA)+np.array(ELL)+np.array(COO_CSR)+np.array(COO_CSR_ELL)+np.array(ALL), edgecolor="white", color = colors[7], hatch=patterns[7], label='Others'); 
ax.legend(loc='best', fontsize = 22, fancybox=True);
ax.set_ylabel('Number of Matrices', fontsize=22)
ax.set_xlabel('Sparse Matrix Storage Format in C', fontsize=22)
ax.set_xticks(ind+width/2);
ax.set_xticklabels(names);
plt.xticks(fontsize=22) 
plt.yticks(fontsize=22) 
plt.show()
