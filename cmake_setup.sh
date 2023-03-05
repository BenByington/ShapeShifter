set -x

rm -rf build/Debug
rm -rf build/Release

mkdir -p build/Debug
mkdir -p build/Release


(cd build/Release; cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS_DEBUG="-g3 -gdwarf-2" -DCMAKE_CXX_FLAGS_DEBUG="-g3 -gdwarf-2 -Wno-everything" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ../.. -DCMAKE_PREFIX_PATH=/opt/local/libexec/qt5/)

(cd build/Debug; cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_FLAGS_DEBUG="-g3 -gdwarf-2" -DCMAKE_CXX_FLAGS_DEBUG="-g3 -gdwarf-2 -Wno-everything" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ../.. -DCMAKE_PREFIX_PATH=/opt/local/libexec/qt5/)

#(cd build/Debug;       cmake -DCMAKE_BUILD_TYPE=Debug          ../..)
#(cd build/Release;     cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ../..)
