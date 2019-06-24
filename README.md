# td-2019-mapviewer-ext

Starting point for the MapViewer extension workshop at the Tooling Days 2019.

This repository contains a docker container that provides ...
* Ubuntu 18.04 with Qt, GCC & NDS MVO 2019.4.0
* Control scripts to launch the container, and build a custom MapViewer extension inside of it.


## Prerequisites

Make sure that you have installed docker, and that you can run `docker` without sudo. Alternatively, run `./start-docker-env.sh` with elevated privileges.

## Getting started

Just launch `./start-docker-env.sh`. It will prompt you to enter Artifacotry credentials, and pull the pre-built container from NDS artifatory.

> **Hint for Windows Users:** The `$PWD` variable used in the script is not working with Docker in the Git Shell. Just replace it with a path to this directory.

After the container has been pulled, it will drop you into a shell inside the container.

In your desktop environment, open a browser and connect to `localhost:5000`. You should receive a new MapViewerOnline Session hosted by the container.

Note, that the map configured as *"map1"* is intentionally left as a dummy - adjust mvo-cfg/mapviewerserver.json to add your own map!

## Compiling the extension

Inside the container shell, just call `./update-mvo-ext.sh` whenever you want to recompile. If compilation succeeds, you can simply reload your browser MVO session to see your updated extension at work!

## Inspecting logs

After starting the container, a log file should be written under `mapviewer.ext/log.txt`. Inspect it inside the container with `tail -f log.txt`, or just view it from outside.