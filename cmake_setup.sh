
export CMAKE_PREFIX_PATH=/Users/bbyington/Qt/5.7/clang_64/

rm -rf build/Debug
rm -rf build/Release

mkdir -p build/Debug
mkdir -p build/Release


(cd build/Debug;       cmake -DCMAKE_BUILD_TYPE=Debug          ../..)
(cd build/Release;     cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ../..)
