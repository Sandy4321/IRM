#ifndef Generate_IRM_H_
#define Generate_IRM_H_
// Generate_IRM.hpp
// Generate_IRMの過程に従って自動的に客を分割
// 入力:Generate_IRMのパラメータalpha,Generate_IRMの試行回数(客の人数)
// 出力:各客の座り方, 各机の客の人数
#include <stdio.h>
#include <stdlib.h>
#include <boost/foreach.hpp>
#include <boost/math/special_functions/beta.hpp>
#include <boost/math/special_functions/factorials.hpp>
#include <boost/random.hpp>
#include <boost/tokenizer.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>
#include "boost/multi_array.hpp"

// typedef  boost::multi_array<int, 2> int_2dim_array_type;

class Generate_IRM {
 private:
  double Generate_IRM_co_alpha;  // Generate_IRMのパラメータalpha(二つで共通)
  double Generate_IRM_Beta_a;  // Generate_IRMのパラメータa(Beta分布で使用)
  double Generate_IRM_Beta_b;  // Generate_IRMのパラメータb(Beta分布で使用)

  int number_of_K;  //比較対象Kの総数,パターン認識本の例の顧客数
  int number_of_L;  //比較対象Lの総数,パターン認識本の例の商品数
  int number_of_cluster_K;  //比較対象Kの総クラスタ数
  int number_of_cluster_L;  //比較対象Lの総クラスタ数

  std::vector<int> hidden_K;  //比較対象Kの潜在系列(Kの要素の各所属クラスタ情報)
  std::vector<int> hidden_L;  //比較対象Lの潜在系列(Lの要素の各所属クラスタ情報)
  std::vector<int> number_of_k_in_each_cluster;  //比較対象Kの各クラスタの要素数
  std::vector<int> number_of_l_in_each_cluster;  //比較対象Lの各クラスタの要素数

  std::vector<int>
      Output_Binary_Relation_k;  // 比較対象Kのk番目の要素とLの関係を表すL長vector

  std::vector<std::vector<int> >
      Output_Binary_Relation_Matrix;  // KとLの関係を表すK*Lの二次元の二値行列.IRMの学習データ

  std::vector<double> Parameter_Relation_k;
  std::vector<std::vector<double> >
      Parameter_Relation_Matrix;  // KとLの各クラスタの関係を表すnumber_of_cluster_K*number_of_cluster_Lのパラメータ行列

 public:
  Generate_IRM();
  void set_number_of_K_L(int a, int b);
  void set_IRM_hyper_parameter(double alpha, double a, double b);

  void set_hidden_K_L(std::vector<int> &K_vector, std::vector<int> &L_vector);

  void get_each_cluster_number();
  void Set_Parameter_Relation_Matrix_Beta();
  void decide_Output_Binary_Relation_Matrix();
  void run_Generate_IRM();  // Generate_IRMの本体
  void Output_by_record_csv();
  double get_Posterior_Probability();
  int Logfactorial(int n);
  void show_IRM_parameter();
  void show_datas();
};
#endif
