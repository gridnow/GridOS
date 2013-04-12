# !/usr/bin/sh
#
# See the readme.txt at the root directory of this project for the idea and originality of this operating system.
# See the license.txt at the root directory of this project for the copyright information about this file and project.
#

#
# Create Grid OS Build And Test Environment
#

function set_linux_build_env()
{
    echo "Set Build Environment"

    # check MY_BUILD_BASE 
    build_base="$(cat ~/.bashrc | grep 'MY_BUILD_BASE')"
    if [ "$build_base" == '' ]; then
        echo "Set MY_BUILD_BASE"
        my_build_base="$(pwd)"
        echo "Build Base Directory: $my_build_base"
        echo "MY_BUILD_BASE=$my_build_base" >> ~/.bashrc
        echo "export MY_BUILD_BASE" >> ~/.bashrc
        echo "PATH=$(pwd):${PATH}" >> ~/.bashrc
    fi
}

function mk_linux_test_env()
{
    echo "Creaet Test Environment"

    mkdir -p .TestGridOS
    if [ ! -f 'gostpl.img' ]; then
        echo "Error: Grid OS disk image template was not exists!"
        exit 255
    fi

    # expand template image
    if [ ! -f 'hd0.img' ]; then
        qemu convert -f qcow2 -O raw gostpl.img hd0.img
    fi

}

# 创建Linux下的测试环境
function mk_linux_env()
{
    echo "Build System: GNU/Linux System"

    set_linux_build_env
    mk_linux_test_env
}

# 创建Windows下的测试环境
function mk_windows_env()
{
    echo "Windows MinGW/Cygwin System"
}

main() 
{
    echo "Create Grid OS Test Environment"

    OS_TYPE=`uname -o`

    case "$OS_TYPE" in
        'GNU/Linux'*)
            mk_linux_env
            ;;
        MinGW* | Cygwin*)
            mk_windows_env
            ;;
        *)
            echo "Target System: ${OS_TYPE} not support yet!"
            exit 255
    esac
}

main