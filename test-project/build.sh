ORIG=$PWD
cd ../angular-graph-lib
./build.sh
cd $ORIG
yarn add ../angular-graph-lib/dist/graph-lib/
