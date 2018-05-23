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
csv_label=[list(map(float,line.rstrip().split(","))) for line in open('Output_size_Parameter_Relation_Matrix.csv').readlines()]
csv_label_list=np.array(csv_label)

tate =[list(map(int,line.rstrip().split(","))) for line in open('hidden_K.csv').readlines()] 
yoko=[list(map(int,line.rstrip().split(","))) for line in open('hidden_L.csv').readlines()]



x = []
for s in yoko:
    x.extend(s)
Output_Binary_Relation_Matrix = pd.DataFrame(data=csv_int, index=tate,columns=x)
print Output_Binary_Relation_Matrix
Output_sorted_by_index = Output_Binary_Relation_Matrix.sort_index()
print Output_sorted_by_index

Output_sorted_by_both = Output_sorted_by_index.sort_index(axis=1)
print Output_sorted_by_both

Output_Parameter_label = pd.DataFrame(data=csv_label_list, index=tate,columns=x)
print Output_Parameter_label
Output_Parameter_label_sorted_by_index = Output_Parameter_label.sort_index()
Output_Parameter_label_sorted_by_both = Output_Parameter_label_sorted_by_index.sort_index(axis=1)
print Output_Parameter_label_sorted_by_both


plt.figure()
sns.heatmap(Output_Parameter_label_sorted_by_both, square=True,annot=Output_sorted_by_both,fmt='.1g',cmap='Blues')
plt.savefig('seaborn_heatmap_list.png')
plt.show()
plt.close('all')

