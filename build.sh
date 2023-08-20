mkdir -pv build && cd build
cmake ..
make install-dust install-zip
cp /home/we/dust/code/neomonome/build/src/neomonome.so /home/we/dust/code/neomonome/lib/
