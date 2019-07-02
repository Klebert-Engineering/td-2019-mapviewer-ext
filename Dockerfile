FROM artifactory.nds-association.org/tooling-dockerreg/afw-builder-ubuntu1804:latest

RUN apt-get update

# Install python runtime
RUN apt-get install -y python3.6 python3-pip psmisc openssh-server

# Package with MapViewerOnline which can be extracted from 
# corresponding MVO image
EXPOSE 5000
ENV NDS_MVO_CFG_DIR /var/opt/nds/mvo
ENV NDS_PLUGIN_PATH /mvo-ext
WORKDIR /MapViewerOnline
ADD ./MapViewerOnline /MapViewerOnline
RUN pip3 install -r python_packages.txt
WORKDIR /MapViewerOnline/mapviewer.sessionserver

# Add NDS AFW SDK which can be extracted from official Ubuntu1804
# release of the DatabaseInspector
ENV AFW_SDK_DIR /ndsafw-sdk/sdk
ADD ./ndsafw-sdk /ndsafw-sdk
