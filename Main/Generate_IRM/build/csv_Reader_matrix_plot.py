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
sns.heatmap(Output_sorted_by_both, square=True,annot=False,cmap='Blues')
plt.savefig('heatmap_output.png')
#plt.show()
plt.close('all')


plt.figure()
sns.heatmap(Output_Parameter_label_sorted_by_both, square=True,annot=False,cmap='Blues')
plt.savefig('heatmap_parameter_output.png')
#plt.show()
plt.close('all')

plt.figure()
sns.heatmap(csv_float, square=True,annot=True,fmt='.5g',cmap='Blues')
plt.savefig('heatmap_parameter.png')
#plt.show()
plt.close('all')

# 既存画像を読み込み
a_png = Image.open('heatmap_output.png', 'r')
b_png = Image.open('heatmap_parameter_output.png', 'r')
c_png= Image.open('heatmap_parameter.png', 'r')


# マージに利用する下地画像を作成する
canvas = Image.new('RGB', (4800, 1200), (255, 255, 255))

# pasteで、座標（0, 0）と（0, 100）に既存画像を乗せる。
canvas.paste(a_png.resize((1600,1200)), (0, 0))
canvas.paste(b_png.resize((1600,1200)), (1600, 0))
canvas.paste(c_png.resize((1600,1200)), (3200, 0))

# 保存
canvas.save('All_Output.png', 'PNG', quality=100, optimize=True)
