#include "src/algorithms/Btree.hpp"
#include <iostream>
#include <sstream>
#include <string>

// Source - https://stackoverflow.com/a/36848138
// Posted by DarthGizka, modified by community. See post 'Timeline' for change history
// Retrieved 2026-05-10, License - CC BY-SA 3.0

class BTreePrinter {
   using Tree = Btree<int,int>;  
   using Node = Tree::NodeP;
   struct NodeInfo
   {
      std::string text;
      unsigned text_pos, text_end;  // half-open range
   };

   typedef std::vector<NodeInfo> LevelInfo;

   std::vector<LevelInfo> levels;

   std::string node_text (Node const node);

   void before_traversal ()
   {
      levels.resize(0);
      levels.reserve(10);   // far beyond anything that could usefully be printed
   }

   void visit (Node const node, unsigned level = 0, unsigned child_index = 0);

   void after_traversal ();

public:
   void print (Tree &tree) {
      before_traversal();
      visit(tree.root());
      after_traversal();
   }
};

void BTreePrinter::visit (Node const node, unsigned level, unsigned child_index)
{
   if (level >= levels.size())
      levels.resize(level + 1);

   LevelInfo &level_info = levels[level];
   NodeInfo info;

   info.text_pos = 0;
   if (!level_info.empty())  // one blank between nodes, one extra blank if left-most child
      info.text_pos = level_info.back().text_end + (child_index == 0 ? 2 : 1);

   info.text = node_text(node);

   if (node->leaf)
   {
      info.text_end = info.text_pos + unsigned(info.text.length());
   }
   else // non-leaf -> do all children so that .text_end for the right-most child becomes known
   {
      for (unsigned i = 0, e = unsigned(node->size()); i <= e; ++i)  // one more pointer than there are keys
         visit(node->child[i], level + 1, i);

      info.text_end = levels[level + 1].back().text_end;
   }

   levels[level].push_back(info);
}

std::string BTreePrinter::node_text (Node const node) {
   std::ostringstream os;
   char const *sep = "";

   os << "[";
   for (unsigned i = 0; i < node->size(); ++i, sep = " ")
      os << sep << node->data[i].k;
   os << "]";

   return os.str();
}

void print_blanks (unsigned n)
{
   while (n--)
      std::cout << ' ';
}

void BTreePrinter::after_traversal ()
{
   for (std::size_t l = 0, level_count = levels.size(); ; )
   {    
      auto const &level = levels[l];
      unsigned prev_end = 0;

      for (auto const &node: level)
      {         
         unsigned total = node.text_end - node.text_pos;
         unsigned slack = total - unsigned(node.text.length());
         unsigned blanks_before = node.text_pos - prev_end;

         print_blanks(blanks_before + slack / 2);
         std::cout << node.text;

         if (&node == &level.back())
            break;

         print_blanks(slack - slack / 2);

         prev_end += blanks_before + total;
      }

      if (++l == level_count)
         break;

      std::cout << "\n\n";
   }

   std::cout << "\n";
}

/*
int main() {
   using namespace std;
   Btree tree;
   int n; cin >> n;
   for(int i = 0; i < n; ++i) {
      int x; cin >> x;
      tree.insert(x,x);
   }
   int q; cin >> q;
   for(int i = 0; i < q; ++i) {
      int x; cin >> x;
      tree.erase(x);
   }
}
*/