#!/usr/bin/env bash
set -ex

# Create build subdirectories
buildDir="`pwd`"
buildDirs="api_include api_lib bin lib include man/man3 doc"
for dir in ${buildDirs}; do
	mkdir -p ${dir}
done

pushd sources
make --print-directory WITH_FLEXLM=NOFLEX            \
  ALLIANCE_TOP=${buildDir}         \
  AVERTEC_TOP=${buildDir}          \
  AVERTEC_OS=`uname -s`            \
  AVERTEC_LICENSE=AVERTEC_DUMMY    \
  AVT_LICENSE_SERVER=house         \
  AVT_LICENSE_FILE=27009@house     \
  AVT_COMPILATION_TYPE=distrib     \
  AVT_DISTRIB_DIR=${buildDir}      \
  PACKAGING_TOP=${localInstall}    \
  SAXON="java -jar ${buildDir}/distrib_extras/saxon9.jar" \
  2>&1 | tee ../build.log
popd

#  LEX=${localInstall}/bin/flex     \

# 'Install'
mkdir -p install
installDir="`pwd`/install"

mkdir -p ${installDir}/bin
mkdir -p ${installDir}/share/tasyag/etc

for conf in avt.slib avttools.dtb Xtas Xyagle trmodel.cfg; do
  cp etc/$conf ${installDir}/share/tasyag/etc
done

TOOLS="avt_shell avtman ttvdiff ttvren"
if [ ${GRAPHICAL+x} ] ; then
  TOOLS="$TOOLS xtas xyargle"
fi

for tool in $TOOLS; do
  cp bin/${tool} ${installDir}/bin
done

cp -r tcl ${installDir}/share/tasyag

echo "AVERTEC_TOP=`pwd`/install/share/tasyag" > "${installDir}/avt_env.sh"
echo 'PATH=${AVERTEC_TOP}/tcl:${PATH}' >> "${installDir}/avt_env.sh"
echo 'export AVERTEC_TOP' >> "${installDir}/avt_env.sh"
