docker pull artifactory.nds-association.org/tooling-dockerreg/td2019-afw-dev:latest
docker run -it \
           -p 5000:5000 \
           -v $PWD:/mvo-ext-src \
           -v $PWD/mvo-cfg:/var/opt/nds/mvo \
           artifactory.nds-association.org/tooling-dockerreg/td2019-afw-dev:latest \
           /bin/sh -c 'cd /mvo-ext-src; ./update-mvo-ext.bash; bash'
