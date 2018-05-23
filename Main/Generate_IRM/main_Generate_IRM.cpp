// main_Generate_IRM.cpp
// IRMの生成過程に従って自動的にデータを分割
// 入力:IRMのパラメータalpha(最初は共通),比較対象Kの総数(パターン認識本の顧客人数),比較対象Lの総数(パターン認識本の商品の総数)
// 出力:
// KとLの関係を表すK*Lの二次元の二値行列.IRMの学習データ,（必要に応じて比較対象KとLの潜在系列(KとLの要素の各所属クラスタ情報))
#include <iostream>
#include <random>
#include <vector>
#include "CRP.hpp"
#include "Generate_IRM.hpp"

int main(int argc, const char *argv[]) {
  if (argc != 6) {
    std::cout << "This program need alpha+ a + b + Number of K+Number of L"
              << std::endl;
    return 0;
  }

  double parameter_co_alpha = std::atoi(
      argv[1]);  //二つのCRPに共通のパラメータalpha(共通でなくてもよし)
  double parameter_Beta_a = std::atof(argv[2]);
  double parameter_Beta_b = std::atof(argv[3]);
  int K_number = std::atoi(argv[4]);  //比較対象Kの総数
  int L_number = std::atoi(argv[5]);  // 比較対象Lの総数

  std::vector<int> tmp_K;  //クラス間の(CRP→IRM)のvecのやり取りのため.気持ち悪い
  std::vector<int> tmp_L;

  Generate_IRM IRM;

  CRP K_CRP, L_CRP;
  K_CRP.set_both_alpha_CRP_customer_number(parameter_co_alpha, K_number);
  L_CRP.set_both_alpha_CRP_customer_number(parameter_co_alpha, L_number);
  IRM.set_number_of_K_L(K_number, L_number);
  IRM.set_IRM_hyper_parameter(parameter_co_alpha, parameter_Beta_a,
                              parameter_Beta_b);

  K_CRP.run_CRP();
  L_CRP.run_CRP();

  // std::cout << "CRP_END" << std::endl;

  K_CRP.get_customer_seating_arrangement(tmp_K);
  L_CRP.get_customer_seating_arrangement(tmp_L);
  IRM.set_hidden_K_L(tmp_K, tmp_L);
  IRM.get_each_cluster_number();

  IRM.Set_Parameter_Relation_Matrix_Beta();
  IRM.decide_Output_Binary_Relation_Matrix();

  // K_CRP.show_customer_datas();
  // L_CRP.show_customer_datas();

  // std::cout << "Generated" << std::endl;
  IRM.show_IRM_parameter();
  IRM.show_datas();
  IRM.Output_by_record_csv();
  double Posterior_prob;
  Posterior_prob = IRM.get_Posterior_Probability();
  std::cout << "Posterior_prob=" << Posterior_prob << std::endl;
  return 0;
}
