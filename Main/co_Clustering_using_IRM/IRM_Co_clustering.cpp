// IRM_Co_clustering.cpp
// IRM_Co_Clusteringの過程に従って自動的に客を分割
// 入力:IRM_Co_Clusteringのパラメータalpha,IRM_Co_Clusteringの試行回数(客の人数)
// 出力:各客の座り方, 各机の客の人数
#include <boost/foreach.hpp>
#include <boost/math/special_functions/beta.hpp>
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

  Max_Posterior_Probability = 0;
  tmp_Posterior_Probability = 0;

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
}

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
  for (const auto &i : tmp_hidden_K) {
    std::cout << "オリジナルのtmp_hidden_K:" << i << std::endl;
  }

  for (int k = 0; k < hidden_K.size(); k++) {
    // tmp_hidden_Kからk番目を一つ削除
    k_iterator = k;
    int tmp_delete_atom_k = 0;
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
      std::cout << "tmp_hidden_Kクラスタアップデート" << k
                << "人目の値(添字なので人なら+1)" << std::endl;
      std::cout << "それは値" << tmp_delete_atom_k << "より上" << std::endl;
      for (const auto &i : tmp_hidden_K) {
        std::cout << "アップデート前のtmp_hidden_K:" << i << std::endl;
      }

      for (auto &i : tmp_hidden_K) {
        if (i > tmp_delete_atom_k) {
          i -= 1;
        }
      }

      std::cout << "そして" << std::endl;

      for (const auto &i : tmp_hidden_K) {
        std::cout << "アップデート後の時の各tmp_hidden_K:" << i << std::endl;
      }
    }

    tmp_hidden_K_get_each_cluster_number();  //改めてクラスター数を設定
    for (const auto &i : tmp_number_of_k_in_each_cluster) {
      std::cout << k << "番目の処理の各number_of_k_in_each_cluster:" << i
                << std::endl;
    }
    std::cout << std::endl;

    double chosed_new_atom = 0;
    chosed_new_atom = get_new_cluster_for_K();
    std::cout << "クラスタ選択chosed_new_atom=" << chosed_new_atom << std::endl;
    if (chosed_new_atom == tmp_number_of_k_in_each_cluster.size()) {
      std::cout << "新しいクラスタが選ばれた"
                << std::endl;  //新しいクラスタが選ばれたら,,特に処理はなし??
    }

    tmp_delete_atom_k = chosed_new_atom;

    // tmp_hidden_Kにtmp_delete_atom_を戻す
    tmp_hidden_K.insert(tmp_hidden_K.begin() + k,
                        tmp_delete_atom_k);  // tmp_delete_atom_kは変更する**
  }

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
  // k番目の要素の最適なクラスターを選択
  // 新しい客の机を選択
  std::discrete_distribution<std::size_t> cluster_dis(probability_ratio.begin(),
                                                      probability_ratio.end());

  for (int i = 0; i < tmp_number_of_k_in_each_cluster.size();
       i++) {  //既存のクラスタを選択する確率
    int iter_i = i;
    probability_ratio.push_back(already_cluster_prob_K(iter_i));
  }
  probability_ratio.push_back(
      new_cluster_prob_K());  //新しいクラスタを選択する確率

  for (const auto &i : probability_ratio) {
    std::cout << "各確率は:" << i << std::endl;
  }

  return cluster_dis(engine) + 1;  //返す値は0スタートだけど机は1スタートなので
}

double IRM_Co_Clustering::already_cluster_prob_K(
    int i_cluster_of_K) {  // i番目の顧客クラスターについて調べる
  double already_prob_K = 1;

  for (int j = 0; j < tmp_number_of_cluster_L; j++) {  //クラスタ数の数だけ実施
    int n_full_full_i_j = 0;
    int bar_n_full_full_i_j = 0;
    int n_notk_full_i_j = 0;
    int bar_n_notk_full_i_j = 0;

    int n_k_full_i_j = 0;      //今回は補助変数
    int bar_n_k_full_i_j = 0;  //今回は補助変数
    for (int k = 0; k < Input_Binary_Relation_Matrix.size();
         k++) {  //クラスタjについてカウント
      if (tmp_hidden_K[k] == i_cluster_of_K) {
        for (int l = 0;
             l <
             Input_Binary_Relation_Matrix[k_iterator].size();  // k行を調べる
             l++) {
          if (tmp_hidden_L[l] == j) {
            if (Input_Binary_Relation_Matrix[k_iterator][l] ==
                1) {  // Relation_Matrixの値が1かどうか
              n_full_full_i_j += 1;
            } else {
              bar_n_full_full_i_j += 1;
            }
          }
        }
      }
    }
    for (int l = 0;
         l < Input_Binary_Relation_Matrix[k_iterator].size();  // k行を調べる
         l++) {
      if (tmp_hidden_L[l] == j) {  //要素lがクラスタjに属しているかどうか
        if (Input_Binary_Relation_Matrix[k_iterator][l] ==
            1) {  // Relation_Matrixの値が1かどうか
          n_k_full_i_j += 1;
        } else {
          bar_n_k_full_i_j += 1;
        }
      }
    }
    n_notk_full_i_j = n_full_full_i_j - n_k_full_i_j;
    bar_n_notk_full_i_j = bar_n_full_full_i_j - bar_n_k_full_i_j;

    already_prob_K *=
        ((double)tmp_number_of_l_in_each_cluster[i_cluster_of_K] *
         (boost::math::beta(IRM_Co_Clustering_Beta_a + n_full_full_i_j,
                            IRM_Co_Clustering_Beta_b + bar_n_full_full_i_j)) /
         (boost::math::beta(IRM_Co_Clustering_Beta_a + n_notk_full_i_j,
                            IRM_Co_Clustering_Beta_b + bar_n_notk_full_i_j)));
    // already_prob_K =1;テスト用
  }

  return already_prob_K;
}
double IRM_Co_Clustering::new_cluster_prob_K() {
  double new_prob = 1;
  for (int j = 0; j < tmp_number_of_cluster_L; j++) {  //クラスタ数の数だけ実施
    int n_k_full_i_j = 0;
    int bar_n_k_full_i_j = 0;
    for (int l = 0;
         l < Input_Binary_Relation_Matrix[k_iterator].size();  // k行を調べる
         l++) {
      if (tmp_hidden_L[l] == j) {  //要素lがクラスタjに属しているかどうか
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
    // new_prob *=IRM_Co_Clustering_co_alpha; テスト用
  }
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
  for (int l = 0; l < hidden_L.size(); l++) {
    // tmp_hidden_Lからl番目を一つ削除
    //諸々の更新
    // l番目に最適なクラスターを選択して決定
    // tmp_hidden_Lに戻す
    // 長さが同じか確認
  }
}



//完成したtmp_K_hiddenとtmp_hidden_Lを用いて事後確率計算
// tmp_hidden_Kにhidden_Kをアップデートするか判定して更新かそのままにする
void IRM_Co_Clustering::decide_update_tmp_or_not_hidden_KL() {
  if (tmp_hidden_K.size() != hidden_K.size()) {  //更新そもそもして大丈夫か確認
    std::cout << "Error!!tmp_hidden_Kとhidden_Kの長さが違う";
  }
  if (tmp_hidden_L.size() != hidden_L.size()) {
    std::cout << "Error!!tmp_hidden_Lとhidden_Lの長さが違う";
  }

  if (tmp_Posterior_Probability >= 1 || tmp_Posterior_Probability < 0 ||
      Max_Posterior_Probability >= 1 || Max_Posterior_Probability < 0) {
    std::cout
        << "Error!!tmp_Posterior_Probability か Posterior_Probabilityが違う";
  }

  if (tmp_Posterior_Probability > Max_Posterior_Probability) {
    // hidden_Kとhidden_Lの更新処理
    Max_Posterior_Probability = tmp_Posterior_Probability;
    std::copy(tmp_hidden_K.begin(), tmp_hidden_K.end(), hidden_K.begin());
    std::copy(tmp_hidden_L.begin(), tmp_hidden_L.end(), hidden_L.begin());
    first_get_each_cluster_number();  //クラスタ関連の数を初期化して更新*****要check
    std::cout << "Updated!!" << std::endl;
  } else {
    std::cout << "Not_Updated!!" << std::endl;
  }
  //続行するか終わるかの処理
}

void IRM_Co_Clustering::Output_by_record_csv() {
  FILE *fp;
  if ((fp = fopen("Input_Binary_Relation_Matrix.csv", "w")) != NULL) {
    for (int i = 0; i < Input_Binary_Relation_Matrix.size(); i++) {
      for (int j = 0; j < Input_Binary_Relation_Matrix[i].size(); j++) {
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
  FILE *fa;
  if ((fa = fopen("Parameter_Relation_Matrix.csv", "w")) != NULL) {
    for (int i = 0; i < Parameter_Relation_Matrix.size(); i++) {
      for (int j = 0; j < Parameter_Relation_Matrix[i].size(); j++) {
        fprintf(fa, "%lf", Parameter_Relation_Matrix[i][j]);
        if (j != Parameter_Relation_Matrix[i].size() - 1) {
          fprintf(fa, ",");
        }
      }
      fprintf(fa, "\n");
    }
    fclose(fa);
  } else {
    std::cout << "File cannot Open" << std::endl;
  }
  FILE *fc;
  if ((fc = fopen("hidden_K.csv", "w")) != NULL) {
    for (int i = 0; i < hidden_K.size(); i++) {
      fprintf(fc, "%d\n", hidden_K[i]);
    }

    fclose(fa);
  } else {
    std::cout << "File cannot Open" << std::endl;
  }
  FILE *fd;
  if ((fd = fopen("hidden_L.csv", "w")) != NULL) {
    for (int i = 0; i < hidden_L.size(); i++) {
      fprintf(fd, "%d\n", hidden_L[i]);
    }

    fclose(fd);
  } else {
    std::cout << "File cannot Open" << std::endl;
  }
  FILE *fe;
  if ((fe = fopen("number_of_k_l_in_each_cluster.csv", "w")) != NULL) {
    for (int i = 0; i < number_of_k_in_each_cluster.size(); i++) {
      fprintf(fe, "number_of_k_in_each_cluster=%d\n",
              number_of_k_in_each_cluster[i]);
    }
    fprintf(fe, "number_of_cluster_K=%d\n", number_of_cluster_K);
    for (int i = 0; i < number_of_l_in_each_cluster.size(); i++) {
      fprintf(fe, "number_of_l_in_each_cluster=%d\n",
              number_of_l_in_each_cluster[i]);
    }
    fprintf(fe, "number_of_cluster_L==%d\n", number_of_cluster_L);

    fclose(fd);
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
   for (int i = 0; i < Parameter_Relation_Matrix.size(); i++) {
     for (int j = 0; j < Parameter_Relation_Matrix[i].size(); j++) {
       std::cout << Parameter_Relation_Matrix[i][j] << "  ";
     }
     std::cout << std::endl;
   }
  */
  std::cout << "Input_Binary_Relation_Matrix" << std::endl;
  for (int i = 0; i < Input_Binary_Relation_Matrix.size(); i++) {
    for (int j = 0; j < Input_Binary_Relation_Matrix[i].size(); j++) {
      std::cout << Input_Binary_Relation_Matrix[i][j] << "  ";
    }
    std::cout << std::endl;
  }
}

/*
void IRM_Co_Clustering::
    Set_Parameter_Relation_Matrix_Beta() {  //
Beta分布で各クラスた毎のパラメータ設定

  //確実にするなら初期化してしまおう

  Parameter_Relation_Matrix = std::vector<std::vector<double>>(
      number_of_k_in_each_cluster.size(),
      std::vector<double>(number_of_l_in_each_cluster.size(), 0));

  Parameter_Relation_Matrix.shrink_to_fit();

  // Parameter_Relation_k.resize(number_of_cluster_L,0))
  std::random_device seed_gen;
  //乱数部分はあとでもっとglobalに纏められそう
  // auto engine = std::mt19937(seed_gen());
  std::mt19937 engine(seed_gen());
  std::gamma_distribution<double> g_dis_a(IRM_Co_Clustering_Beta_a, 1.0 / 1.0);
  std::gamma_distribution<double> g_dis_b(IRM_Co_Clustering_Beta_b, 1.0 / 1.0);

  for (int i = 0; i < Parameter_Relation_Matrix.size(); i++) {
    for (int j = 0; j < Parameter_Relation_Matrix[i].size(); j++) {
      double ga = g_dis_a(engine);
      double gb = g_dis_b(engine);
      Parameter_Relation_Matrix[i][j] = ga / (ga + gb);
    }
  }
}

void IRM_Co_Clustering::decide_Output_Binary_Relation_Matrix() {
  //各Relation_Matrixをベルヌーイ分布から決定
  Input_Binary_Relation_Matrix = std::vector<std::vector<int>>(
      hidden_K.size(), std::vector<int>(hidden_L.size(), 0));


  Parameter_Relation_Matrix.shrink_to_fit();

  std::random_device seed_gen;
  std::mt19937 engine(seed_gen());

  for (int i = 0; i < Input_Binary_Relation_Matrix.size(); i++) {
    for (int j = 0; j < Input_Binary_Relation_Matrix[i].size(); j++) {
      double theta = 0;
      theta = Parameter_Relation_Matrix[hidden_K[i] - 1][hidden_L[j] - 1];
      std::bernoulli_distribution dist(theta);
      Input_Binary_Relation_Matrix[i][j] = dist(engine);
    }
  }
}

void IRM_Co_Clustering::run_IRM_Co_Clustering() {  // IRM_Co_Clusteringの本体
}
*/
