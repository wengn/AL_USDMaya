#!/usr/bin/env bash
set -e

mkdir -p $TMP_DIR

#----------------------------------------------
# Checkout, build and install USD
# TODO: use a zipped version of AL_USDMaya to have a more consistant run
#----------------------------------------------

export PATH=$MAYA_LOCATION/bin:$PATH

cd $TMP_DIR &&\
  cd AL_USDMaya &&\
    rm -Rf build &&\
    mkdir build &&\
    cd build  &&\
      echo "MayaLocation=${MAYA_LOCATION}"
      echo "UsdPxrConfig=$BUILD_DIR/usd/${USD_VERSION}/pxrConfig.cmake"
      cmake -Wno-dev \
            -DCMAKE_INSTALL_PREFIX=$BUILD_DIR \
            -DCMAKE_MODULE_PATH=$BUILD_DIR \
            -DCMAKE_BUILD_TYPE=RelWithDebInfo \
            -DBOOST_ROOT=$BUILD_DIR \
            -DMAYA_LOCATION=$MAYA_LOCATION \
            -DGTEST_ROOT=$BUILD_DIR \
            -DUSD_CONFIG_FILE=$BUILD_DIR/usd/${USD_VERSION}/pxrConfig.cmake \
            -DCMAKE_PREFIX_PATH=$MAYA_LOCATION/lib/cmake \
            -DUSD_MAYA_ROOT=$BUILD_DIR/usd/${USD_VERSION} \
            ..
      make -j ${BUILD_PROCS} install
      ctest -V
    cd ..
    rm -rf build
