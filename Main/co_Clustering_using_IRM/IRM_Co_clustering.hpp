#ifndef IRM_CO_CLUSTERING
#define IRM_CO_CLUSTERING
// IRM_Co_clustering.hpp
// IRM_Co_Clusteringの過程に従って自動的に客を分割
// 入力:IRM_Co_Clusteringのパラメータalpha,IRM_Co_Clusteringの試行回数(客の人数)
// 出力:各客の座り方, 各机の客の人数
#include <stdio.h>
#include <stdlib.h>
#include <boost/foreach.hpp>
#include <boost/multi_array.hpp>
#include <boost/tokenizer.hpp>
#include <iostream>
#include <random>
#include <vector>
// typedef  boost::multi_array<int, 2> int_2dim_array_type;

class IRM_Co_Clustering {
 private:
  double
      IRM_Co_Clustering_co_alpha;  // IRM_Co_Clusteringのパラメータalpha(二つで共通)
  double
      IRM_Co_Clustering_Beta_a;  // IRM_Co_Clusteringのパラメータa(Beta分布で使用)
  double
      IRM_Co_Clustering_Beta_b;  // IRM_Co_Clusteringのパラメータb(Beta分布で使用)

  int number_of_K;  //比較対象Kの総数,パターン認識本の例の顧客数
  int number_of_L;  //比較対象Lの総数,パターン認識本の例の商品数
  int number_of_cluster_K;  //比較対象Kの総クラスタ数
  int number_of_cluster_L;  //比較対象Lの総クラスタ数
  int tmp_number_of_cluster_K;  //比較対象Kの総クラスタ数
  int tmp_number_of_cluster_L;  //比較対象Lの総クラスタ数

  double Max_Posterior_Probability;
  double tmp_Posterior_Probability;

  std::vector<int> hidden_K;  //比較対象Kの潜在系列(Kの要素の各所属クラスタ情報)
  std::vector<int> hidden_L;  //比較対象Lの潜在系列(Lの要素の各所属クラスタ情報)

  std::vector<int>
      tmp_hidden_K;  //現時点での比較対象Kの潜在系列(Kの要素の各所属クラスタ情報)
  std::vector<int>
      tmp_hidden_L;  //現時点での比較対象Lの潜在系列(Lの要素の各所属クラスタ情報)

  std::vector<int> number_of_k_in_each_cluster;  //比較対象Kの各クラスタの要素数
  std::vector<int> number_of_l_in_each_cluster;  //比較対象Lの各クラスタの要素数
  std::vector<int>
      tmp_number_of_k_in_each_cluster;  //比較対象Kの各クラスタの要素数
  std::vector<int>
      tmp_number_of_l_in_each_cluster;  //比較対象Lの各クラスタの要素数

  std::vector<int>
      Input_Binary_Relation_k;  // 比較対象Kのk番目の要素とLの関係を表すL長vector

  std::vector<std::vector<int> >
      Input_Binary_Relation_Matrix;  // KとLの関係を表すK*Lの二次元の二値行列.IRMの学習データ

  std::vector<double> Parameter_Relation_k;
  std::vector<std::vector<double> >
      Parameter_Relation_Matrix;  // KとLの各クラスタの関係を表すnumber_of_cluster_K*number_of_cluster_Lのパラメータ行列

 public:
  IRM_Co_Clustering();
  void Read_csv_Input_Binary_Relation_Matrix(const char *argv1);
  void set_number_of_K_L();
  void out_number_of_KL(int &tmp_K, int &tmp_L);
  void set_IRM_hyper_parameter(double alpha, double a, double b);
  void first_set_hidden_K_L(std::vector<int> &K_vector,
                            std::vector<int> &L_vector);
  void first_get_each_cluster_number();

  void update_hidden_K();
  void tmp_hidden_K_get_each_cluster_number();

  void update_hidden_L();

  void decide_update_tmp_or_not_hidden_KL();

  void Set_Parameter_Relation_Matrix_Beta();
  void decide_Output_Binary_Relation_Matrix();
  void run_IRM_Co_Clustering();  // IRM_Co_Clusteringの本体

  void Output_by_record_csv();

  void show_IRM_parameter();
  void show_datas();
};
#endif
