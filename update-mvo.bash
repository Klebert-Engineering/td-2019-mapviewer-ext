# TODO Stop MVO
cd /mvo-ext-src
mkdir -p build
cd build
cmake -DAFW_SDK_DIR=$AFW_SDK_DIR \
      -DCMAKE_PREFIX_PATH=/qt/5.12.2/gcc_64/lib/cmake \
      ../mapviewer.ext
cmake --build .
# TODO Start MVO

