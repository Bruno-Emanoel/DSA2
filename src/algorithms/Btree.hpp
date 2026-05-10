#ifndef BTREE_CLASS
#define BTREE_CLASS
#include <vector>
template <typename K, typename T>
struct BTreeData {
  K k;
  T val;
  BTreeData() = default;
  BTreeData(const K &k) : k(k) {};
  BTreeData(const K &k, const T &t) : k(k), val(t) {};
};

template <typename K = int, typename T = int>
class Btree {
public:
  using DataT = BTreeData<K,T>;
  using sizeT = std::size_t;
  struct NodeT  {
    std::vector<DataT> data;
    std::vector<NodeT*> child;
    bool leaf = true;
    NodeT() = default;
    NodeT(sizeT t) {
      data.reserve(2*t-1);
    }
    sizeT size() const { return data.size(); }
    void expand() { 
      leaf = false;
      child.reserve(data.capacity()+1);
    }
    void expand(sizeT t) {
      data.reserve(2*t-1);
      expand();
    }
    void resize(sizeT n) {
      data.resize(n);
      if(!leaf)
        child.resize(n+1);
    }
    void emplace(const DataT &x) {
      data.emplace_back(x);
    }
    void emplace(NodeT *p) {
      child.emplace_back(p);
    }
    void emplace(const DataT &x, NodeT *p) {
      emplace(x);
      emplace(p);
    }
  };
  using NodeP = NodeT*;
  using CNodeP = const NodeT*;

private:
  #define MIN_SIZE (m_degree - 1)
  #define MAX_SIZE (2*m_degree - 1)

  sizeT m_size = 0, m_degree = 3;
  NodeT m_root;

  NodeP p_createNode() const {
    return new NodeT(m_degree);
  }

  void p_splitChild(NodeP x, sizeT i) {
    #define MID MIN_SIZE
    NodeP y = x->child[i], z = p_createNode();
    if(!y->leaf) {
      z->expand();
      for(sizeT j = 1; j <= MID; ++j)
        z->emplace(std::move(y->data[MID+j]), y->child[MID+j]);
      z->emplace(y->child.back());
    }else {
      for(sizeT j = 1; j <= MID; ++j)
        z->emplace(std::move(y->data[MID+j]));
    }
    x->data.emplace(x->data.begin() + i, std::move(y->data[MID]));
    x->child.emplace(x->child.begin() + i + 1, z);
    y->resize(MID);
  }

  void p_reroot() {
    NodeP oldRoot = new NodeT(std::move(m_root));
    m_root.expand(m_degree);
    m_root.child.emplace_back(oldRoot);
    p_splitChild(&m_root,0); // split the oldRoot and place its median as the new root
  }

  T *p_search(const K &k, NodeP x) const {
    sizeT i = 0;  
    while(i < x->size() && k > x->data[i].k)
      ++i;
    if(i < x->size() && k == x->data[i].k)
      return &x->data[i].val;
    if(x->leaf)
      return nullptr;
    return p_search(k, x->child[i]);
  }

  T *p_emplace(const K &k, const T &val, NodeP x) {
    sizeT i = 0;  
    while(i < x->size() && k > x->data[i].k)
      ++i;
    if(x->leaf) {
      x->data.emplace(x->data.begin() + i, DataT(k, val));
      return &x->data[i].val;
    }
    if(x->child[i]->size() == MAX_SIZE) {
      p_splitChild(x,i);
      if(k > x->data[i].k)
        ++i;
    }
    return p_emplace(k, val, x->child[i]);
  }

  T *p_findOrCreate(const K &k, const T &val, NodeP x) {
    sizeT i = 0;  
    while(i < x->size() && k > x->data[i].k)
      ++i;
    if(i < x->size() && k == x->data[i].k)
      return &x->data[i].val;
    if(x->leaf) {
      x->data.emplace(x->data.begin() + i, DataT(k, val));
      return &x->data[i].val;
    }
    if(x->child[i]->size() == MAX_SIZE) {
      p_splitChild(x,i);
      if(k > x->data[i].k)
        ++i;
    }
    return p_findOrCreate(k, val, x->child[i]);
  }

  sizeT p_depth(CNodeP x) const {
    if(x->leaf)
      return 1;
    sizeT d = 1;
    for(sizeT i = 0; i <= x->size(); ++i) {
      d = std::max(d, p_depth(x->child[i])+1);
    }
    return d;
  }

  void p_clear(NodeP x) {
    if(x->leaf)
      return;
    for(sizeT i = 0; i <= x->size(); ++i) {
      p_clear(x->child[i]);
      delete x->child[i];
    }
  }

public:
  Btree(sizeT t): m_degree(t), m_root(t) {};
  Btree(): Btree(3) {};

  T *search(const K &key) const {
    return p_search(key, &m_root);
  }
  T *insert(const K &key, const T &val) {
    if(m_root.size() == MAX_SIZE)
      p_reroot();
    ++m_size;
    return p_emplace(key, val, &m_root);
  }

  T *operator[](const K &key) {
    if(m_root.size() == MAX_SIZE)
      p_reroot();
    return p_findOrCreate(key, &m_root);
  }

  NodeP const root() { return &m_root; }

  sizeT depth() const {
    return p_depth(&m_root);
  }

  void clear() {
    p_clear(&m_root);
    m_root = NodeT();
    m_size = 0;
  }
};

#endif
