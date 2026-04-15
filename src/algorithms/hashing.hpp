#include <array>
#include <utility>
#include <bitset>
#include <queue>
#include <vector>
#include <random>

int random1_100() {
  static std::uniform_int_distribution<int> uni(1,100);
  return uni(rng);
}

template <typename K, typename T>
struct HashData {
  K k = 0;
  T val = 0;
  HashData() = default;
  HashData(K k, T t): k(k), val(t) {}
  HashData(K k): HashData(k, k) {}
};

#ifdef TABLE_SIZE
constexpr int DEFAULT_M = TABLE_SIZE;
#else
constexpr int DEFAULT_M = 11; // necessarily a prime
#endif

template <typename K = int, int M = DEFAULT_M>
struct HashFunction {
  static int hash1(K k) { return ((k % M) + M) % M; }
  static int hash2(K k) { 
    int val  = hash1(k / M); 
    return val ? val : 1;
  }
};

template <typename K = int, typename T = int, int M = DEFAULT_M> 
class HashTable {
  typedef HashData<K, T> DataT;
  typedef HashFunction<K, M> H;

  std::array<DataT,M> m_data;
  unsigned int m_size = 0;
  int m_aux;
  enum {
    NO_REALLOC,
    BRENT,
    BINARY_TREE,
    RANDOMIZED
  } m_reallocation_type = NO_REALLOC;
  int m_reallocation_chance = 50;
public: 
  HashTable() = default;

  int search(const DataT &r) {
    int i = H::hash1(r.k);
    int step = H::hash2(r.k);
    int cnt = 1;
    while(cnt < M && m_data[i].k != 0 && m_data[i].k != r.k)
      i = (i + step) % M, ++cnt;
    
    if(m_data[i].k != r.k)
      i = -1;
    m_aux = cnt;
    return i;
  }
  int get_cnt() { return m_aux; }

  int insert(DataT r) {
    if(m_size == M)
      return -1;
    int i = H::hash1(r.k);
    int step = H::hash2(r.k);

    while (m_data[i].k != 0 && m_data[i].k != r.k) {
      if(m_reallocation_type == RANDOMIZED) {
        if(random1_100()<=m_reallocation_chance) {
          std::swap(r, m_data[i]);
          i = H::hash1(r.k);
          step = H::hash2(r.k);
        }
      }
      i = (i + step) % M;
    }
    
    if(m_data[i].k != r.k) {
      if(m_reallocation_type == BRENT || m_reallocation_type == BINARY_TREE)
        i = realloc(r);
      m_data[i] = r;
    }

    return i;
  }

  int realloc_brent(const DataT &r) {
    int i = H::hash1(r.k);
    int step = H::hash2(r.k);
    for(int p = 1; p < M; ++p)
      for(int q = 0; q < p; ++q) {
        int j = (i + step * (p - q - 1)) % M;
        int jump = H::hash2(m_data[j].k);
        int k = (j + jump * q) % M;
        if(m_data[k].k == 0) {
          m_data[k] = m_data[j];
          i = j;
          p = q = M;
        }
      }
    return i;
  }

  struct NodeT {
    int ind, val, back;
    NodeT() = default;
    NodeT(int a, int b, int c): ind(a), val(b), back(c) {}
  };
  int realloc_binary_tree(const DataT &r) {
    using namespace std;
    int i = H::hash1(r.k);
    int step = H::hash2(r.k);
    static bitset<M*M> used;
    used[i + M*step] = true;
    static vector<NodeT> tree;
    tree.emplace_back(i, r.k, -1);
    for(int j = 0; j < tree.size(); ++j) {
      auto [ ind, val, back ] = tree[j];
      if(m_data[ind].k == 0) {
        while(tree[j].back!=-1){
          m_data[tree[j].ind] = m_data[tree[tree[j].back].ind];
          j = tree[j].back;
        }
        i = tree[j].ind;
        break;
      }
      int jump = H::hash2(val);
      if(!used[(ind + jump)%M + jump*M]) {
        used[(ind + jump)%M + jump*M] = true;
        tree.emplace_back((ind + jump)%M, val, back);
      }
      jump = H::hash2(m_data[ind].k);
      if(!used[(ind + jump)%M + jump*M]) {
        used[(ind + jump)%M + jump*M] = true;
        tree.emplace_back((ind + jump)%M, m_data[ind].k, j);
      }
    }
    for(auto &toers : tree) {
      int step = H::hash2(toers.val);
      used[toers.ind + step*M] = false;
    }
    tree.clear();
    return i;
  }

  int realloc(const DataT &r) {
    if(m_reallocation_type == BRENT)
      return realloc_brent(r);
    else
      return realloc_binary_tree(r);
  }

  void print() {
    std::cout << "------TABLE------" << std::endl;
    for(int i = 0; i < M; ++i) {
      std::cout << i << "\t|\t" << m_data[i].k << std::endl;
    }
  }

  void clear() {
    for(int i = 0; i < M; ++i) 
      m_data[i] = DataT();
  }

  void set_no_realloc() { m_reallocation_type = NO_REALLOC; }
  void set_randomized_realloc() { m_reallocation_type = RANDOMIZED; }
  void set_brent_realloc() { m_reallocation_type = BRENT; }
  void set_binary_realloc() { m_reallocation_type = BINARY_TREE; }
  void set_reallocation_chance(int x) { m_reallocation_chance = x; }
};
