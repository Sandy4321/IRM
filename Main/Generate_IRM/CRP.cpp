// CRP.cpp
// CRPの過程に従って自動的に客を分割
// 入力:CRPのパラメータalpha,CRPの試行回数(客の人数)
// 出力:各客の座り方, 各机の客の人数
#include <iostream>
#include <random>
#include <vector>
#include "CRP.hpp"

CRP::CRP() {
  CRP_alpha = 0;
  CRP_customer_number = 0;
  Occupied_desk_number = 1;
  tmp_new_customer_desk = 0;
  customer_seating_arrangement.resize(1, 0);
  desk_seating_arrangement.resize(Occupied_desk_number, 0);
  desk_seating_ratio.resize(Occupied_desk_number + 1, 0);
}

void CRP::get_customer_seating_arrangement(
    std::vector<int> &Return_vector) {  // IRMへの応用用,参照返し
  Return_vector = customer_seating_arrangement;
}

double CRP::CRP_repeat() {  // CRPを繰り返す関数，遺伝研用
  double average_desk = 0;
  int Repeat_number = 100;
  for (auto j = 1; j < Repeat_number; j++) {
    CRP::run_CRP();
    CRP::show_customer_datas();
    average_desk += (double)desk_seating_arrangement.size();
  }
  average_desk /= 100;
  return average_desk;
}

int CRP::get_desk_number() {//遺伝研用
  int a;
  a = desk_seating_arrangement.size();
  return a;
}


 void CRP::set_both_alpha_CRP_customer_number(
      double a, int n)  //他のクラス(IRMなど)で使うパラメータ設定
{
  CRP_alpha = a;
  CRP_customer_number = n;
}


void CRP::set_CRP_customer_number_alpha_1(int n) {  //簡単なパラメータ設定(遺伝研用)
  CRP_alpha = 1;
  CRP_customer_number = n;
}

void CRP::get_CRP_parameter() {  //手入力
  std::cout << "CRPパラメータ a=";
  std::cin >> CRP_alpha;
  std::cout << "CRPの客の人数=";
  std::cin >> CRP_customer_number;
}

void CRP::run_CRP() {  // CRPの本体
  CRP::CRP_first_customer();
  std::random_device seed_gen;  //乱数部分はあとでもっとglobalに纏められそう
  std::mt19937 engine(seed_gen());

  for (auto j = 1; j < CRP_customer_number; j++) {
    std::discrete_distribution<std::size_t> disc_dis(  //新しい客の机選択
        desk_seating_ratio.begin(), desk_seating_ratio.end());
    tmp_new_customer_desk = disc_dis(engine);

    CRP::CRP_update_seating_arrangement();
    CRP::each_CRP_check();
  }
}

void CRP::CRP_first_customer() {
  customer_seating_arrangement.front() = 1;
  desk_seating_arrangement.front() = 1;
  desk_seating_ratio.front() = 1;
  desk_seating_ratio.back() = CRP_alpha;
  tmp_new_customer_desk =
      1;  //新しい客が座る机の番号-1(配列にはそのまま代入可能)
}

void CRP::CRP_update_seating_arrangement() {
  customer_seating_arrangement.push_back(
      tmp_new_customer_desk + 1);  //机のindexは1スタートなので代入値は+1

  if (tmp_new_customer_desk == desk_seating_arrangement.size()) {
    //新しい机が選ばれたら
    desk_seating_ratio.back() = 1;
    desk_seating_ratio.push_back(CRP_alpha);
    desk_seating_arrangement.push_back(1);
  } else {
    //既出の机が選ばれたら
    desk_seating_ratio[tmp_new_customer_desk] += 1;
    desk_seating_arrangement[tmp_new_customer_desk] += 1;
  }
}

void CRP::each_CRP_check() {  // 各CRPの総和のチェック,食い違えば数字をだす
  if (std::accumulate(desk_seating_arrangement.begin(),
                      desk_seating_arrangement.end(),
                      0) != customer_seating_arrangement.size()) {
    std::cout << "Attention!!" << std::endl;
    std::cout << "Sum customer is " << customer_seating_arrangement.size()
              << std::endl;
    std::cout << "Sum desk is "
              << std::accumulate(desk_seating_arrangement.begin(),
                                 desk_seating_arrangement.end(), 0)
              << std::endl;
  }
  // else { std::cout <<j <<"Each Sum of desk_seating_arrangement is OK"
  // <<// std::endl; }
}

void CRP::show_CRP_parameter() {
  std::cout << "CRPパラメータ" << CRP_alpha << std::endl;
  std::cout << "CRP入店人数" << CRP_customer_number << std::endl;
}

void CRP::show_customer_datas() {
  std::cout << "座席配置の全要素表示開始" << std::endl;
  /*    for (const auto &i : customer_seating_arrangement) {
        std::cout << "customer_seating_arrangement" << i << std::endl;
      }
  */

  /*    for (const auto &i : desk_seating_ratio) {
        std::cout << "desk_seating_ratio" << i << std::endl;
      }
  */
  for (const auto &i : desk_seating_arrangement) {
    std::cout << "desk_seating_arrangement" << i << std::endl;
  }

  std::cout << "total_desk_" << desk_seating_arrangement.size() << std::endl;
  std::cout << "座席配置の全要素表示終了" << std::endl;
}
