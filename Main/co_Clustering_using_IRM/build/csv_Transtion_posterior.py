#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
import seaborn as sns
import pandas as pd
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
import csv
from PIL import Image

Posterior=[list(map(float,line.rstrip().split(","))) for line in open('Trantsition_Posterior_distriibution.csv').readlines()]

plt.figure()
sns.tsplot(Posterior)
plt.savefig('Posterior.png')
plt.show()
plt.close('all')


"""
csv_int=[list(map(int,line.rstrip().split(","))) for line in open('Output_Binary_Relation_Matrix.csv').readlines()]

csv_label=[list(map(float,line.rstrip().split(","))) for line in open('Output_size_Parameter_Relation_Matrix.csv').readlines()]
csv_label_list=np.array(csv_label)

tate =[list(map(int,line.rstrip().split(","))) for line in open('hidden_K.csv').readlines()] 
yoko=[list(map(int,line.rstrip().split(","))) for line in open('hidden_L.csv').readlines()]

csv_float=[list(map(float,line.rstrip().split(","))) for line in open('Parameter_Relation_Matrix.csv').readlines()]


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
sns.heatmap(Output_Parameter_label_sorted_by_both, square=True,annot=Output_sorted_by_both,fmt='.5g',cmap='Blues')
plt.savefig('heatmap_output.png')
#plt.show()
plt.close('all')

plt.figure()
sns.heatmap(csv_float, square=True,annot=True,fmt='.5g',cmap='Blues')
plt.savefig('heatmap_parameter.png')
#plt.show()
plt.close('all')

# 既存画像を読み込み
a_png = Image.open('heatmap_output.png', 'r')
b_png= Image.open('heatmap_parameter.png', 'r')

# マージに利用する下地画像を作成する
canvas = Image.new('RGB', (1600, 600), (255, 255, 255))

# pasteで、座標（0, 0）と（0, 100）に既存画像を乗せる。
canvas.paste(a_png.resize((800,600)), (0, 0))
canvas.paste(b_png.resize((800,600)), (800, 0))

# 保存
canvas.save('All_Output.png', 'PNG', quality=100, optimize=True)
"""