#ifndef CRP_H_
#define CRP_H_
// CRP.hpp
// CRPの過程に従って自動的に客を分割
// 入力:CRPのパラメータalpha,CRPの試行回数(客の人数)
// 出力:各客の座り方, 各机の客の人数
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <random>
#include <vector>
class CRP {
 private:
  double CRP_alpha;         // CRPのパラメータalpha
  int CRP_customer_number;  // CRPの試行回数(客の人数)
  int Occupied_desk_number;  //一人でも座っている人がいる机の数 1で初期化
  int tmp_new_customer_desk;  //新しい客が座る机の番号-1(配列にはそのまま代入可能)

  std::vector<int>
      customer_seating_arrangement;  //各客がどの机に座るか(配列長は1から,机のindexは1からスタート,0で末入店)
  std::vector<int>
      desk_seating_arrangement;  //各机に何人座っているか(Occupiedに限定)n_k
  std::vector<double>
      desk_seating_ratio;  //次の客が各机に座る確率(比率),最後の要素はCRP_alpha

 public:
  CRP();
  double CRP_repeat();
  int get_desk_number();

  void set_CRP_parameter(int n);
  void get_CRP_parameter();
  void CRP_prrocedure();  // CRPの本体
  void CRP_first_customer();
  void CRP_update_seating_arrangement();
  void each_CRP_check();

  void show_CRP_parameter();
  void show_customer_datas();
};

#endif
