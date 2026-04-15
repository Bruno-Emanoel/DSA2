
executable="./exec/main_small"

if [ -f "$1" ]; then
  executable=$1
fi

element_quantity=9

if [ -n "$2" ]; then
  element_quantity=$2
fi

echo "Using executable: $executable"
echo "Using element quantity: $element_quantity"

TIMEFORMAT=%R

"$executable" --generate_test --test_quantity 100 --value_quantity "$element_quantity" --reallocation_method 0 > out1


echo "Time spent on tests:"
echo "Double Hashing..."
time "$executable" --test_quantity 100 --value_quantity "$element_quantity" --reallocation_method 0 > out1
echo ""
echo "Brent method..."
time "$executable" --test_quantity 100 --value_quantity "$element_quantity" --reallocation_method 1 > out2
echo ""
echo "Randomized..."
time "$executable" --test_quantity 100 --value_quantity "$element_quantity" --reallocation_method 3 --reallocation_chance 50 > out3
echo ""
echo "Binary tree..."
time "$executable" --test_quantity 100 --value_quantity "$element_quantity" --reallocation_method 2 > out4
