set -x

export CMAKE_PREFIX_PATH=/Users/bbyington/Qt/5.7/clang_64/

rm -rf build/Debug
rm -rf build/Release

mkdir -p build/Debug
mkdir -p build/Release

gcc=/opt/local/bin/gcc-mp-6
gpp=/opt/local/bin/g++-mp-6

clangcc=/opt/local/bin/clang-mp-7.0
clangpp=/opt/local/bin/clang++-mp-7.0

cc=${gcc}
pp=${gpp}


(cd build/Release; cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=${cc} -DCMAKE_CXX_COMPILER=${pp} -DCMAKE_C_FLAGS_DEBUG="-g3 -gdwarf-2" -DCMAKE_CXX_FLAGS_DEBUG="-g3 -gdwarf-2 -Wno-everything" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ../.. -DCMAKE_PREFIX_PATH=/opt/local/libexec/qt5/)

(cd build/Debug; cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=${cc} -DCMAKE_CXX_COMPILER=${pp} -DCMAKE_C_FLAGS_DEBUG="-g3 -gdwarf-2" -DCMAKE_CXX_FLAGS_DEBUG="-g3 -gdwarf-2 -Wno-everything" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ../.. -DCMAKE_PREFIX_PATH=/opt/local/libexec/qt5/)

#(cd build/Debug;       cmake -DCMAKE_BUILD_TYPE=Debug          ../..)
#(cd build/Release;     cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ../..)
