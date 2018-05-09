// sampler.cpp
//確率密度関数類のまとめ,IRM::sampler::で呼び出し
#include <chrono>  //C++11依存,時間計測(これを基に乱数発生)
#include "sampler.h"

namespace IRM {
namespace sampler {

int seed =
    std::chrono::system_clock::now().time_since_epoch().count();  //現在時刻
std::mt19937 mt(seed);
//メルセンヌツイスター法で乱数の確率分布引数発生
void set_seed(int seed) { auto  mt = std::mt19937(seed); }

//以下,使用する確率分布
double gamma(double a, double b) {
  std::gamma_distribution<double> distribution(a, 1.0 / b);
  return distribution(mt);
}
double beta(double a,double b) {
  double ga = gamma(a, 1.0);
  double gb = gamma(b, 1.0);
  return ga / (ga + gb);
}
double bernoulli(double p) { 
std::bernoulli_distribution distribution(p);
return  distribution(mt);
}
double uniform(double min, double max) {  //二値間の一様分布(実数double)
  std::uniform_real_distribution<double> uni_d(min, max);
  return uni_d(mt);
}
double uniform_int(int min, int max) {  //二値間の一様分布(整数int)
  std::uniform_int_distribution<> uni_i(min, max);
  return uni_i(mt);
}
double normal(double mean, double stddev) {  //正規分布
  std::normal_distribution<double> Normal(mean, stddev);
  return Normal(mt);
}
}  // namespace sampler
}  // namespace IRM

