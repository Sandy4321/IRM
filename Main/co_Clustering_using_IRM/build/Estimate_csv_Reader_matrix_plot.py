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


csv_int=[list(map(int,line.rstrip().split(","))) for line in open('Input_Binary_Relation_Matrix.csv').readlines()]

csv_label=[list(map(float,line.rstrip().split(","))) for line in open('Estimate_Output_size_Parameter_Relation_Matrix.csv').readlines()]
csv_label_list=np.array(csv_label)

csv_number_label=[list(map(float,line.rstrip().split(","))) for line in open('Estimate_Each_cluster_number_label_Matrix.csv').readlines()]
csv_number_label_list=np.array(csv_number_label)
#print csv_number_label
print csv_number_label_list

tate =[list(map(int,line.rstrip().split(","))) for line in open('Estimate_hidden_K.csv').readlines()] 
yoko=[list(map(int,line.rstrip().split(","))) for line in open('Estimate_hidden_L.csv').readlines()]

csv_float=[list(map(float,line.rstrip().split(","))) for line in open('Estimate_Parameter_Relation_Matrix.csv').readlines()]
csv_float_list=np.array(csv_float)
print csv_float_list

x = []
for s in yoko:
    x.extend(s)
Output_Binary_Relation_Matrix = pd.DataFrame(data=csv_int, index=tate,columns=x)

#print Output_Binary_Relation_Matrix
Output_sorted_by_index = Output_Binary_Relation_Matrix.sort_index()
#print Output_sorted_by_index

Output_sorted_by_both = Output_sorted_by_index.sort_index(axis=1)
#print Output_sorted_by_both

Output_Parameter_label = pd.DataFrame(data=csv_label_list, index=tate,columns=x)
#print Output_Parameter_label
Output_Parameter_label_sorted_by_index = Output_Parameter_label.sort_index()
Output_Parameter_label_sorted_by_both = Output_Parameter_label_sorted_by_index.sort_index(axis=1)
#print Output_Parameter_label_sorted_by_both

plt.figure()
sns.heatmap(csv_int, square=True,annot=False,cmap='Blues')
plt.savefig('Raw_heatmap_output.png')
#plt.show()
plt.close('all')

plt.figure()
sns.heatmap(csv_label_list, square=True,annot=False,cmap='Blues')
plt.savefig('Raw_heatmap_parameter_output.png')
#plt.show()
plt.close('all')

plt.figure()
sns.heatmap(csv_float_list, square=True,annot=csv_number_label_list,fmt='.6g',cmap='Blues')
plt.savefig('Raw_heatmap_number.png')
#plt.show()
plt.close('all')


plt.figure()
sns.heatmap(Output_sorted_by_both, square=True,annot=False,cmap='Blues')
plt.savefig('Sort_heatmap_output.png')
#plt.show()
plt.close('all')


plt.figure()
sns.heatmap(Output_Parameter_label_sorted_by_both, square=True,annot=False,cmap='Blues')
plt.savefig('Sort_heatmap_parameter_output.png')
#plt.show()
plt.close('all')

plt.figure()
sns.heatmap(csv_float, square=True,annot=True,fmt='.3g',cmap='Blues')
plt.savefig('Sort_heatmap_parameter.png')
#plt.show()
plt.close('all')

# 既存画像を読み込み
a1_png = Image.open('Raw_heatmap_output.png', 'r')
b1_png = Image.open('Raw_heatmap_parameter_output.png', 'r')
c1_png= Image.open('Raw_heatmap_number.png', 'r')

a2_png = Image.open('Sort_heatmap_output.png', 'r')
b2_png = Image.open('Sort_heatmap_parameter_output.png', 'r')
c2_png= Image.open('Sort_heatmap_parameter.png', 'r')



# マージに利用する下地画像を作成する
canvas = Image.new('RGB', (9600, 4800), (255, 255, 255))

# pasteで、座標（0, 0）と（0, 100）に既存画像を乗せる。
canvas.paste(a1_png.resize((3200,2400)), (0, 0))
canvas.paste(b1_png.resize((3200,2400)), (3200, 0))
canvas.paste(c1_png.resize((3200,2400)), (6400, 0))

canvas.paste(a2_png.resize((3200,2400)), (0, 2400))
canvas.paste(b2_png.resize((3200,2400)), (3200, 2400))
canvas.paste(c2_png.resize((3200,2400)), (6400, 2400))

# 保存
canvas.save('Estimate_All_Output.png', 'PNG', quality=100, optimize=True)

# マージに利用する下地画像を作成する
canvas = Image.new('RGB', (9600, 2400), (255, 255, 255))

# pasteで、座標（0, 0）と（0, 100）に既存画像を乗せる。
canvas.paste(a2_png.resize((3200,2400)), (0, 0))
canvas.paste(b2_png.resize((3200,2400)), (3200, 0))
canvas.paste(c2_png.resize((3200,2400)), (6400, 0))

# 保存
canvas.save('Estimate_Sort_All_Output.png')






