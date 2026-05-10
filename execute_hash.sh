
executable="./exec/program"

if [ ! -d "exec/" ]; then
  mkdir exec
fi

if [ ! -f "$executable" ]; then
  ./compile.sh
fi

element_quantity=9
table_size=11

if [ -n "$1" ]; then
  element_quantity=$1
fi

if [ -n "$2" ]; then
  table_size=$2
fi

echo "Using executable: $executable"
echo "Using element quantity: $element_quantity"
echo "Using table size: $table_size"

TIMEFORMAT=%R

"$executable" --generate_test --table_size "$table_size" --test_quantity 100 --value_quantity "$element_quantity" --reallocation_method 0 --exec_tests 0 > logs/out_generator.log

echo "Test generation completed. Starting tests..."
if [ ! -d "logs/" ]; then
  mkdir logs
fi

echo "Time spent on tests:"
echo "Double Hashing..."
time "$executable" --table_size "$table_size" --test_quantity 100 --value_quantity "$element_quantity" --reallocation_method 0 > logs/out_double_hash.log
echo ""
echo "Brent method..."
time "$executable" --table_size "$table_size" --test_quantity 100 --value_quantity "$element_quantity" --reallocation_method 1 > logs/out_brent.log
echo ""
echo "Randomized..."
time "$executable" --table_size "$table_size" --test_quantity 100 --value_quantity "$element_quantity" --reallocation_method 3 --reallocation_chance 30 > logs/out_randomized.log
echo ""
echo "Binary tree..."
time "$executable" --table_size "$table_size" --test_quantity 100 --value_quantity "$element_quantity" --reallocation_method 2 > logs/out_binary_tree.log
