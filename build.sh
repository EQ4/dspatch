#!/bin/bash
src_dir=$(dirname $(readlink -f "$0"))
build_dir=${src_dir}/build

if [ -f "/usr/bin/ninja" ] ; then
  GENERATOR="Ninja"
  BUILD_COMMAND="ninja"
else
  GENERATOR="Unix Makefiles"
  BUILD_COMMAND="make"
fi

rm -r -f ${build_dir}
mkdir -p ${build_dir}
cd ${build_dir}
cmake ${src_dir} -G "${GENERATOR}"
${BUILD_COMMAND}
