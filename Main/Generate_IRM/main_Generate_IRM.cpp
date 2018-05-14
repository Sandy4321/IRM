// Generate_IRM.cpp
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
  if (argc != 4) {
    std::cout << "This program need alpha+ Number of K+Number of L"
              << std::endl;
    return 0;
  }

  double parameter_co_alpha = std::atoi(
      argv[1]);  //二つのCRPに共通のパラメータalpha(共通でなくてもよし)
  int K_number = std::atoi(argv[2]);  //比較対象Kの総数
  int L_number = std::atoi(argv[3]);  // 比較対象Lの総数
                                      // C.get_CRP_parameter();//手動入力
  // C.run_CRP();
  // C.show_CRP_parameter();
  // C.show_customer_datas();
  //std::cout << "parameter_co_alpha" << parameter_co_alpha << std::endl;
  //std::cout << "K_number" << K_number << std::endl;
  //std::cout << "L_number" << L_number << std::endl;

  CRP K_CRP, L_CRP;
  K_CRP.set_both_alpha_CRP_customer_number(parameter_co_alpha, K_number);
  L_CRP.set_both_alpha_CRP_customer_number(parameter_co_alpha, L_number);

  K_CRP.run_CRP();
  L_CRP.run_CRP();



  K_CRP.show_customer_datas();
  L_CRP.show_customer_datas();

  std::cout << "Generated" << std::endl;
  return 0;
}
