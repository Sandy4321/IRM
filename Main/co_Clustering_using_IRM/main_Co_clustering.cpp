// main_Co_clustering.cpp
// IRMの生成過程に従って自動的にデータを分割
// 入力:IRMのパラメータalpha(最初は共通),比較対象Kの総数(パターン認識本の顧客人数),比較対象Lの総数(パターン認識本の商品の総数)
// 出力:
// KとLの関係を表すK*Lの二次元の二値行列.IRMの学習データ,（必要に応じて比較対象KとLの潜在系列(KとLの要素の各所属クラスタ情報))
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
//#include <stdio.h>
//#include <stdlib.h>

#include "CRP.hpp"
#include "IRM_Co_clustering.hpp"

int main(int argc, const char *argv[]) {
  if (argc != 5) {
    std::cout << "This program need file name + CRP alpha + Beta_a + Beta_b"
              << std::endl;
    return 0;
  }

  double parameter_co_alpha = std::atoi(
      argv[2]);  //二つのCRPに共通のパラメータalpha(共通でなくてもよし)
  double parameter_Beta_a = std::atoi(argv[3]);
  double parameter_Beta_b = std::atoi(argv[4]);

  IRM_Co_Clustering IRM;

  IRM.Read_csv_Input_Binary_Relation_Matrix(argv[1]);
  IRM.set_number_of_K_L();
  IRM.set_IRM_hyper_parameter(parameter_co_alpha, parameter_Beta_a,
                              parameter_Beta_b);

  std::vector<int> tmp_K;  //クラス間の(CRP→IRM)のvecのやり取りのため.気持ち悪い
  std::vector<int> tmp_L;
  int K_number_tmp = 0;
  int L_number_tmp = 0;

  CRP K_CRP, L_CRP;                                  // CRPで初期化
  IRM.out_number_of_KL(K_number_tmp, L_number_tmp);  // K(L)_number_tmpに代入
  K_CRP.set_both_alpha_CRP_customer_number(parameter_co_alpha, K_number_tmp);
  L_CRP.set_both_alpha_CRP_customer_number(parameter_co_alpha, L_number_tmp);
  K_CRP.run_CRP();
  L_CRP.run_CRP();
  K_CRP.get_customer_seating_arrangement(tmp_K);
  L_CRP.get_customer_seating_arrangement(tmp_L);

  IRM.first_set_hidden_K_L(tmp_K, tmp_L);
  IRM.first_get_each_cluster_number();

  IRM.update_hidden_K();
  IRM.decide_update_tmp_or_not_hidden_KL();

  /*
      IRM.Set_Parameter_Relation_Matrix_Beta();
      IRM.decide_Output_Binary_Relation_Matrix();

      // K_CRP.show_customer_datas();
      // L_CRP.show_customer_datas();

      // std::cout << "Generated" << std::endl;
          IRM.Output_by_record_csv();
    */
  IRM.show_IRM_parameter();
  IRM.show_datas();

  return 0;
}