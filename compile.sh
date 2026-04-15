g++ main.cpp -O2 -o exec/main_small -I.
g++ main.cpp -O2 -o exec/main_medium -I. -DTABLE_SIZE=101
g++ main.cpp -O2 -o exec/main_large -I. -DTABLE_SIZE=1009
g++ main.cpp -O2 -o exec/main_xlarge -I. -DTABLE_SIZE=10007