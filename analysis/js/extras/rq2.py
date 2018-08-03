import matplotlib.pyplot as plt
import numpy as np
import sys
import csv

def make_autopct(values):
    def my_autopct(pct):
        total = sum(values)
        val = int(round(pct*total/100.0))
        return '{p:.1f}%  ({v:d})'.format(p=pct,v=val)
    return my_autopct

fig1, ax1 = plt.subplots()
#fig1.suptitle('Format Distribution for C') 
arg = sys.argv
colors = ['midnightblue', 'maroon', 'darkgreen', 'darkorchid', 'deeppink', 'darkorange', 'y', 'gold'] 
patterns = [ "/" , "\\" , "|", "." , "+" , "x", "o", "*", "0" ]

with open(arg[1]) as csvfile:
  readCSV = csv.reader(csvfile, delimiter=',')
  explode = list()
  names = []
  sizes = []
  for row in readCSV:
    names.append(row[0])
    sizes.append(int(row[1]))
    explode.append(0.1)
percent = 100.*np.array(sizes)/np.array(sizes).sum()
ax1.axis('equal')
pie = ax1.pie(sizes, explode=explode, colors=colors, startangle=90)
labels = ['{0} - {1:1.1f} %'.format(i,j) for i,j in zip(names, percent)]
ax1.legend(pie[0], labels, fontsize=30, loc='best', bbox_to_anchor=(-0.1, 1.0), framealpha=0.5);
plt.savefig('piechart.png', bbox_inches='tight')
