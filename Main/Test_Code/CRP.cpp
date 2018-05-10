// CRP.cpp
// CRPの過程に従って自動的に客を分割
// 入力:CRPのパラメータalpha,CRPの試行回数(客の人数)
// 出力:各客の座り方, 各机の客の人数
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
  CRP() {
    CRP_alpha = 0;
    CRP_customer_number = 0;
    Occupied_desk_number = 1;
    tmp_new_customer_desk = 0;
    customer_seating_arrangement.resize(1, 0);
    desk_seating_arrangement.resize(Occupied_desk_number, 0);
    desk_seating_ratio.resize(Occupied_desk_number + 1, 0);
  }

  void get_CRP_parameter() {
    std::cout << "CRPパラメータ a=";
    std::cin >> CRP_alpha;
    std::cout << "CRPの客の人数=";
    std::cin >> CRP_customer_number;
  }

  void CRP_prrocedure() {  // CRPの本体
    CRP_first_customer();
    std::random_device seed_gen;  //乱数部分はあとでもっとglobalに纏められそう
    std::mt19937 engine(seed_gen());

    for (auto j = 1; j < CRP_customer_number; j++) {
      std::discrete_distribution<std::size_t> disc_dis(  //新しい客の机選択
          desk_seating_ratio.begin(), desk_seating_ratio.end());
      tmp_new_customer_desk = disc_dis(engine);

      CRP_update_seating_arrangement();
      each_CRP_check();
    }
  }

  void CRP_first_customer() {
    customer_seating_arrangement.front() = 1;
    desk_seating_arrangement.front() = 1;
    desk_seating_ratio.front() = 1;
    desk_seating_ratio.back() = CRP_alpha;
    tmp_new_customer_desk =
        1;  //新しい客が座る机の番号-1(配列にはそのまま代入可能)
  }

  void CRP_update_seating_arrangement() {
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

  void each_CRP_check() {  // 各CRPの総和のチェック,食い違えば数字をだす
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

  void show_CRP_parameter() {
    std::cout << "CRPパラメータ" << CRP_alpha << std::endl;
    std::cout << "CRP入店人数" << CRP_customer_number << std::endl;
  }
  void show_customer_datas() {
    std::cout << "vecotorの全要素表示開始" << std::endl;
    for (const auto &i : customer_seating_arrangement) {
      std::cout << "customer_seating_arrangement" << i << std::endl;
    }
    for (const auto &j : desk_seating_ratio) {
      std::cout << "desk_seating_ratio" << j << std::endl;
    }
    for (const auto &k : desk_seating_arrangement) {
      std::cout << "desk_seating_arrangement" << k << std::endl;
    }
    std::cout << "vecotorの全要素表示終了" << std::endl;
  }
};

int main() {
  CRP C;
  C.get_CRP_parameter();
  C.show_CRP_parameter();
  C.show_customer_datas();
  C.CRP_prrocedure();
  C.show_customer_datas();
  return 0;
 }
