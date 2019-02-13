#!/bin/sh
echo "Creating directories"
BASEDIR=$PWD/rpmbuild
mkdir $BASEDIR
mkdir $BASEDIR/BUILD
mkdir $BASEDIR/RPMS
mkdir $BASEDIR/SOURCES
mkdir $BASEDIR/SPECS
mkdir $BASEDIR/SRPMS

echo "Preparing files"
echo "%_topdir" $BASEDIR > ~/.rpmmacros
cp MihPhoto.tgz $BASEDIR/SOURCES
cp MihPhoto.spec $BASEDIR/SPECS

echo "Building RPM"
cd $BASEDIR
pwd
rpmbuild -v -ba SPECS/MihPhoto.spec

echo "Deleting base dir: " $BASEDIR
#rm -rf $BASEDIR
rm ~/.rpmmacros