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

    cd /MapViewerOnline/mapviewer.server/plugins/mapviewer.backend
	mv mapviewer.ext.3dobjects.pluginspec mapviewer.ext.3dobjects.pluginspec.disabled
	mv mapviewer.ext.autodrive.pluginspec mapviewer.ext.autodrive.pluginspec.disabled
	mv mapviewer.ext.landmarks.pluginspec mapviewer.ext.landmarks.pluginspec.disabled
	mv mapviewer.ext.obstacles.pluginspec mapviewer.ext.obstacles.pluginspec.disabled
	mv mapviewer.extension.gpx.pluginspec mapviewer.extension.gpx.pluginspec.disabled
	mv mapviewer.extension.kml.pluginspec mapviewer.extension.kml.pluginspec.disabled
	mv mapviewer.extension.locmark.pluginspec mapviewer.extension.locmark.pluginspec.disabled
	mv mapviewer.extension.recorder.pluginspec mapviewer.extension.recorder.pluginspec.disabled
	mv mapviewer.ext.poi.pluginspec mapviewer.ext.poi.pluginspec.disabled

    cd /MapViewerOnline/mapviewer.sessionserver
    /MapViewerOnline/mapviewer.sessionserver/startMvoServer.sh > /mvo-ext-src/log.txt 2>&1 &

    cd /mvo-ext-src/build
fi
