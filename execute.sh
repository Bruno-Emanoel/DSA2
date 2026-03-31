g++ -I. main.cpp
./a.out --generate_test --print_table --test_quantity 100 --reallocation_method 0 > out1
./a.out --print_table --test_quantity 100 --reallocation_method 1 > out2
./a.out --print_table --test_quantity 100 --reallocation_method 2 > out3
./a.out --print_table --test_quantity 100 --reallocation_method 3 > out4
