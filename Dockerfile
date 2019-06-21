FROM artifactory.nds-association.org/tooling-dockerreg/afw-builder-ubuntu1804:latest

RUN apt-get update

# Install python runtime
RUN apt-get install -y python3.6 python3-pip

# Package with MapViewerOnline which can be extracted from 
# corresponding MVO image
EXPOSE 5000
ENV NDS_MVO_CFG_DIR /var/opt/nds/mvo
ENV NDS_PLUGIN_PATH /mvo-ext
WORKDIR /MapViewerOnline
ADD ./MapViewerOnline /MapViewerOnline
RUN pip3 install -r python_packages.txt
WORKDIR /MapViewerOnline/mapviewer.sessionserver
CMD ["./startMvoServer.sh"]
