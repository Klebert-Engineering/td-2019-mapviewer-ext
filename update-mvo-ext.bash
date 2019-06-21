# Refresh build of plugin
cd /mvo-ext-src
mkdir -p build
cd build
cmake -DAFW_SDK_DIR=$AFW_SDK_DIR \
      -DCMAKE_PREFIX_PATH=/qt/5.12.2/gcc_64/lib/cmake \
      ../mapviewer.ext
cmake --build .

# Start MVO on demand
MVO_RUNNING=$(ps | grep startMvoServer)
if [ -z "$MVO_RUNNING" ]; then
    echo "Starting MVO in the background..."
    export NDS_PLUGIN_PATH=/mvo-ext-src/build
    cd /MapViewerOnline/mapviewer.sessionserver
    /MapViewerOnline/mapviewer.sessionserver/startMvoServer.sh > /dev/null 2>&1 &
    cd -
fi
