#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include "src/util/random.hpp"
#include "src/util/generators.hpp"
#include "src/algorithms/hashing.hpp"
#include "src/algorithms/Btree.hpp"
#include "src/util/BtreePrinter.hpp"

class Profiler {
  bool m_generate_tests = false;
  bool m_print_container = false;
  std::string m_test_folder = "data/tests";
  int m_test_quantity = 1;
  int m_value_quantity = 9;
  int m_capacity = 11;
  int m_seed = 47;
  bool m_exec_tests = 1;
  enum Method {
    LINEAR = 1,
    DOUBLE = 2
  } m_method = DOUBLE;
  HashTable<int, int> *m_hash_table;
  Btree<int,int> *m_btree;
  std::string m_container_type;
public:
  Profiler(int argc, char** argv) {
    process_parameter(argc, argv);
    profile();
  }

  void add_elements(const std::vector<int> &data) {
    if(m_container_type == "hash")
      for(auto x : data) {
        m_hash_table->insert(x);
      }
    else
      for(auto x : data) {
        m_btree->insert(x,x);
        print_container();
      }
  }

  void print_container() {
    if(m_container_type == "hash")
      m_hash_table->print();
    else {
      std::cout << std::endl;
      BTreePrinter printer;
      printer.print(*m_btree);
      std::cout << std::endl;
    }
  }

  void profile() {
    if(m_generate_tests) {
      gen_test_cases(m_test_folder, m_test_quantity, m_value_quantity, m_seed, m_capacity*m_capacity);
    }
    
    if(!m_exec_tests)
      return;

    std::cout << "Starting profiling with " << m_container_type << std::endl;

    std::filesystem::path data_path{m_test_folder};
    try {
      double overall_total = 0.0;
      int amount = 0;
      for(const auto& file : std::filesystem::directory_iterator(data_path))
      if(file.is_regular_file()) {
        std::vector<int> data;
        if(m_container_type == "hash") {
          std::ifstream fs(file.path());
          int x;
          while(fs >> x)
            data.emplace_back(x);
        }else {
          int x;
          while(std::cin >> x)
            data.emplace_back(x);
        }
        
        if(m_print_container) {
          std::cout << "elements added:" << std::endl;
          for(auto x : data)
            std::cout << x << ' ';
          std::cout << std::endl;
        }
        add_elements(data);

        if(m_container_type == "hash") {
          int total = 0.0;
          for (auto &x : data) {
            m_hash_table->search(x);
            total += m_hash_table->get_cnt();
          }
          
          std::cout << std::fixed << std::setprecision(4) << "Average access = " << (total / (double)data.size()) << std::endl;
          
          m_hash_table->clear();
          
          std::cout << std::endl;
          overall_total += total / (double) data.size();
          ++amount;
        }else {
          std::cout << "B-Tree height = " << m_btree->depth() << std::endl;
          std::cout << std::endl << "Starting random deleting" << std::endl;
          std::vector<int> toDel = selectRandom(data, data.size()/4);
          for(auto x : toDel)
            std::cout << x << ' ';
          std::cout << std::endl;
          for(auto x : toDel) {
            m_btree->erase(x);
            if(m_print_container)
              print_container();
          }
          m_btree->clear();
          return;
        }
      }
      std::cout << "Total average = " << (overall_total / (double)amount) << std::endl;
    }catch(std::filesystem::filesystem_error const& err) {
      std::cout << "Error: " << err.what() << std::endl;
    }
  }

  
  void process_parameter(int argc, char** argv) {
    if(argc<=1) {
      std::cout << "You should provide the container type as follows: {executable} --container {hash|btree}" << std::endl;
      exit(-1);
    }
    std::string str = argv[1];
    if(str!="--container") {
      std::cout << "The first parameter should be the container type." << std::endl;
      exit(-1);
    }
    for(int i = 1; i < argc; ++i) {
      str = argv[i];
      if(str == "--container") {
        m_container_type = argv[++i];
        if(m_container_type == "hash")
          m_hash_table = new HashTable<int,int>();
        else if(m_container_type == "btree")
          m_btree = new Btree();
        else {
          std::cout << "Container not found" << std::endl;
          exit(-1);
        }
      }else if(str == "--generate_test") {
        m_generate_tests = true;
      }else if(str == "--test_folder") {
        m_test_folder = argv[++i];
      }else if(str == "--method") {
        m_method = static_cast<Method>( std::stoi(argv[++i]) );
      }else if(str == "--print_container") {
        m_print_container = true;
      }else if(str == "--seed") {
        m_seed = std::stoi(argv[++i]);
        rng = std::mt19937(m_seed);
      }else if(str == "--test_quantity") {
        m_test_quantity = std::stoi(argv[++i]);
      }else if(str == "--value_quantity") {
        m_value_quantity = std::stoi(argv[++i]);
      }else if(str == "--capacity") {
        m_capacity = std::stoi(argv[++i]);
        if(m_container_type == "hash")
          m_hash_table->resize(m_capacity);
      }else if(str == "--exec_tests") {
        m_exec_tests = std::stoi(argv[++i])!=0;
      }else if(str == "--reallocation_method") {
        int val = std::stoi(argv[++i]);
        switch (val) {
          case 1:
          m_hash_table->set_brent_realloc();
          break;
          case 2:
          m_hash_table->set_binary_realloc();
          break;
          case 3:
          m_hash_table->set_randomized_realloc();
          break;
          default:
          m_hash_table->set_no_realloc();
          break;
        }
      }else if(str == "--reallocation_chance") {
        m_hash_table->set_reallocation_chance(std::stoi(argv[++i]));
      }
    }
  }
};
