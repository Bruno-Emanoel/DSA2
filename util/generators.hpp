#include <cstdlib>

void gen_test_cases(std::string data_location, int n, int value_quantity, int seed, int max_val = 1e2) {
  namespace fs = std::filesystem;
  using namespace std;
  
  fs::path data_path(data_location);

  try {
    if (fs::exists(data_path)) {
      for(const auto &file : fs::directory_iterator(data_path)) {
        fs::remove_all(file.path());
      }
      cout << "Data folder cleared." << endl;
    } else {
      fs::create_directory(data_path);
      cout << "Data folder created" << endl;
    }
  } catch (const fs::filesystem_error& ex) {
    cerr << "Error: " << ex.what() << endl;
    exit(0);
  }
  
  rng = mt19937(seed);
  uniform = uniform_int_distribution<int>(1, max_val);

  cout << "Generating test cases..." << endl;
  for(int i = 0; i < n; ++i) {
    cout << "Test case #" << i+1 << endl;

    string file_name = data_location;
    if(data_location[data_location.size()-1]!='/')
      file_name += "/";
    file_name += to_string(i+1) + ".in";
    ofstream data_file(file_name);

    for(int j = 0; j < value_quantity; ++j) {
      data_file << uniform(rng);
      if(j<value_quantity-1)
        data_file << " ";
    }

    cout << "Done #" << i+1 << endl;
  }

}
