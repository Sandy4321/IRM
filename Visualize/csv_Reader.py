#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
import seaborn as sns
import pandas as pd
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
import csv
csv_int=[list(map(int,line.rstrip().split(","))) for line in open('Output_Binary_Relation_Matrix.csv').readlines()]
csv_2=[list(map(int,line.rstrip().split(","))) for line in open('Panda_out.csv').readlines()]
csv_3=np.array(csv_2)
tate =[list(map(int,line.rstrip().split(","))) for line in open('tatehidden_K.csv').readlines()]
 
yoko=[list(map(int,line.rstrip().split(","))) for line in open('yokohidden_L.csv').readlines()]

x = []
for s in yoko:
    x.extend(s)
df = pd.DataFrame(data=csv_int, index=tate,columns=x)

plt.figure()
sns.heatmap(df, square=True,annot=csv_3,cmap='inferno')
plt.savefig('seaborn_heatmap_list.png')
plt.show()
plt.close('all')

