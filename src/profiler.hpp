#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include "util/random.hpp"
#include "algorithms/hashing.hpp"
#include "util/generators.hpp"

class Profiler {
  bool m_generate_tests = false;
  bool m_print_table = false;
  std::string m_test_folder = "data/tests";
  int m_test_quantity = 1;
  int m_value_quantity = 9;
  int m_seed = 47;
  enum Method {
    LINEAR = 1,
    DOUBLE = 2
  } m_method = DOUBLE;
  HashTable<int, int> m_hash_table;
public:
  Profiler(int argc, char** argv) {
    process_parameter(argc, argv);
    profile();
  }

  void add_elements(const std::vector<int> &data) {
    for(auto x : data) {
      m_hash_table.insert(x);
    }
  }

  void profile() {
    if(m_generate_tests) {
      gen_test_cases(m_test_folder, m_test_quantity, m_value_quantity, m_seed);
    }
    
    std::filesystem::path data_path{m_test_folder};
    try {
      double overall_total = 0.0;
      int amount = 0;
      for(const auto& file : std::filesystem::directory_iterator(data_path))
      if(file.is_regular_file()) {

        std::vector<int> data;
        std::ifstream fs(file.path());
        int x;
        while(fs >> x) {
          data.emplace_back(x);
        }
        add_elements(data);
        
        if(m_print_table)
          m_hash_table.print();

        int total = 0.0;
        for (auto &x : data) {
          m_hash_table.search(x);
          total += m_hash_table.get_cnt();
        }
        
        std::cout << std::fixed << std::setprecision(4) << "Average access = " << (total / (double)data.size()) << std::endl;

        m_hash_table.clear();
        
        std::cout << std::endl;
        overall_total += total;
        ++amount;
      }
      std::cout << "Total average = " << (overall_total / (double)amount) << std::endl;
    }catch(std::filesystem::filesystem_error const& err) {
      std::cout << "Error: " << err.what() << std::endl;
    }
  }

  
  void process_parameter(int argc, char** argv) {
    for(int i = 1; i < argc; ++i) {
      std::string str = argv[i];
      if(str == "--generate_test") {
        m_generate_tests = true;
      }else if(str == "--test_folder") {
        m_test_folder = argv[++i];
      }else if(str == "--method") {
        m_method = static_cast<Method>( std::stoi(argv[++i]) );
      }else if(str == "--print_table") {
        m_print_table = true;
      }else if(str == "--seed") {
        m_seed = std::stoi(argv[++i]);
      }else if(str == "--test_quantity") {
        m_test_quantity = std::stoi(argv[++i]);
      }else if(str == "--value_quantity") {
        m_value_quantity = std::stoi(argv[++i]);
      }else if(str == "--reallocation_method") {
        int val = std::stoi(argv[++i]);
        switch (val) {
          case 1:
          m_hash_table.set_brent_realloc();
          break;
          case 2:
          m_hash_table.set_binary_realloc();
          break;
          case 3:
          m_hash_table.set_randomized_realloc();
          break;
          default:
          m_hash_table.set_no_realloc();
          break;
        }
      }
    }
  }
};
