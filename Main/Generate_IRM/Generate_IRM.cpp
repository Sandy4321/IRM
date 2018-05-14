// Generate_IRM.cpp
// Generate_IRMの過程に従って自動的に客を分割
// 入力:Generate_IRMのパラメータalpha,Generate_IRMの試行回数(客の人数)
// 出力:各客の座り方, 各机の客の人数
#include <iostream>
#include <random>
#include <vector>
#include "boost/multi_array.hpp"
using namespace boost;
#include "Generate_IRM.hpp"

Generate_IRM::Generate_IRM() {
  Generate_IRM_co_alpha = 0;
  number_of_K = 0;  //比較対象Kの総数,パターン認識本の例の顧客数
  number_of_L = 0;  //比較対象Lの総数,パターン認識本の例の商品数
  number_of_cluster_K = 0;  //比較対象Kの総クラスタ数
  number_of_cluster_L = 0;  //比較対象Lの総クラスタ数

  hidden_K.resize(1, 0);
  hidden_L.resize(1, 0);
  Output_Binary_Relation_k.resize(
      1, 0);  // 比較対象Kのk番目の要素とLの関係を表すL長vector
  Output_Binary_Relation_Matrix.resize(
      1, Output_Binary_Relation_k);  // A(boost::extents[10][10]);


Parameter_Relation_k.resize(1,0);

Parameter_Relation_Matrix.resize(
    1,
    Parameter_Relation_k);  // A(boost::extents[10][10]);
}

void Generate_IRM::run_Generate_IRM() {  // Generate_IRMの本体
  // Generate_IRM::Generate_IRM_first_customer();
  std::random_device seed_gen;  //乱数部分はあとでもっとglobalに纏められそう
  std::mt19937 engine(seed_gen());
}
