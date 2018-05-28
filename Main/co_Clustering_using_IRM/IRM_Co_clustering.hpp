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

  unsigned int gibbs_counter;
  unsigned int
      stop_counter;  //アップデートされなかったことが連続した時の打ち切り回数
  unsigned int not_updated_counter;  //アップデートされなかった回数

  unsigned int k_iterator;
  unsigned int l_iterator;

  unsigned int number_of_K;  //比較対象Kの総数,パターン認識本の例の顧客数
  unsigned int number_of_L;  //比較対象Lの総数,パターン認識本の例の商品数
  unsigned int number_of_cluster_K;      //比較対象Kの総クラスタ数
  unsigned int number_of_cluster_L;      //比較対象Lの総クラスタ数
  unsigned int tmp_number_of_cluster_K;  //比較対象Kの総クラスタ数
  unsigned int tmp_number_of_cluster_L;  //比較対象Lの総クラスタ数

  double Max_Posterior_Probability;
  double tmp_Posterior_Probability;

  std::vector<unsigned int>
      hidden_K;  //比較対象Kの潜在系列(Kの要素の各所属クラスタ情報)
  std::vector<unsigned int>
      hidden_L;  //比較対象Lの潜在系列(Lの要素の各所属クラスタ情報)

  std::vector<unsigned int>
      tmp_hidden_K;  //現時点での比較対象Kの潜在系列(Kの要素の各所属クラスタ情報)
  std::vector<unsigned int>
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

  std::vector<double> Trantsition_Posterior_distriibution;

 public:
  IRM_Co_Clustering();
  void set_gibbs_counter(unsigned int i);
  void set_stop_counter(unsigned int i);
  void Read_csv_Input_Binary_Relation_Matrix(const char *argv1);
  void set_number_of_K_L();
  void out_number_of_KL(int &tmp_K, int &tmp_L);
  void set_IRM_hyper_parameter(double alpha, double a, double b);
  void first_set_hidden_K_L(std::vector<int> &K_vector,
                            std::vector<int> &L_vector);
  void first_get_each_cluster_number();

  void tmp_hidden_K_get_each_cluster_number();
  void update_hidden_K();
  int get_new_cluster_for_K();
  double already_cluster_prob_K(unsigned int i);
  double log_Beta(double a, double b, double n_f_f_i_j, double bar_n_f_f_i_j,
                  double n_notk, double bar_n_not_k);
  double new_cluster_prob_K();

  void tmp_hidden_L_get_each_cluster_number();
  void update_hidden_L();
  int get_new_cluster_for_L();
  double already_cluster_prob_L(unsigned int i);
  double new_cluster_prob_L();

  void update_alpha();

  int decide_update_tmp_or_not_hidden_KL();
  double get_tmp_Posterior_Probability();
  double Logfactorial(double n);

  void Get_Parameter_Matrix();
  void Output_by_record_csv();

  void show_IRM_parameter();
  void show_datas();
};
#endif
