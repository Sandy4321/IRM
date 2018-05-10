// CRP.cpp
// CRPの過程に従って自動的に客を分割
// 入力:CRPのパラメータalpha,CRPの試行回数(客の人数)
// 出力:各客の座り方, 各机の客の人数
#include <iostream>
#include <random>
#include <vector>

int main() {
  int CRP_customer_number;  // CRPの試行回数(客の人数)
  double CRP_alpha;         // CRPのパラメータalpha

  std::cout << "CRPパラメータ a=";
  std::cin >> CRP_alpha;

  std::cout << "CRPの客の人数=";
  std::cin >> CRP_customer_number;

  std::random_device seed_gen;  //乱数部分はあとでもっとglobalに纏められそう
  std::mt19937 engine(seed_gen());

  int Occupied_desk_number = 1;  //一人でも座っている人がいる机の数
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
  // 1CRP初期化
  customer_seating_arrangement.front() = 1;
  desk_seating_arrangement.front() = 1;
  desk_seating_ratio.front() = 1;
  desk_seating_ratio.back() = CRP_alpha;
  int new_customer_desk;  //新しい客が座る机の番号-1(配列にはそのまま代入可能)

  //次の確率に従って多項分布から座る机を決定
  for (auto j = 1; j < CRP_customer_number; j++) {
    // 2CRP試行を2人目から繰り返す

    std::discrete_distribution<std::size_t> disc_dis(desk_seating_ratio.begin(),
                                                     desk_seating_ratio.end());

    new_customer_desk = disc_dis(engine);
    //  std::cout << "new_customer_deskは" << new_customer_desk<<"番目"<<
    //  std::endl;

    //それに伴う諸々の変更
    customer_seating_arrangement.push_back(
        new_customer_desk + 1);  //机のindexは1スタートなので代入値は+1

    if (new_customer_desk == desk_seating_arrangement.size()) {
      //新しい机
      desk_seating_ratio.back() = 1;
      desk_seating_ratio.push_back(CRP_alpha);
      desk_seating_arrangement.push_back(1);
      // std::cout << "新しい机誕生"<< std::endl;

    } else {
      //既存の机

      // for (const auto &i : desk_seating_ratio)
      // std::cout << "既存desk_seating_ratio更新前" << i << std::endl;
      // for (const auto &i : desk_seating_ratio)
      //  std::cout << "既存desk_seating_arrangement更新前" << i << std::endl;

      desk_seating_ratio[new_customer_desk] += 1;
      desk_seating_arrangement[new_customer_desk] += 1;

      // desk_seating_ratio[0]+=1;
      // desk_seating_arrangement[0]+=1;

      //   std::cout << "既存の机選択"<< std::endl;

      // for (const auto &i : desk_seating_ratio)
      //  std::cout << "既存desk_seating_ratio更新後" << i << std::endl;
      // for (const auto &i : desk_seating_arrangement)
      // std::cout << "既存desk_seating_arrangement更新後" << i << std::endl;
    }

    //配置を変更

    //各CRPの総和があっているか確認例外部分

    if (std::accumulate(desk_seating_arrangement.begin(),
                        desk_seating_arrangement.end(),
                        0) != customer_seating_arrangement.size()) {
      std::cout << "Sum customer is " << customer_seating_arrangement.size()
                << std::endl;
      std::cout << "Sum desk is "
                << std::accumulate(desk_seating_arrangement.begin(),
                                   desk_seating_arrangement.end(), 0)
                << std::endl;
    }
    // else { std::cout <<j <<"Each Sum of desk_seating_arrangement is OK" <<//
    // std::endl; }
  }

  //例外処理部かvector判定する部分
  // CRP_customer_numberrと各机のcustomerの総和が等しいか確認

  if (std::accumulate(desk_seating_arrangement.begin(),
                      desk_seating_arrangement.end(),
                      0) == CRP_customer_number) {
    std::cout << "Sum of desk_seating_arrangement is OK" << std::endl;
  }
  if (std::accumulate(desk_seating_ratio.begin(), desk_seating_ratio.end(), 0) -
          CRP_alpha ==
      CRP_customer_number) {
    std::cout << "Sum of desk_seating_ratio is OK" << std::endl;
  }

  //全てを出力
  // std::cout << "CRPパラメータ" << CRP_alpha << std::endl;
  // std::cout << "CRP入店人数" << CRP_customer_number << std::endl;

  // std::cout << "vecoteの全要素open最終値" << std::endl;
  //  for (const auto &i : customer_seating_arrangement)
  //    std::cout << "customer_seating_arrangement" << i << std::endl;
  // for (const auto &i : desk_seating_ratio)
  //  std::cout << "desk_seating_ratio" << i << std::endl;
  // for (const auto &i : desk_seating_arrangement)
  //  std::cout << "desk_seating_arrangement" << i << std::endl;
  // std::cout << "vecoteの全要素終了" << std::endl;
  std::cout << "Last_total_desk_nuber=" << desk_seating_arrangement.size()
            << std::endl;
  return 0;
}
