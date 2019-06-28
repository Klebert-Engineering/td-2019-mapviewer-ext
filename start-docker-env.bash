docker pull artifactory.nds-association.org/tooling-dockerreg/td2019-afw-dev:latest
docker run -it \
           -p 5000:5000 \
           -p 80:80 \
           -p 2222:22 \
           -v $PWD:/mvo-ext-src \
           -v $PWD/mvo-cfg:/var/opt/nds/mvo \
           -v /var/lib/data/nds/databases:/var/lib/data/nds/databases \
           artifactory.nds-association.org/tooling-dockerreg/td2019-afw-dev:latest \
           /bin/sh -c 'cd /mvo-ext-src; ./update-mvo-ext.bash; bash'
