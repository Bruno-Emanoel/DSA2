
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

echo "Using element quantity: $element_quantity"
echo "Using table size: $capacity"

TIMEFORMAT=%R

if [ ! -d "logs/" ]; then
  mkdir logs
fi
if [ ! -d "logs/btree/" ]; then
  mkdir logs/btree
fi

"$executable" --container btree --generate_test --capacity "$capacity" --test_quantity 100 --value_quantity "$element_quantity" --exec_tests 0 > logs/btree/out_generator.log

echo "Test generation completed. Starting tests..."

for i in {1..100}; do
  echo "Running test $i..."
  "$executable" --container btree --capacity "$capacity" --test_quantity 1 --value_quantity "$element_quantity" --exec_tests 1 --print_container < data/tests/$i.in > logs/btree/out_test_$i.log
done
