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
  void get_customer_seating_arrangement(
      std::vector<int> &Return_vector);  // IRM用
  void set_both_alpha_CRP_customer_number(
      double a, int n);  //他のクラス(IRMなど)で使うパラメータ設定
  void run_CRP();  // CRPの本体
  void CRP_first_customer();
  void CRP_update_seating_arrangement();
  void each_CRP_check();

  void show_CRP_parameter();
  void show_customer_datas();
};

#endif
/*
  double CRP_repeat();    //遺伝研用
  int get_desk_number();  //遺伝研用
  void get_CRP_parameter();


  void set_CRP_customer_number_alpha_1(int n);  //簡単なパラメータ設定
*/
