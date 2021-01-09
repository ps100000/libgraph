ORIG=$PWD
cd ../wasm
./build.sh
cd $ORIG
ng build
