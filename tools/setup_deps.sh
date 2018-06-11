#!/usr/bin/env bash

portaudio_src_url='http://www.portaudio.com/archives/pa_stable_v190600_20161030.tgz'
fftw_src_url='ftp://ftp.fftw.org/pub/fftw/fftw-3.3.8.tar.gz'

function setup_library()
{
    url=$1
    filename=$(basename $url)
    wget $url
    if [[ $? -ne 0 ]]; then
        echo "failed downloading ${filename}"
        exit -1
    fi

    tar -xf ${filename}
    if [[ $? -ne 0 ]]; then
        echo "failed extracting ${filename}"
        exit -1
    fi

    rm ${filename}
    cd */.
    if [[ $? -ne 0 ]]; then
        echo "failed going into extracted directory of ${filename}"
        exit -1
    fi

    ./configure --prefix=$DEPS_DIR
    if [[ $? -ne 0 ]]; then
        echo "failed configuring ${filename}"
        exit -1
    fi

    make -j install
    if [[ $? -ne 0 ]]; then
        echo "failed setting up ${filename}"
        exit -1
    fi
    cd ..
}

TOOL_DIR=$(cd $(dirname $0) && pwd)
ROOT_DIR=$(dirname $TOOL_DIR)
DEPS_DIR="$ROOT_DIR/deps"
TEMP_DIR="$TOOL_DIR/temp"

# start with a clean temp directory
rm -rf ${TEMP_DIR}
mkdir -p ${TEMP_DIR}
cd ${TEMP_DIR}

# setup portaudio
mkdir portaudio
cd portaudio
setup_library ${portaudio_src_url}
cd ${TEMP_DIR}

# setup FFTW
mkdir fftw
cd fftw
setup_library ${fftw_src_url}
cd ${TEMP_DIR}

# cleanup
cd ${TOOL_DIR}
rm -rf ${TEMP_DIR}
