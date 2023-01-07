rm -rf cbuild
mkdir -p cbuild
cd cbuild
cmake -DCMAKE_TOOLCHAIN_FILE=../bbb_toolchain.cmake ..
make

mkdir -p ../bin
cp pinmux ../bin