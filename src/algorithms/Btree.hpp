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

  void p_joinChild(NodeP x, sizeT i) {
    NodeP y = x->child[i], z = x->child[i+1];
    y->emplace(std::move(x->data[i]));
    if(z->leaf) {
      for(sizeT j = 0; j < z->size(); ++j)
        y->emplace(std::move(z->data[j]));
    }else {
      for(sizeT j = 0; j < z->size(); ++j) {
        y->emplace(std::move(z->data[j]), z->child[j]);
      }
      y->emplace(z->child.back());
    }
    x->data.erase(x->data.begin() + i);
    x->child.erase(x->child.begin() + i + 1);
    delete z;
  }

  void p_reroot() {
    NodeP oldRoot = new NodeT(std::move(m_root));
    m_root.expand(m_degree);
    m_root.child.emplace_back(oldRoot);
    p_splitChild(&m_root,0); // split the oldRoot and place its median as the new root
  }

  void p_compressRoot() {
    NodeP oldRoot = m_root.child[0];
    p_joinChild(&m_root, 0);
    std::swap(m_root, *oldRoot);
    oldRoot->child.clear();
    delete oldRoot;
  }

  T *p_search(const K &k, NodeP x) {
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
      ++m_size;
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

  DataT p_erase(const K &k, NodeP x, bool closer = false) {
    sizeT i = 0;  
    while(i < x->size() && k > x->data[i].k)
      ++i;
    if(!closer && i < x->size() && k == x->data[i].k) {
      DataT ret;
      if(x->leaf) {
        ret = std::move(x->data[i]);
        x->data.erase(x->data.begin() + i);
      }else {
        if(x->child[i]->size() > MIN_SIZE) {
          ret = std::move(x->data[i]);
          x->data[i] = p_erase(k, x->child[i], true);
        }else if(x->child[i+1]->size() > MIN_SIZE) {
          ret = std::move(x->data[i]);
          x->data[i] = p_erase(k, x->child[i+1], true);
        }else {
          p_joinChild(x,i);
          ret = p_erase(k, x->child[i]);
        }
      }
      return ret;
    }else {
      if(x->leaf) {
        sizeT ind = k >= x->data.front().k ? x->data.size()-1 : 0;
        DataT ret = std::move(x->data[ind]);
        x->data.erase(x->data.begin() + i);
        return ret;
      }
      if(x->child[i]->size() == MIN_SIZE) {
        if(i > 0 && x->child[i-1]->size() > MIN_SIZE) {
          NodeP child = x->child[i], neighbor = x->child[i-1];
          child->data.emplace(child->data.begin(), std::move(x->data[i-1]));
          if(!child->leaf) {
            child->child.emplace(child->child.begin(), neighbor->child.back());
            neighbor->child.pop_back();
          }
          x->data[i-1] = std::move(neighbor->data.back());
          neighbor->data.pop_back();
        }else if(i < x->size() && x->child[i+1]->size() > MIN_SIZE) {
          NodeP child = x->child[i], neighbor = x->child[i];
          child->data.emplace_back(std::move(x->data[i]));
          if(!child->leaf) {
            child->child.emplace_back(neighbor->child[0]);
            neighbor->child.erase(neighbor->child.begin());
          }
          x->data[i] = std::move(neighbor->data[0]);
          neighbor->data.erase(neighbor->data.begin());
        }else {
          if(i < x->size())
            p_joinChild(x, i);
          else
            p_joinChild(x, --i);
        }
      }
      return p_erase(k, x->child[i], closer);
    }
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

  T *search(const K &key) {
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

  T erase(const K &key) {
    if(search(key)) {
      if(m_root.size() == 1 && !m_root.leaf && m_root.child[0]->size() == MIN_SIZE && m_root.child[1]->size() == MIN_SIZE)
        p_compressRoot();
      DataT ret = p_erase(key, &m_root);
      return ret.val;
    }
    return T(0);
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
