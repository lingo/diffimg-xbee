#!/bin/sh
rpmdev-setuptree
cp MihPhoto.tgz ~/rpmbuild/SOURCES
cp MihPhoto.spec ~/rpmbuild/SPECS
cd ~/rpmbuild
rpmbuild -v -ba --clean SPECS/MihPhoto.spec
#rm -rf ~/rpmbuild