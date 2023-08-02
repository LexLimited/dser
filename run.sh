rm -rf build/*
cmake -B build
cd build
make
echo "
----------------------------------------
"
time ./dser
