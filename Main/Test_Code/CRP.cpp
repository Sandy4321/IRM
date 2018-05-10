// CRP.cpp
// CRPの過程に従って自動的に客を分割
#include <chrono>
#include <iostream>
#include <random>
#include <vector>

void OpenVectors(int &a);

int main() {
  int CRP_customer_number;
  double CRP_alpha;

  int Occupied_desk_number = 1;  //一人でも座っている人がいる机の数

  std::cout << "CRPのパラメータを入力" << std::endl;
  std::cin >> CRP_alpha;

  std::cout << "CRPの客の人数を入力" << std::endl;
  std::cin >> CRP_customer_number;

  std::vector<int> customer_seating_arrangement(
      1, 0);  //各客がどの机に座るか(配列長は1から,
              //机のindexは1からスタート)(0で末入店)
  std::vector<int> desk_seating_arrangement(
      Occupied_desk_number,
      0);  //各机に何人座っているか(Occupiedに限定)n_k
  std::vector<double> desk_seating_ratio(
      Occupied_desk_number + 1,
      0);  //次の客が各机に座る確率(比率),最後の要素はCRP_alpha
  // CRPの本体
  //一人目の客は一つ目の机に座る
  customer_seating_arrangement.front() = 1;
  desk_seating_arrangement.front() = 1;
  desk_seating_ratio.front() = 1;
  desk_seating_ratio.back() = CRP_alpha;
  int new_customer_desk;//新しい客が座る机の番号-1(配列にはそのまま代入可能)

//  std::cout << "vecoteの全要素open初期値" << std::endl;
//  for (const auto &i : customer_seating_arrangement)
//    std::cout << "customer_seating_arrangement" << i << std::endl;
//  for (const auto &i : desk_seating_ratio)
//    std::cout << "desk_seating_ratio" << i << std::endl;
//  for (const auto &i : desk_seating_arrangement)
//    std::cout << "desk_seating_arrangement" << i << std::endl;
//  std::cout << "vecoteの全要素終了" << std::endl;


  //次の確率に従って多項分布から座る机を決定
    for (auto j= 1;
         j < CRP_customer_number; j++) {
  //次の客が座る位置の決定

  std::random_device seed_gen;  //呼び出すたびに変える必要あり?
  std::mt19937 engine(seed_gen());  //ここに入れると重そう

  std::discrete_distribution<std::size_t> disc_dis(desk_seating_ratio.begin(),
                                                   desk_seating_ratio.end());

  new_customer_desk = disc_dis(engine);
  std::cout << "new_customer_deskは" << new_customer_desk<<"番目"<< std::endl;

  //それに伴う諸々の変更
  customer_seating_arrangement.push_back(new_customer_desk+1);//机のindexは1スタートなので代入値は+1

  if(new_customer_desk==desk_seating_arrangement.size()){
  //新しい机
  desk_seating_ratio.back()=1;
  desk_seating_ratio.push_back(CRP_alpha);
 desk_seating_arrangement.push_back(1);
 std::cout << "新しい机誕生"<< std::endl;

  }else{
  //既存の机

  //for (const auto &i : desk_seating_ratio)
   // std::cout << "既存desk_seating_ratio更新前" << i << std::endl;
  //for (const auto &i : desk_seating_ratio)
  //  std::cout << "既存desk_seating_arrangement更新前" << i << std::endl;

desk_seating_ratio[new_customer_desk]+=1;
desk_seating_arrangement[new_customer_desk]+=1;

//desk_seating_ratio[0]+=1;
//desk_seating_arrangement[0]+=1;

   std::cout << "既存の机選択"<< std::endl;

  // for (const auto &i : desk_seating_ratio)
  //  std::cout << "既存desk_seating_ratio更新後" << i << std::endl;
  //for (const auto &i : desk_seating_arrangement)
   // std::cout << "既存desk_seating_arrangement更新後" << i << std::endl;
 
  }


  //配置を変更

  //総和があっているか確認
    }

  std::cout << "CRPパラメータ" << CRP_alpha << std::endl;

  std::cout << "CRP入店人数" << CRP_customer_number << std::endl;

  std::cout << "vecoteの全要素open最終値" << std::endl;
  for (const auto &i : customer_seating_arrangement)
    std::cout << "customer_seating_arrangement" << i << std::endl;
  for (const auto &i : desk_seating_ratio)
    std::cout << "desk_seating_ratio" << i << std::endl;
  for (const auto &i : desk_seating_arrangement)
    std::cout << "desk_seating_arrangement" << i << std::endl;
  std::cout << "vecoteの全要素終了" << std::endl;

  // customerと各机のcustomerの総和をとって確認
  return 0;
}
