// CRP.cpp
// CRPの過程に従って自動的に客を分割
// 入力:CRPのパラメータalpha,CRPの試行回数(客の人数)
// 出力:各客の座り方, 各机の客の人数
#include <iostream>
#include <random>
#include <vector>
#include "CRP.hpp"

int main(int argc, const char *argv[]) {
  if (argc != 2) {
    std::cout << "This program need  Number of trial" << std::endl;
    return 0;
  }

  int customer_number = atoi(argv[1]);  //コマンドラインからに試行回数を入力
                                        //

  double average_desk = 0;
  int Repeat_number = 1000;  //暫定で決定
  for (auto j = 0; j < Repeat_number; j++) {
    CRP C;
    C.set_CRP_customer_number_alpha_1(customer_number); 
    C.run_CRP();
    C.show_customer_datas();
    average_desk += (double)C.get_desk_number();
  }
  average_desk /= (double)Repeat_number;

  // average_desk=C.CRP_repeat();
  std::cout << "average_desk=" << average_desk << std::endl;

  /*
    CRP C;
    C.get_CRP_parameter();  //手動入力
    C.run_CRP();
    C.show_CRP_parameter();
    C.show_customer_datas();
  */
  return 0;
}
