// IRM_Co_clustering.cpp
// IRM_Co_Clusteringの過程に従って自動的に客を分割
// 入力:IRM_Co_Clusteringのパラメータalpha,IRM_Co_Clusteringの試行回数(客の人数)
// 出力:各客の座り方, 各机の客の人数
#include <boost/foreach.hpp>
#include <boost/math/special_functions/beta.hpp>
#include <boost/math/special_functions/factorials.hpp>
#include <boost/random.hpp>
#include <boost/tokenizer.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>
#include "CRP.hpp"
#include "IRM_Co_clustering.hpp"

IRM_Co_Clustering::IRM_Co_Clustering() {
  gibbs_counter = 0;
  stop_counter = 0;
  not_updated_counter = 0;
  k_iterator = 0;
  l_iterator = 0;
  IRM_Co_Clustering_co_alpha = 0;
  IRM_Co_Clustering_Beta_a = 0;
  IRM_Co_Clustering_Beta_b = 0;
  number_of_K = 0;  //比較対象Kの総数,パターン認識本の例の顧客数
  number_of_L = 0;  //比較対象Lの総数,パターン認識本の例の商品数
  number_of_cluster_K = 0;  //比較対象Kの総クラスタ数
  number_of_cluster_L = 0;  //比較対象Lの総クラスタ数

  tmp_number_of_cluster_K = 0;  //比較対象Kの総クラスタ数
  tmp_number_of_cluster_L = 0;  //比較対象Lの総クラスタ数

  Max_Posterior_Probability = std::log(0);
  tmp_Posterior_Probability = std::log(0);

  hidden_K.resize(0, 0);
  hidden_L.resize(0, 0);

  tmp_hidden_K.resize(0, 0);
  tmp_hidden_L.resize(0, 0);

  number_of_k_in_each_cluster.resize(number_of_cluster_K, 0);
  number_of_l_in_each_cluster.resize(number_of_cluster_L, 0);

  tmp_number_of_k_in_each_cluster.resize(number_of_cluster_K, 0);
  tmp_number_of_l_in_each_cluster.resize(number_of_cluster_L, 0);

  Input_Binary_Relation_k.resize(
      0, 0);  // 比較対象Kのk番目の要素とLの関係を表すL長vector
  Input_Binary_Relation_Matrix.resize(
      0, Input_Binary_Relation_k);  //最初は空っぽで

  Parameter_Relation_k.resize(0, 0);

  Parameter_Relation_Matrix.resize(
      0,
      Parameter_Relation_k);  // A(boost::extents[10][10]);

  Trantsition_Posterior_distriibution.resize(0, 0);
}

void IRM_Co_Clustering::set_gibbs_counter(unsigned int i) { gibbs_counter = i; }
void IRM_Co_Clustering::set_stop_counter(unsigned int i) { stop_counter = i; }

void IRM_Co_Clustering::Read_csv_Input_Binary_Relation_Matrix(  // csv読み込み
    const char *argv1) {
  typedef boost::tokenizer<boost::escaped_list_separator<char>>
      BOOST_TOKENIZER_ESCAPED_LIST;

  std::ifstream ifs_Read;
  std::string csv_file_path(argv1);

  try {
    ifs_Read.open(csv_file_path.c_str());
    if (!ifs_Read) {
      std::cout << "I Can not open file1" << std::endl;
      //終了処理
    }

    std::string Each_line;

    while (getline(ifs_Read, Each_line)) {
      BOOST_TOKENIZER_ESCAPED_LIST after_tokens1(Each_line);

      std::vector<int> tmp_horizontal_reading_data;

      BOOST_FOREACH (std::string read_result, after_tokens1) {
        //  std::cout << "<" << read_result << "> ";
        tmp_horizontal_reading_data.push_back(std::stoi(read_result));
      }
      // std::cout << std::endl;
      Input_Binary_Relation_Matrix.push_back(tmp_horizontal_reading_data);  //
    }
    ifs_Read.close();
  } catch (std::exception &ex) {
    std::cerr << ex.what() << std::endl;
    std::cout << "読み込みに失敗しました" << std::endl;
  }
}

void IRM_Co_Clustering::set_number_of_K_L() {
  if (Input_Binary_Relation_Matrix.back().size() !=
      Input_Binary_Relation_Matrix.front().size()) {
    std::cout << "Attention!!!行の長さが列によって食い違った不正なデータです."
              << std::endl;
    std::cout << "number_of_K=" << Input_Binary_Relation_Matrix.size()
              << std::endl;

    std::cout << "number_of_L最初="
              << Input_Binary_Relation_Matrix.back().size() << std::endl;
    std::cout << "number_of_L最後="
              << Input_Binary_Relation_Matrix.front().size() << std::endl;
  }
  number_of_K = Input_Binary_Relation_Matrix.size();
  number_of_L = Input_Binary_Relation_Matrix.back().size();
}

void IRM_Co_Clustering::out_number_of_KL(
    int &tmp_K, int &tmp_L) {  //参照渡さないと変更しないぞ
  tmp_K = number_of_K;
  tmp_L = number_of_L;
  // std::cout << "tmp_K" << tmp_K << std::endl;
}

void IRM_Co_Clustering::set_IRM_hyper_parameter(double alpha, double a,
                                                double b) {
  IRM_Co_Clustering_co_alpha = alpha;
  IRM_Co_Clustering_Beta_a = a;
  IRM_Co_Clustering_Beta_b = b;
}

void IRM_Co_Clustering::first_set_hidden_K_L(std::vector<int> &K_vector,
                                             std::vector<int> &L_vector) {
  hidden_K.resize(K_vector.size());
  hidden_L.resize(L_vector.size());

  tmp_hidden_K.resize(K_vector.size());
  tmp_hidden_L.resize(L_vector.size());

  std::copy(K_vector.begin(), K_vector.end(), hidden_K.begin());
  std::copy(L_vector.begin(), L_vector.end(), hidden_L.begin());
  std::copy(K_vector.begin(), K_vector.end(), tmp_hidden_K.begin());
  std::copy(L_vector.begin(), L_vector.end(), tmp_hidden_L.begin());
}

void IRM_Co_Clustering::first_get_each_cluster_number() {
  number_of_k_in_each_cluster.resize(0, 0);  //初期化
  number_of_k_in_each_cluster.shrink_to_fit();

  int find_number_in_k = 1;
  while (1) {
    auto itr_k = std::find(hidden_K.begin(), hidden_K.end(), find_number_in_k);
    if (itr_k != hidden_K.end()) {  // find_nuber_in_k を発見した場合
      number_of_k_in_each_cluster.push_back(
          std::count(hidden_K.begin(), hidden_K.end(), find_number_in_k));
      find_number_in_k++;

      /*      if (find_number_in_k > 100) {  //whileのバグの場合の強制終了
              std::cout << "Toomuch" << std::endl;
              break;
            }
      */

    } else {
      break;
    }
  }
  number_of_cluster_K = number_of_k_in_each_cluster.size();  //クラスタ数も更新

  number_of_l_in_each_cluster.resize(0, 0);  // hidden_Lにもにも同様の操作
  number_of_l_in_each_cluster.shrink_to_fit();

  int find_number_in_l = 1;  // 今回はCRPの机なので検索対象は1スタート
  while (1) {
    auto itr = std::find(hidden_L.begin(), hidden_L.end(), find_number_in_l);
    if (itr != hidden_L.end()) {
      number_of_l_in_each_cluster.push_back(
          std::count(hidden_L.begin(), hidden_L.end(), find_number_in_l));
      find_number_in_l++;
      /*  if (find_number_in_l > 1000) {  // whileのバグの場合の強制終了
                std::cout << "Too much L" << std::endl;
                break;
             }
      */

    } else {
      break;
    }
  }
  number_of_cluster_L = number_of_l_in_each_cluster.size();

  //  std::cout << "EndCount" << std::endl;
  // tmp系も同様に初期化
  tmp_number_of_k_in_each_cluster.resize(number_of_k_in_each_cluster.size());
  tmp_number_of_l_in_each_cluster.resize(number_of_l_in_each_cluster.size());

  std::copy(number_of_k_in_each_cluster.begin(),
            number_of_k_in_each_cluster.end(),
            tmp_number_of_k_in_each_cluster.begin());
  std::copy(number_of_l_in_each_cluster.begin(),
            number_of_l_in_each_cluster.end(),
            tmp_number_of_l_in_each_cluster.begin());
  tmp_number_of_cluster_K = tmp_number_of_k_in_each_cluster.size();
  tmp_number_of_cluster_L = tmp_number_of_l_in_each_cluster.size();
}
void IRM_Co_Clustering::tmp_hidden_K_get_each_cluster_number() {
  tmp_number_of_k_in_each_cluster.resize(0, 0);  //初期化
  tmp_number_of_k_in_each_cluster.shrink_to_fit();

  int find_number_in_k = 1;
  while (1) {
    auto itr_k =
        std::find(tmp_hidden_K.begin(), tmp_hidden_K.end(), find_number_in_k);
    if (itr_k != tmp_hidden_K.end()) {  // find_nuber_in_k を発見した場合
      tmp_number_of_k_in_each_cluster.push_back(std::count(
          tmp_hidden_K.begin(), tmp_hidden_K.end(), find_number_in_k));
      find_number_in_k++;

      /*      if (find_number_in_k > 100) {  //whileのバグの場合の強制終了
              std::cout << "Toomuch" << std::endl;
              break;
            }
      */

    } else {
      break;
    }
  }
  tmp_number_of_cluster_K =
      tmp_number_of_k_in_each_cluster.size();  //クラスタ総数も更新
}

void IRM_Co_Clustering::update_hidden_K() {
  /*
    for (const auto &i : tmp_hidden_K) {
      std::cout << "オリジナルのtmp_hidden_K:" << i << std::endl;
    }
    for (const auto &i : tmp_number_of_k_in_each_cluster) {
      std::cout << "オリジナルのtmp_number_of_k_in_each_cluster:" << i
                << std::endl;
    }
  */
  for (unsigned int k = 0; k < hidden_K.size(); k++) {
    // tmp_hidden_Kからk番目を一つ削除
    k_iterator = k;
    unsigned int tmp_delete_atom_k = 0;
    tmp_delete_atom_k = tmp_hidden_K[k];

    tmp_hidden_K.erase(tmp_hidden_K.begin() + k);
    tmp_hidden_K.shrink_to_fit();
    //諸々の更新(クラスタ数0になるものがあるかどうか)
    //	tmp_delete_atom_kをerase済みtmp_hidden_Kから探す

    auto itr_k_search =
        std::find(tmp_hidden_K.begin(), tmp_hidden_K.end(), tmp_delete_atom_k);
    if (itr_k_search ==
        tmp_hidden_K.end()) {  // tmp_delete_atom_k
                               // を発見できなかった場合＝tmp_hidden_Kの更新
      /*
           std::cout << "tmp_hidden_Kクラスタアップデート" << k
                     << "人目の値(添字なので人なら+1)" << std::endl;
           std::cout << "それは値" << tmp_delete_atom_k << "より上" <<
         std::endl; for (const auto &i : tmp_hidden_K) { std::cout <<
         "アップデート前のtmp_hidden_K:" << i << std::endl;
           }
     */
      for (auto &i : tmp_hidden_K) {
        if (i > tmp_delete_atom_k) {
          i -= 1;
        }
      }

      /*
            std::cout << "そして" << std::endl;

            for (const auto &i : tmp_hidden_K) {
              std::cout << "アップデート後の時の各tmp_hidden_K:" << i <<
         std::endl;
            }
      */
    }
    //改めてクラスター数を設定

    tmp_hidden_K_get_each_cluster_number();
    /*
        for (const auto &i : tmp_hidden_K) {
          std::cout << k << "番目の処理のtmp_hidden_K:" << i << std::endl;
        }
        for (const auto &i : tmp_number_of_k_in_each_cluster) {
          std::cout << k << "番目の処理の各number_of_k_in_each_cluster:" << i
                    << std::endl;
        }
        std::cout << std::endl;
    */

    double chosed_new_atom = 0;
    chosed_new_atom = get_new_cluster_for_K();
    /*
std::cout << "クラスタ選択chosed_new_atom=" << chosed_new_atom << std::endl;
if (chosed_new_atom == tmp_number_of_k_in_each_cluster.size() + 1) {
  std::cout << "新しいクラスタが選ばれた"
            << std::endl;  //新しいクラスタが選ばれたら,,特に処理はなし??
}
*/
    tmp_delete_atom_k = chosed_new_atom;

    // tmp_hidden_Kにtmp_delete_atom_を戻す
    tmp_hidden_K.insert(tmp_hidden_K.begin() + k,
                        tmp_delete_atom_k);  // tmp_delete_atom_kは変更する**

    tmp_hidden_K_get_each_cluster_number();
    /*
        for (const auto &i : tmp_hidden_K) {
          std::cout << k << "要素の更新後のtmp_hidden_K:" << i << std::endl;
        }
        for (const auto &i : tmp_number_of_k_in_each_cluster) {
          std::cout << k << "要素の更新後のtmp_number_of_k_in_each_cluster:" <<
       i
                    << std::endl;
        }
            */
  }
  //更新繰り返しは上まで
  // 長さが同じか確認
  if (tmp_hidden_K.size() != hidden_K.size()) {
    std::cout
        << "Error!!tmp_hidden_Kとhidden_Kの長さが違う,tmp_hidden_Kの更新失敗";
  }
}

int IRM_Co_Clustering::
    get_new_cluster_for_K() {  //各クラスタを選ぶ確率を計算.この実装がかなり難しい
  std::random_device seed_gen;  //乱数部分はあとでもっとglobalに纏められそう
  std::mt19937 engine(seed_gen());  //この二行はあとでfor分の外にだす

  std::vector<double> probability_ratio;

  for (unsigned int i = 0; i < tmp_number_of_k_in_each_cluster.size();
       i++) {  //既存のクラスタを選択する確率
    int iter_i = i;
    probability_ratio.push_back(already_cluster_prob_K(iter_i));
  }
  probability_ratio.push_back(
      new_cluster_prob_K());  //新しいクラスタを選択する確率

  // k番目の要素の最適なクラスターを選択
  // 新しい客の机を選択
  std::discrete_distribution<std::size_t> cluster_dis(probability_ratio.begin(),
                                                      probability_ratio.end());
  /*
    for (const auto &i : probability_ratio) {
      std::cout << "各確率は:" << i << std::endl;
    }
  */
  int chosed = 0;
  chosed = cluster_dis(engine);
  // std::cout << "chosed" << chosed << std::endl;
  return chosed + 1;  //返す値は0スタートだけど机は1スタートなので
}

double IRM_Co_Clustering::already_cluster_prob_K(
    unsigned int i_cluster_of_K) {  // i番目の顧客クラスターの確率について調べる

  double already_prob_K = 1;

  for (unsigned int j_cluster_of_L = 0;
       j_cluster_of_L < tmp_number_of_cluster_L;
       j_cluster_of_L++) {  // Lのクラスタ数の数だけ積を実施

    unsigned int n_full_full_i_j = 0;
    unsigned int bar_n_full_full_i_j = 0;
    unsigned int n_notk_full_i_j = 0;
    unsigned int bar_n_notk_full_i_j = 0;

    unsigned int n_k_full_i_j = 0;      //今回は補助変数
    unsigned int bar_n_k_full_i_j = 0;  //今回は補助変数
    for (unsigned int k = 0; k < Input_Binary_Relation_Matrix.size();
         k++) {  //クラスタjについてカウント
      for (
          unsigned int l = 0;
          l < Input_Binary_Relation_Matrix[k_iterator].size();  // k行を調べる
          l++) {  // tmp_hidden_Kは長さがK-1長なので場合分けの必要(tmp_hidden_Kのアップデート中のため)//tmp_hidden_LはL長
        if (k < k_iterator) {
          if (tmp_hidden_K[k] == i_cluster_of_K + 1) {
            // //デバッグ対象その1

            if (tmp_hidden_L[l] == j_cluster_of_L + 1) {
              if (Input_Binary_Relation_Matrix[k_iterator][l] ==
                  1) {  // Relation_Matrixの値が1かどうか
                n_full_full_i_j += 1;
                n_notk_full_i_j += 1;
              } else {
                bar_n_full_full_i_j += 1;
                bar_n_notk_full_i_j += 1;
              }
            }
          }
        } else if (k == k_iterator) {  // k行目について調べるとき
          if (tmp_hidden_L[l] ==
              j_cluster_of_L + 1) {  //要素lがクラスタjに属しているかどうか
            if (Input_Binary_Relation_Matrix[k_iterator][l] ==
                1) {  // Relation_Matrixの値が1かどうか
              n_k_full_i_j += 1;
              n_full_full_i_j += 1;

            } else {
              bar_n_k_full_i_j += 1;
              bar_n_full_full_i_j += 1;
            }
          }

        } else if (k > k_iterator) {
          if (tmp_hidden_K[k - 1] == i_cluster_of_K + 1) {
            // //デバッグ対象その1

            if (tmp_hidden_L[l] == j_cluster_of_L + 1) {
              if (Input_Binary_Relation_Matrix[k_iterator][l] ==
                  1) {  // Relation_Matrixの値が1かどうか
                n_full_full_i_j += 1;
                n_notk_full_i_j += 1;

              } else {
                bar_n_full_full_i_j += 1;
                bar_n_notk_full_i_j += 1;
              }
            }
          }
        }
      }
    }



	/*
already_prob_K+=(std::lgamma(IRM_Co_Clustering_Beta_a+n_full_full_i_j)+std::lgamma(IRM_Co_Clustering_Beta_b)-std::lgammma())-(std::lgamma()+std::lgamma()-std::lgammma())
*/
    already_prob_K *=
        ((double)tmp_number_of_l_in_each_cluster[j_cluster_of_L] *
         (boost::math::beta(IRM_Co_Clustering_Beta_a + n_full_full_i_j,
                            IRM_Co_Clustering_Beta_b + bar_n_full_full_i_j)) /
         (boost::math::beta(IRM_Co_Clustering_Beta_a + n_notk_full_i_j,
                            IRM_Co_Clustering_Beta_b + bar_n_notk_full_i_j)));

  }

  // already_prob_K = 1;
  ///テスト用

  return already_prob_K;
}
double IRM_Co_Clustering::new_cluster_prob_K() {
  double new_prob = 1;

  for (unsigned int j_cluster_of_L = 0;
       j_cluster_of_L < tmp_number_of_cluster_L;
       j_cluster_of_L++) {  //クラスタ数の数だけ実施
    int n_k_full_i_j = 0;
    int bar_n_k_full_i_j = 0;
    for (unsigned int l = 0;
         l < Input_Binary_Relation_Matrix[k_iterator].size();  // k行を調べる
         l++) {
      if (tmp_hidden_L[l] ==
          j_cluster_of_L + 1) {  //要素lがクラスタjに属しているかどうか
        if (Input_Binary_Relation_Matrix[k_iterator][l] ==
            1) {  // Relation_Matrixの値が1かどうか
          n_k_full_i_j += 1;
        } else {
          bar_n_k_full_i_j += 1;
        }
      }
    }

    new_prob *=  //
        (IRM_Co_Clustering_co_alpha *
         (boost::math::beta(IRM_Co_Clustering_Beta_a + n_k_full_i_j,
                            IRM_Co_Clustering_Beta_b + bar_n_k_full_i_j)) /
         (boost::math::beta(IRM_Co_Clustering_Beta_a,
                            IRM_Co_Clustering_Beta_b)));
  }

  // new_prob *= IRM_Co_Clustering_co_alpha;
  //テスト用
  return new_prob;
}

//ここからLの処理
void IRM_Co_Clustering::tmp_hidden_L_get_each_cluster_number() {
  tmp_number_of_l_in_each_cluster.resize(0, 0);  //初期化
  tmp_number_of_l_in_each_cluster.shrink_to_fit();

  int find_number_in_l = 1;
  while (1) {
    auto itr_l =
        std::find(tmp_hidden_L.begin(), tmp_hidden_L.end(), find_number_in_l);
    if (itr_l != tmp_hidden_L.end()) {  // find_nuber_in_l を発見した場合
      tmp_number_of_l_in_each_cluster.push_back(std::count(
          tmp_hidden_L.begin(), tmp_hidden_L.end(), find_number_in_l));
      find_number_in_l++;

      /*      if (find_number_in_l > 100) {  //whileのバグの場合の強制終了
              std::cout << "Toomuch" << std::endl;
              break;
            }
      */

    } else {
      break;
    }
  }
  tmp_number_of_cluster_L =
      tmp_number_of_l_in_each_cluster.size();  //クラスタ総数も更新
}

void IRM_Co_Clustering::update_hidden_L() {
  // tmp_hidden_Lからl番目を一つ削除
  //諸々の更新
  // l番目に最適なクラスターを選択して決定
  // tmp_hidden_Lに戻す
  // 長さが同じか確認
  //
  for (unsigned int l = 0; l < hidden_L.size(); l++) {
    // tmp_hidden_Kからk番目を一つ削除
    l_iterator = l;
    unsigned int tmp_delete_atom_l = 0;
    tmp_delete_atom_l = tmp_hidden_L[l_iterator];

    tmp_hidden_L.erase(tmp_hidden_L.begin() + l);
    tmp_hidden_L.shrink_to_fit();
    //諸々の更新(クラスタ数0になるものがあるかどうか)
    //	tmp_delete_atom_kをerase済みtmp_hidden_Kから探す

    auto itr_l_search =
        std::find(tmp_hidden_L.begin(), tmp_hidden_L.end(), tmp_delete_atom_l);
    if (itr_l_search ==
        tmp_hidden_L.end()) {  // tmp_delete_atom_l
                               // を発見できなかった場合＝tmp_hidden_Lの更新

      for (auto &i : tmp_hidden_L) {
        if (i > tmp_delete_atom_l) {
          i -= 1;
        }
      }
    }
    //改めてクラスター数を設定

    tmp_hidden_L_get_each_cluster_number();
    double chosed_new_atom = 0;
    chosed_new_atom = get_new_cluster_for_L();
    tmp_delete_atom_l = chosed_new_atom;

    // tmp_hidden_Kにtmp_delete_atom_を戻す
    tmp_hidden_L.insert(tmp_hidden_L.begin() + l,
                        tmp_delete_atom_l);  // tmp_delete_atom_kは変更する**

    tmp_hidden_L_get_each_cluster_number();
  }

  // 長さが同じか確認
  if (tmp_hidden_L.size() != hidden_L.size()) {
    std::cout
        << "Error!!tmp_hidden_Kとhidden_Kの長さが違う,tmp_hidden_Kの更新失敗";
  }
}

int IRM_Co_Clustering::
    get_new_cluster_for_L() {  //各クラスタを選ぶ確率を計算.この実装がかなり難しい
  std::random_device seed_gen;  //乱数部分はあとでもっとglobalに纏められそう
  std::mt19937 engine(seed_gen());  //この二行はあとでfor分の外にだす

  std::vector<double> probability_ratio;

  for (unsigned int j = 0; j < tmp_number_of_l_in_each_cluster.size();
       j++) {  //既存のクラスタを選択する確率
    int iter_j = j;
    probability_ratio.push_back(already_cluster_prob_L(iter_j));
  }
  probability_ratio.push_back(
      new_cluster_prob_L());  //新しいクラスタを選択する確率

  /*
  for (const auto &i : probability_ratio) {
    std::cout << "Lの各確率は:" << i << std::endl;
  }
*/
  int chosed = 0;
  // l番目の要素の最適なクラスターを選択
  std::discrete_distribution<std::size_t> cluster_dis_l(
      probability_ratio.begin(), probability_ratio.end());
  chosed = cluster_dis_l(engine);
  // std::cout << "chosed" << chosed << std::endl;
  return chosed + 1;  //返す値は0スタートだけど机は1スタートなので

  //  return cluster_dis(engine) + 1;
  //  //返す値は0スタートだけど机は1スタートなので
}

double IRM_Co_Clustering::already_cluster_prob_L(
    unsigned int j_cluster_of_L) {  // j番目の商品クラスターについて調べる
  double already_prob_L = 1;
  for (unsigned int i_cluster_of_K = 0;
       i_cluster_of_K < tmp_number_of_cluster_K;
       i_cluster_of_K++) {  // Lのクラスタ数の数だけ積を実施

    unsigned int n_full_full_i_j = 0;
    unsigned int bar_n_full_full_i_j = 0;
    unsigned int n_notl_full_i_j = 0;
    unsigned int bar_n_notl_full_i_j = 0;

    unsigned int n_l_full_i_j = 0;      //今回は補助変数
    unsigned int bar_n_l_full_i_j = 0;  //今回は補助変数
    for (unsigned int l = 0;
         l < Input_Binary_Relation_Matrix[l_iterator].size();
         l++) {  //クラスタjについてカウント
      for (
          unsigned int k = 0;
          k < Input_Binary_Relation_Matrix.size();  // l列を調べる
          k++) {  // tmp_hidden_Lは長さがL-1長なので場合分けの必要(tmp_hidden_Lのアップデート中のため)//tmp_hidden_KはK長
        if (l < l_iterator) {
          if (tmp_hidden_L[l] == j_cluster_of_L + 1) {
            // //デバッグ対象その1

            if (tmp_hidden_K[k] == i_cluster_of_K + 1) {
              if (Input_Binary_Relation_Matrix[k][l_iterator] ==
                  1) {  // Relation_Matrixの値が1かどうか
                n_full_full_i_j += 1;
                n_notl_full_i_j += 1;
              } else {
                bar_n_full_full_i_j += 1;
                bar_n_notl_full_i_j += 1;
              }
            }
          }
        } else if (l == l_iterator) {  // k行目について調べるとき
          if (tmp_hidden_K[k] ==
              i_cluster_of_K + 1) {  //要素lがクラスタjに属しているかどうか
            if (Input_Binary_Relation_Matrix[k][l_iterator] ==
                1) {  // Relation_Matrixの値が1かどうか
              n_l_full_i_j += 1;
              n_full_full_i_j += 1;

            } else {
              bar_n_l_full_i_j += 1;
              bar_n_full_full_i_j += 1;
            }
          }

        } else if (l > l_iterator) {
          if (tmp_hidden_L[l - 1] == j_cluster_of_L + 1) {
            // //デバッグ対象その1

            if (tmp_hidden_K[k] == i_cluster_of_K + 1) {
              if (Input_Binary_Relation_Matrix[k][l_iterator] ==
                  1) {  // Relation_Matrixの値が1かどうか
                n_full_full_i_j += 1;
                n_notl_full_i_j += 1;

              } else {
                bar_n_full_full_i_j += 1;
                bar_n_notl_full_i_j += 1;
              }
            }
          }
        }
      }
    }
    already_prob_L *=
        ((double)tmp_number_of_k_in_each_cluster[i_cluster_of_K] *
         (boost::math::beta(IRM_Co_Clustering_Beta_a + n_full_full_i_j,
                            IRM_Co_Clustering_Beta_b + bar_n_full_full_i_j)) /
         (boost::math::beta(IRM_Co_Clustering_Beta_a + n_notl_full_i_j,
                            IRM_Co_Clustering_Beta_b + bar_n_notl_full_i_j)));
  }

  return already_prob_L;
}

double IRM_Co_Clustering::new_cluster_prob_L() {
  double new_prob = 1;

  for (unsigned int i_cluster_of_K = 0;
       i_cluster_of_K < tmp_number_of_cluster_K; i_cluster_of_K++) {
    //クラスタ数の数だけ実施
    int n_l_full_i_j = 0;
    int bar_n_l_full_i_j = 0;
    for (unsigned int k = 0;
         k < Input_Binary_Relation_Matrix.size();  // k行を調べる
         k++) {
      if (tmp_hidden_K[k] == i_cluster_of_K + 1) {
        //要素lがクラスタjに属しているかどうか
        if (Input_Binary_Relation_Matrix[k][l_iterator] ==
            1) {  // Relation_Matrixの値が1かどうか
          n_l_full_i_j += 1;
        } else {
          bar_n_l_full_i_j += 1;
        }
      }
    }

    new_prob *=  //
        IRM_Co_Clustering_co_alpha *
        ((boost::math::beta(IRM_Co_Clustering_Beta_a + n_l_full_i_j,
                            IRM_Co_Clustering_Beta_b + bar_n_l_full_i_j)) /
         (boost::math::beta(IRM_Co_Clustering_Beta_a,
                            IRM_Co_Clustering_Beta_b)));
  }

  // new_prob *= IRM_Co_Clustering_co_alpha;  //テスト用

  return new_prob;
}

//完成したtmp_K_hiddenとtmp_hidden_Lを用いて事後確率計算
// tmp_hidden_Kにhidden_Kをアップデートするか判定して更新かそのままにする
int IRM_Co_Clustering::decide_update_tmp_or_not_hidden_KL() {
  if (tmp_hidden_K.size() != hidden_K.size()) {  //更新そもそもして大丈夫か確認
    std::cout << "Error!!tmp_hidden_Kとhidden_Kの長さが違う";
  }
  if (tmp_hidden_L.size() != hidden_L.size()) {
    std::cout << "Error!!tmp_hidden_Lとhidden_Lの長さが違う";
  }
  tmp_Posterior_Probability = get_tmp_Posterior_Probability();

  if (tmp_Posterior_Probability > Max_Posterior_Probability) {
    // hidden_Kとhidden_Lの更新処理
    Max_Posterior_Probability = tmp_Posterior_Probability;
    std::copy(tmp_hidden_K.begin(), tmp_hidden_K.end(), hidden_K.begin());
    std::copy(tmp_hidden_L.begin(), tmp_hidden_L.end(), hidden_L.begin());
    first_get_each_cluster_number();  //クラスタ関連の数を初期化して更新*****要check
    //  std::cout << "hidden_KL is Updated!!Max_Posterior_Probability="
    //           << Max_Posterior_Probability << std::endl;
    not_updated_counter = 0;
  } else if (tmp_Posterior_Probability == Max_Posterior_Probability) {
    not_updated_counter += 1;

  } else {
    not_updated_counter += 1;

    //  std::cout << "hidden_K_L is Not_Updated!!Max_Posterior_Probability="
    //           << Max_Posterior_Probability << std::endl;
  }
  //続行するか終わるかの処理
  Trantsition_Posterior_distriibution.push_back(Max_Posterior_Probability);
  if (not_updated_counter > stop_counter) {
    return 0;
  }

  return 1;  //継続では1を返す
}

double IRM_Co_Clustering::get_tmp_Posterior_Probability() {
  double Posterior_Probability = 0;
  double S1_Posterior_Probability = 0;

  double S2_Posterior_Probability = 0;
  double S3_Posterior_Probability = 0;

  /* S1 = std::pow(IRM_Co_Clustering_co_alpha,
                 tmp_number_of_k_in_each_cluster.size());
   S1_Posterior_Probability = std::log(S1);
   */
  S1_Posterior_Probability =
      tmp_number_of_k_in_each_cluster.size() *
          (std::log(IRM_Co_Clustering_co_alpha)) -
      (Logfactorial(tmp_number_of_k_in_each_cluster.size()));

  for (unsigned int i = 0; i < tmp_number_of_k_in_each_cluster.size(); i++) {
    S1_Posterior_Probability +=
        Logfactorial(tmp_number_of_k_in_each_cluster[i] - 1);
  }
  for (unsigned int i = 0; i < tmp_hidden_K.size(); i++) {
    S1_Posterior_Probability -= std::log(IRM_Co_Clustering_co_alpha + i);
  }

  S2_Posterior_Probability =
      tmp_number_of_l_in_each_cluster.size() *
          (std::log(IRM_Co_Clustering_co_alpha)) -
      (Logfactorial(tmp_number_of_l_in_each_cluster.size()));

  for (unsigned int j = 0; j < tmp_number_of_l_in_each_cluster.size(); j++) {
    S2_Posterior_Probability +=
        Logfactorial(tmp_number_of_l_in_each_cluster[j] - 1);
  }
  for (unsigned int j = 0; j < tmp_hidden_L.size(); j++) {
    S2_Posterior_Probability -= std::log(IRM_Co_Clustering_co_alpha + j);
  }

  for (unsigned int i = 0; i < tmp_number_of_k_in_each_cluster.size(); i++) {
    for (unsigned int j = 0; j < tmp_hidden_L.size(); j++) {
      double n_full_full_i_j = 0;
      double bar_n_full_full_i_j = 0;

      for (unsigned int k = 0; k < Input_Binary_Relation_Matrix.size();
           k++) {  //クラスタjについてカウント
        for (unsigned int l = 0;
             l < Input_Binary_Relation_Matrix[k].size();  // k行を調べる
             l++) {
          if (tmp_hidden_K[k] == i + 1) {
            if (tmp_hidden_L[l] == j + 1) {
              // std::cout << "tmp_hidden_L[l] == j+1) at" << tmp_hidden_L[l]
              //         << std::endl;

              if (Input_Binary_Relation_Matrix[k][l] ==
                  1) {  // Relation_Matrixの値が1かどうか
                n_full_full_i_j += 1;
              } else {
                bar_n_full_full_i_j += 1;
              }
            }
          }
        }
      }
      // std::cout << "n_full_full_i_j" << n_full_full_i_j << std::endl;

      S3_Posterior_Probability +=
          (std::lgamma(IRM_Co_Clustering_Beta_a + n_full_full_i_j) +
           std::lgamma(IRM_Co_Clustering_Beta_b + bar_n_full_full_i_j) -
           std::lgamma(IRM_Co_Clustering_Beta_a + n_full_full_i_j +
                       IRM_Co_Clustering_Beta_b + bar_n_full_full_i_j)) -
          (std::lgamma(IRM_Co_Clustering_Beta_a) +
           std::lgamma(IRM_Co_Clustering_Beta_b) -
           std::lgamma(IRM_Co_Clustering_Beta_a + IRM_Co_Clustering_Beta_b));

      /*
            S3_Posterior_Probability *=
                (boost::math::beta(IRM_Co_Clustering_Beta_a + n_full_full_i_j,
                                   IRM_Co_Clustering_Beta_b +
         bar_n_full_full_i_j)) / (boost::math::beta(IRM_Co_Clustering_Beta_a,
                                   IRM_Co_Clustering_Beta_b));
      */
    }
  }
  /*
    std::cout << "S1_Posterior_Probability=" << S1_Posterior_Probability
              << std::endl;
    std::cout << "S2_Posterior_Probability=" << S2_Posterior_Probability
              << std::endl;
    std::cout << "S3_Posterior_Probability=" << S3_Posterior_Probability
              << std::endl;
  */
  Posterior_Probability = S1_Posterior_Probability + S2_Posterior_Probability +
                          S3_Posterior_Probability;

  // std::cout << "Posterior_Probability=" << Posterior_Probability <<
  // std::endl;
  return Posterior_Probability;
}

double IRM_Co_Clustering::Logfactorial(
    int n) {  // boostの階乗は桁落ちしてしまうので実装しておく
  int result = 0;
  int k = 0;
  for (k = 1; k <= n; k++) {
    result += std::log(k);
  }
  return result;
}

void IRM_Co_Clustering::Output_by_record_csv() {
  FILE *fp;
  if ((fp = fopen("Input_Binary_Relation_Matrix.csv", "w")) != NULL) {
    for (unsigned int i = 0; i < Input_Binary_Relation_Matrix.size(); i++) {
      for (unsigned int j = 0; j < Input_Binary_Relation_Matrix[i].size();
           j++) {
        //カンマで区切ることでCSVファイルとする
        fprintf(fp, "%d", Input_Binary_Relation_Matrix[i][j]);
        if (j != Input_Binary_Relation_Matrix[i].size() - 1) {
          fprintf(fp, ",");
        }
      }
      fprintf(fp, "\n");
    }
    fclose(fp);
  } else {
    std::cout << "File cannot Open" << std::endl;
  }

  FILE *fc;
  if ((fc = fopen("hidden_K.csv", "w")) != NULL) {
    for (unsigned int i = 0; i < hidden_K.size(); i++) {
      fprintf(fc, "%d", hidden_K[i]);
      if (i != hidden_K.size() - 1) {
        fprintf(fc, ",");
      }
    }

    fclose(fc);
  } else {
    std::cout << "File cannot Open" << std::endl;
  }
  FILE *fd;
  if ((fd = fopen("hidden_L.csv", "w")) != NULL) {
    for (unsigned int j = 0; j < hidden_L.size(); j++) {
      fprintf(fd, "%d", hidden_L[j]);
      if (j != hidden_L.size() - 1) {
        fprintf(fd, ",");
      }
    }

    fclose(fd);
  } else {
    std::cout << "File cannot Open" << std::endl;
  }

  FILE *fe;
  if ((fe = fopen("number_of_k_l_in_each_cluster.csv", "w")) != NULL) {
    for (unsigned int i = 0; i < number_of_k_in_each_cluster.size(); i++) {
      fprintf(fe, "number_of_k_in_each_cluster=%d\n",
              number_of_k_in_each_cluster[i]);
    }
    fprintf(fe, "number_of_cluster_K=%d\n", number_of_cluster_K);
    for (unsigned int i = 0; i < number_of_l_in_each_cluster.size(); i++) {
      fprintf(fe, "number_of_l_in_each_cluster=%d\n",
              number_of_l_in_each_cluster[i]);
    }
    fprintf(fe, "number_of_cluster_L==%d\n", number_of_cluster_L);

    fclose(fe);
  } else {
    std::cout << "File cannot Open" << std::endl;
  }
  FILE *ff;
  if ((ff = fopen("Trantsition_Posterior_distriibution.csv", "w")) != NULL) {
    for (unsigned int n = 0; n < Trantsition_Posterior_distriibution.size();
         n++) {
      fprintf(ff, "%lf", Trantsition_Posterior_distriibution[n]);
      if (n != Trantsition_Posterior_distriibution.size() - 1) {
        fprintf(ff, ",");
      }
    }

    fclose(ff);
  } else {
    std::cout << "File cannot Open" << std::endl;
  }
}

void IRM_Co_Clustering::show_IRM_parameter() {
  std::cout << "IRM_Co_Clustering_co_alpha=" << IRM_Co_Clustering_co_alpha
            << std::endl;
  std::cout << "IRM_Co_Clustering_Beta_a=" << IRM_Co_Clustering_Beta_a
            << std::endl;
  std::cout << "IRM_Co_Clustering_Beta_b=" << IRM_Co_Clustering_Beta_b
            << std::endl;
  std::cout << "number_of_K=" << number_of_K << std::endl;
  std::cout << "number_of_L=" << number_of_L << std::endl;
}

void IRM_Co_Clustering::show_datas() {
  std::cout << "Vector系の全要素表示開始" << std::endl;

  for (const auto &i : hidden_K) {
    std::cout << "hidden_K:" << i << std::endl;
  }

  for (const auto &i : hidden_L) {
    std::cout << "hidden_L:" << i << std::endl;
  }

  for (const auto &i : number_of_k_in_each_cluster) {
    std::cout << "number_of_k_in_each_cluster:" << i << std::endl;
  }
  std::cout << "number_of_cluster_K=" << number_of_cluster_K << std::endl;

  for (const auto &i : number_of_l_in_each_cluster) {
    std::cout << "number_of_l_in_each_cluster:" << i << std::endl;
  }
  std::cout << "number_of_cluster_L=" << number_of_cluster_L << std::endl;

  std::cout << "total_hidden_K=" << hidden_K.size() << std::endl;

  std::cout << "total_hidden_L=" << hidden_L.size() << std::endl;

  // std::cout << "total_desk_" << std::endl;
  std::cout << "Vector系の全要素表示終了" << std::endl;
  /*
   std::cout << "Parameter_Relation_Matrix1" << std::endl;
   for (unsigned int i = 0; i < Parameter_Relation_Matrix.size(); i++) {
     for (unsigned int j = 0; j < Parameter_Relation_Matrix[i].size(); j++) {
       std::cout << Parameter_Relation_Matrix[i][j] << "  ";
     }
     std::cout << std::endl;
   }
  */
  std::cout << "Input_Binary_Relation_Matrix" << std::endl;
  for (unsigned int i = 0; i < Input_Binary_Relation_Matrix.size(); i++) {
    for (unsigned int j = 0; j < Input_Binary_Relation_Matrix[i].size(); j++) {
      std::cout << Input_Binary_Relation_Matrix[i][j] << "  ";
    }
    std::cout << std::endl;
  }
}
