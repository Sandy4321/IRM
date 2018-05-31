// Generate_IRM.cpp
// Generate_IRMの過程に従って自動的に客を分割
// 入力:Generate_IRMのパラメータalpha,Generate_IRMの試行回数(客の人数)
// 出力:各客の座り方, 各机の客の人数
#include <boost/foreach.hpp>
#include <boost/math/special_functions/beta.hpp>
#include <boost/math/special_functions/factorials.hpp>
#include <boost/random.hpp>
#include <boost/tokenizer.hpp>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>
#include "CRP.hpp"
#include "Generate_IRM.hpp"

Generate_IRM::Generate_IRM() {
  Posterior = 0;
  Generate_IRM_co_alpha = 0;
  Generate_IRM_Beta_a = 0;
  Generate_IRM_Beta_b = 0;
  number_of_K = 0;  //比較対象Kの総数,パターン認識本の例の顧客数
  number_of_L = 0;  //比較対象Lの総数,パターン認識本の例の商品数
  number_of_cluster_K = 0;  //比較対象Kの総クラスタ数
  number_of_cluster_L = 0;  //比較対象Lの総クラスタ数

  hidden_K.resize(1, 0);
  hidden_L.resize(1, 0);
  number_of_k_in_each_cluster.resize(number_of_cluster_K, 0);
  number_of_l_in_each_cluster.resize(number_of_cluster_L, 0);

  Output_Binary_Relation_k.resize(
      1, 0);  // 比較対象Kのk番目の要素とLの関係を表すL長vector
  Output_Binary_Relation_Matrix.resize(
      1, Output_Binary_Relation_k);  // A(boost::extents[10][10]);

  Parameter_Relation_k.resize(1, 0);

  Parameter_Relation_Matrix.resize(
      1,
      Parameter_Relation_k);  // A(boost::extents[10][10]);
}

void Generate_IRM::set_number_of_K_L(int a, int b) {
  number_of_K = a;
  number_of_L = b;
}

void Generate_IRM::set_IRM_hyper_parameter(double alpha, double a, double b) {
  Generate_IRM_co_alpha = alpha;
  Generate_IRM_Beta_a = a;
  Generate_IRM_Beta_b = b;
}

void Generate_IRM::get_each_cluster_number() {
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
}

void Generate_IRM::set_hidden_K_L(std::vector<int> &K_vector,
                                  std::vector<int> &L_vector) {
  hidden_K.resize(K_vector.size());
  hidden_L.resize(L_vector.size());

  std::copy(K_vector.begin(), K_vector.end(), hidden_K.begin());
  std::copy(L_vector.begin(), L_vector.end(), hidden_L.begin());
}

void Generate_IRM::
    Set_Parameter_Relation_Matrix_Beta() {  // Beta分布で各クラスた毎のパラメータ設定

  //確実にするなら初期化してしまおう

  Parameter_Relation_Matrix = std::vector<std::vector<double>>(
      //この方法だとベータ分布の入力パラメータは0.005が最小(Boost使う方にいずれ変えた方が良い)
      number_of_k_in_each_cluster.size(),
      std::vector<double>(number_of_l_in_each_cluster.size(), 0));

  Parameter_Relation_Matrix.shrink_to_fit();

  // Parameter_Relation_k.resize(number_of_cluster_L,0))
  std::random_device seed_gen;
  //乱数部分はあとでもっとglobalに纏められそう
  // auto engine = std::mt19937(seed_gen());
  std::mt19937 engine(seed_gen());
  std::gamma_distribution<double> g_dis_a(Generate_IRM_Beta_a, 1.0 / 1.0);
  std::gamma_distribution<double> g_dis_b(Generate_IRM_Beta_b, 1.0 / 1.0);

  for (unsigned int i = 0; i < Parameter_Relation_Matrix.size(); i++) {
    for (unsigned int j = 0; j < Parameter_Relation_Matrix[i].size(); j++) {
      double ga = g_dis_a(engine);
      double gb = g_dis_b(engine);
      Parameter_Relation_Matrix[i][j] = ga / (ga + gb);
      /*
            std::cout << "ga=" << ga << std::endl;
            std::cout << "gb=" << gb << std::endl;
            std::cout << "Each Parameter_Relation_Matrix[i][j]"
                      << " " << i << " " << j << " "
                      << Parameter_Relation_Matrix[i][j] << std::endl;
      */
    }
  }
}
void Generate_IRM::decide_Output_Binary_Relation_Matrix() {
  //各Relation_Matrixをベルヌーイ分布から決定
  Output_Binary_Relation_Matrix = std::vector<std::vector<int>>(
      hidden_K.size(), std::vector<int>(hidden_L.size(), 0));
  /*
    std::cout << "Output_Binary_Relation_Matrix!!!!!" << std::endl;
    for (int i = 0; i < Output_Binary_Relation_Matrix.size(); i++) {
      for (int j = 0; j < Output_Binary_Relation_Matrix[i].size(); j++) {
        std::cout << Output_Binary_Relation_Matrix[i][j] << "  ";
      }
      std::cout << std::endl;
    }
  */
  Parameter_Relation_Matrix.shrink_to_fit();

  std::random_device seed_gen;
  std::mt19937 engine(seed_gen());

  for (unsigned int i = 0; i < Output_Binary_Relation_Matrix.size(); i++) {
    for (unsigned int j = 0; j < Output_Binary_Relation_Matrix[i].size(); j++) {
      double theta = 0;
      theta = Parameter_Relation_Matrix[hidden_K[i] - 1][hidden_L[j] - 1];
      std::bernoulli_distribution dist(theta);
      Output_Binary_Relation_Matrix[i][j] = dist(engine);

      std::cout << theta << " ";  // tmp
    }
    std::cout << std::endl;  // tmp
  }
}

double Generate_IRM::get_Posterior_Probability() {
  double Posterior_Probability = 0;

  double S1_Posterior_Probability = 0;
  double S2_Posterior_Probability = 0;
  double S3_Posterior_Probability = 0;

  S1_Posterior_Probability =
      (double)number_of_k_in_each_cluster.size() *
          (std::log((double)Generate_IRM_co_alpha)) -
      (Logfactorial((double)number_of_k_in_each_cluster.size()));

  for (unsigned int i = 0; i < number_of_k_in_each_cluster.size(); i++) {
    S1_Posterior_Probability +=
        Logfactorial((double)number_of_k_in_each_cluster[i] - 1);
  }
  for (unsigned int i = 0; i < hidden_K.size(); i++) {
    S1_Posterior_Probability -= std::log((double)Generate_IRM_co_alpha + i);
  }

  S2_Posterior_Probability =
      (double)number_of_l_in_each_cluster.size() *
          (std::log((double)Generate_IRM_co_alpha)) -
      (Logfactorial((double)number_of_l_in_each_cluster.size()));

  for (unsigned int i = 0; i < number_of_l_in_each_cluster.size(); i++) {
    S2_Posterior_Probability +=
        Logfactorial((double)number_of_l_in_each_cluster[i] - 1);
  }
  for (unsigned int i = 0; i < hidden_L.size(); i++) {
    S2_Posterior_Probability -= std::log((double)Generate_IRM_co_alpha + i);
  }

  /*
        S1_Posterior_Probability =
            (std::pow(Generate_IRM_co_alpha,
     number_of_k_in_each_cluster.size())); for (auto i = 0; i <
     number_of_k_in_each_cluster.size(); i++) { S1_Posterior_Probability *=
              (boost::math::factorial<double>(number_of_k_in_each_cluster[i] -
      1));
        }
        for (auto i = 0; i < hidden_K.size(); i++) {
          S1_Posterior_Probability /= (Generate_IRM_co_alpha + i);
        }

        S2_Posterior_Probability =
            std::pow(Generate_IRM_co_alpha, number_of_l_in_each_cluster.size());
        for (auto j = 0; j < number_of_l_in_each_cluster.size(); j++) {
          S2_Posterior_Probability *=
              (boost::math::factorial<double>(number_of_l_in_each_cluster[j] -
      1));
        }
        for (auto j = 0; j < hidden_L.size(); j++) {
          S2_Posterior_Probability /= (Generate_IRM_co_alpha + j);
        }
      */

  for (unsigned int i = 0; i < number_of_k_in_each_cluster.size(); i++) {
    for (unsigned int j = 0; j < hidden_L.size(); j++) {
      double n_full_full_i_j = 0;
      double bar_n_full_full_i_j = 0;

      for (unsigned int k = 0; k < Output_Binary_Relation_Matrix.size();
           k++) {  //クラスタjについてカウント

        if (hidden_K[k] == i + 1) {
          for (unsigned int l = 0;
               l < Output_Binary_Relation_Matrix[k].size();  // k行を調べる
               l++) {
            if (hidden_L[l] == j + 1) {
              if (Output_Binary_Relation_Matrix[k][l] ==
                  1) {  // Relation_Matrixの値が1かどうか
                n_full_full_i_j += 1;
              } else {
                bar_n_full_full_i_j += 1;
              }
            }
          }
        }
      }

      S3_Posterior_Probability +=
          (std::lgamma(Generate_IRM_Beta_a + n_full_full_i_j) +
           std::lgamma(Generate_IRM_Beta_b + bar_n_full_full_i_j) -
           std::lgamma(Generate_IRM_Beta_a + n_full_full_i_j +
                       Generate_IRM_Beta_b + bar_n_full_full_i_j)) -
          (std::lgamma(Generate_IRM_Beta_a) + std::lgamma(Generate_IRM_Beta_b) -
           std::lgamma(Generate_IRM_Beta_a + Generate_IRM_Beta_b));
/*
          (boost::math::beta(Generate_IRM_Beta_a, Generate_IRM_Beta_b)) 
          (boost::math::beta(Generate_IRM_Beta_a + n_full_full_i_j,
                             Generate_IRM_Beta_b + bar_n_full_full_i_j));
  */ }
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
  Posterior = Posterior_Probability;
  std::cout << "Posterior_Probability=" << Posterior_Probability << std::endl;
  return Posterior_Probability;
}

double Generate_IRM::Logfactorial(
    double n) {  // boostの階乗は桁落ちしてしまうので実装しておく
  double result = 0;
  double k;

  for (k = 1; k <= n; k++) {
    result += std::log(k);
  }
  return result;
}

void Generate_IRM::run_Generate_IRM() {  // Generate_IRMの本体
  // Generate_IRM::Generate_IRM_first_customer();
}

void Generate_IRM::Output_by_record_csv() {
  FILE *fp;
  if ((fp = fopen("Simulate_Output_Binary_Relation_Matrix.csv", "w")) != NULL) {
    for (unsigned int i = 0; i < Output_Binary_Relation_Matrix.size(); i++) {
      for (unsigned int j = 0; j < Output_Binary_Relation_Matrix[i].size();
           j++) {
        //カンマで区切ることでCSVファイルとする
        fprintf(fp, "%d", Output_Binary_Relation_Matrix[i][j]);
        if (j != Output_Binary_Relation_Matrix[i].size() - 1) {
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
  if ((fa = fopen("Simulate_Parameter_Relation_Matrix.csv", "w")) != NULL) {
    for (unsigned int i = 0; i < Parameter_Relation_Matrix.size(); i++) {
      for (unsigned int j = 0; j < Parameter_Relation_Matrix[i].size(); j++) {
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
  if ((fc = fopen("Simulate_hidden_K.csv", "w")) != NULL) {
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
  if ((fd = fopen("Simulate_hidden_L.csv", "w")) != NULL) {
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
  if ((fe = fopen("Simulate_number_of_k_l_in_each_cluster.csv", "w")) != NULL) {
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
    fprintf(fe, "Posterior=%lf\n", Posterior);

    fclose(fe);
  } else {
    std::cout << "File cannot Open" << std::endl;
  }
  FILE *ff;
  if ((ff = fopen("Simulate_Output_size_Parameter_Relation_Matrix.csv", "w")) != NULL) {
    for (unsigned int i = 0; i < Output_Binary_Relation_Matrix.size(); i++) {
      for (unsigned int j = 0; j < Output_Binary_Relation_Matrix[i].size();
           j++) {
        //カンマで区切ることでCSVファイルとする
        fprintf(ff, "%lf",
                Parameter_Relation_Matrix[hidden_K[i] - 1][hidden_L[j] - 1]);
        if (j != Output_Binary_Relation_Matrix[i].size() - 1) {
          fprintf(ff, ",");
        }
      }
      fprintf(ff, "\n");
    }
    fclose(ff);
  } else {
    std::cout << "File cannot Open" << std::endl;
  }
  FILE *fg;
  if ((fg = fopen("Simulate_Parameter_Relation_Matrix.csv", "w")) != NULL) {
    for (unsigned int k = 0; k < Parameter_Relation_Matrix.size(); k++) {
      for (unsigned int l = 0; l < Parameter_Relation_Matrix[k].size(); l++) {
        //カンマで区切ることでCSVファイルとする
        fprintf(fg, "%lf", Parameter_Relation_Matrix[k][l]);
        if (l != Parameter_Relation_Matrix[k].size() - 1) {
          fprintf(fg, ",");
        }
      }
      fprintf(fg, "\n");
    }
    fclose(fg);
  } else {
    std::cout << "File cannot Open" << std::endl;
  }
  FILE *fh;
  FILE *fi;
  if ((fh = fopen("Simulate_Each_cluster_number_label_Matrix.csv", "w")) != NULL) {
    if ((fi = fopen("Simulate_Each_cluster_1_number_label_Matrix.csv", "w")) != NULL) {
      for (unsigned int i = 0; i < number_of_k_in_each_cluster.size(); i++) {
        for (unsigned int j = 0; j < number_of_l_in_each_cluster.size(); j++) {
          int n_full_full_i_j = 0;
          int bar_n_full_full_i_j = 0;
          for (unsigned int k = 0; k < Output_Binary_Relation_Matrix.size();
               k++) {  //クラスタjについてカウント
            for (unsigned int l = 0;
                 l < Output_Binary_Relation_Matrix[k].size();  // k行を調べる
                 l++) {
              if (hidden_K[k] == i + 1) {
                if (hidden_L[l] == j + 1) {
                  if (Output_Binary_Relation_Matrix[k][l] ==
                      1) {  // Relation_Matrixの値が1かどうか
                    n_full_full_i_j += 1;
                  } else {
                    bar_n_full_full_i_j += 1;
                  }
                }
              }
            }
          }

          fprintf(fh, "%d", n_full_full_i_j + bar_n_full_full_i_j);
          if (j != number_of_l_in_each_cluster.size() - 1) {
            fprintf(fh, ",");
          }
          fprintf(fi, "%d", n_full_full_i_j);
          if (j != number_of_l_in_each_cluster.size() - 1) {
            fprintf(fi, ",");
          }
        }
        fprintf(fh, "\n");
        fprintf(fi, "\n");
      }
      fclose(fh);
      fclose(fi);

    } else {
      std::cout << "File cannot Open" << std::endl;
    }

  } else {
    std::cout << "File cannot Open" << std::endl;
  }
}
void Generate_IRM::show_IRM_parameter() {
  std::cout << "Generate_IRM_co_alpha" << Generate_IRM_co_alpha << std::endl;
  std::cout << "Generate_IRM_Beta_a" << Generate_IRM_Beta_a << std::endl;
  std::cout << "Generate_IRM_Beta_b" << Generate_IRM_Beta_b << std::endl;
  std::cout << "number_of_K" << number_of_K << std::endl;
  std::cout << "number_of_cluster_L" << number_of_L << std::endl;
}

void Generate_IRM::show_datas() {
  std::cout << "Vector系の全要素表示開始" << std::endl;

  /* for (const auto &i : hidden_K) {
     std::cout << "hidden_K" << i << std::endl;
   }
   */
  /*
  for (const auto &i : hidden_L) {
    std::cout << "hidden_L" << i << std::endl;
  }
  */
  for (const auto &i : number_of_k_in_each_cluster) {
    std::cout << "number_of_k_in_each_cluster" << i << std::endl;
  }
  std::cout << "number_of_cluster_K" << number_of_cluster_K << std::endl;

  for (const auto &i : number_of_l_in_each_cluster) {
    std::cout << "number_of_l_in_each_cluster" << i << std::endl;
  }
  std::cout << "number_of_cluster_L" << number_of_cluster_L << std::endl;

  std::cout << "total_hidden_K" << hidden_K.size() << std::endl;

  std::cout << "total_hidden_L" << hidden_L.size() << std::endl;

  // std::cout << "total_desk_" << std::endl;
  std::cout << "Vector系の全要素表示終了" << std::endl;
  std::cout << "Parameter_Relation_Matrix1" << std::endl;
  for (unsigned int i = 0; i < Parameter_Relation_Matrix.size(); i++) {
    for (unsigned int j = 0; j < Parameter_Relation_Matrix[i].size(); j++) {
      std::cout << Parameter_Relation_Matrix[i][j] << "  ";
    }
    std::cout << std::endl;
  }
  std::cout << "Output_Binary_Relation_Matrix" << std::endl;
  for (unsigned int i = 0; i < Output_Binary_Relation_Matrix.size(); i++) {
    for (unsigned int j = 0; j < Output_Binary_Relation_Matrix[i].size(); j++) {
      std::cout << Output_Binary_Relation_Matrix[i][j] << "  ";
    }
    std::cout << std::endl;
  }
}
