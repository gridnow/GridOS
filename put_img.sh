# !/usr/bin/sh
#
# See the readme.txt at the root directory of this project for the idea and originality of this operating system.
# See the license.txt at the root directory of this project for the copyright information about this file and project.
#

# 存放目标文件到测试镜像中
# 工作原理
#   找到当前目录下的Makefile
#   从Makefile中提取TARGET
#   格式是:
#       TARGET = XXX
#   从中提取XXX
#   如果是Linux直接挂载镜像到指定目录下
#   然后拷贝XXX文件到指定目录下
#   卸载目录
#   完成Linux的安装到目录下的功能
#   如果是Windows
#   借助winimage工具将目标文件写入到镜像中
#

PWD=`pwd`

MKFILE=${PWD}/Makefile

TARGET_FILE=`cat ${MKFILE} | grep -E '^TARGET' | awk '{print $NF}'`

TARGET_FILE=${PWD}/$TARGET_FILE
# 目标文件是否存在
if [ ! -f "${TARGET_FILE}" ]; then
    echo "Target file: ${TARGET_FILE} was not exists!"
    exit -1
fi

# try mount the system test image
if [ ! -f "hd0.img" ]; then
    mount -t vfat -o offset=32256 $MY_BUILD_BASE/hd0.img $MY_BUILD_BASE/.TestGridOS
    cp $TARGET_FILE $MY_BUILD_BASE/.TestGridOS/.
    umount $MY_BUILD_BASE/.TestGridOS
fi

