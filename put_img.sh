# !/usr/bin/sh
#
# See the readme.txt at the root directory of this project for the idea and originality of this operating system.
# See the license.txt at the root directory of this project for the copyright information about this file and project.
#

# ���Ŀ���ļ������Ծ�����
# ����ԭ��
#   �ҵ���ǰĿ¼�µ�Makefile
#   ��Makefile����ȡTARGET
#   ��ʽ��:
#       TARGET = XXX
#   ������ȡXXX
#   �����Linuxֱ�ӹ��ؾ���ָ��Ŀ¼��
#   Ȼ�󿽱�XXX�ļ���ָ��Ŀ¼��
#   ж��Ŀ¼
#   ���Linux�İ�װ��Ŀ¼�µĹ���
#   �����Windows
#   ����winimage���߽�Ŀ���ļ�д�뵽������
#

PWD=`pwd`

MKFILE=${PWD}/Makefile

TARGET_FILE=`cat ${MKFILE} | grep -E '^TARGET' | awk '{print $NF}'`

TARGET_FILE=${PWD}/$TARGET_FILE
# Ŀ���ļ��Ƿ����
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

