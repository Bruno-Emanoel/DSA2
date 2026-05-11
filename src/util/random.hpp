#include <random>
  
std::mt19937 rng(47);
std::uniform_int_distribution<int> uniform;

std::vector<int> selectRandom(std::vector<int> vec, size_t size) {
  using namespace std;
  vector<int> ret;
  ret.reserve(size);
  while(ret.size()!=size) {
    int ind = uniform_int_distribution<int>(0,vec.size()-1)(rng);
    ret.emplace_back(vec[ind]);
    swap(vec[ind],vec.back());
    vec.pop_back();
  }
  return ret;
}