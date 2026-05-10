
executable="./exec/program"

if [ ! -d "exec/" ]; then
  mkdir exec
fi

if [ -f "$executable" ]; then
  rm "$executable"
fi
./compile.sh

element_quantity=9
capacity=11

if [ -n "$1" ]; then
  element_quantity=$1
fi

if [ -n "$2" ]; then
  capacity=$2
fi

echo "Using executable: $executable"
echo "Using element quantity: $element_quantity"
echo "Using table size: $capacity"

TIMEFORMAT=%R

if [ ! -d "logs/" ]; then
  mkdir logs
fi
if [ ! -d "logs/hash/" ]; then
  mkdir logs/hash
fi

"$executable" --container hash --generate_test --capacity "$capacity" --test_quantity 100 --value_quantity "$element_quantity" --reallocation_method 0 --exec_tests 0 > logs/hash/out_generator.log

echo "Test generation completed. Starting tests..."

echo "Time spent on tests:"
echo "Double Hashing..."
time "$executable" --container hash --capacity "$capacity" --test_quantity 100 --value_quantity "$element_quantity" --reallocation_method 0 > logs/hash/out_double_hash.log
echo ""
echo "Brent method..."
time "$executable" --container hash --capacity "$capacity" --test_quantity 100 --value_quantity "$element_quantity" --reallocation_method 1 > logs/hash/out_brent.log
echo ""
echo "Randomized..."
time "$executable" --container hash --capacity "$capacity" --test_quantity 100 --value_quantity "$element_quantity" --reallocation_method 3 --reallocation_chance 30 > logs/hash/out_randomized.log
echo ""
echo "Binary tree..."
time "$executable" --container hash --capacity "$capacity" --test_quantity 100 --value_quantity "$element_quantity" --reallocation_method 2 > logs/hash/out_binary_tree.log
