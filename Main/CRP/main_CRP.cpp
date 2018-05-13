// CRP.cpp
// CRPの過程に従って自動的に客を分割
// 入力:CRPのパラメータalpha,CRPの試行回数(客の人数)
// 出力:各客の座り方, 各机の客の人数
#include <iostream>
#include <random>
#include <vector>
#include "CRP.hpp"

int main(int argc,const char *argv[]) {
  CRP C;
  C.get_CRP_parameter();
  C.CRP_prrocedure();
  // C.show_CRP_parameter();
  C.show_customer_datas();
  return 0;
}
