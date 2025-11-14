#!/bin/bash

# SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
# SPDX-License-Identifier: MIT

# MediaDebugger CMake构建脚本

set -e

# 默认参数
BUILD_TYPE="Release"
BUILD_DIR="build"
INSTALL_PREFIX="/usr"
QT_VERSION=""

# 解析命令行参数
while [[ $# -gt 0 ]]; do
    case $1 in
        -d|--debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        -c|--clean)
            CLEAN=true
            shift
            ;;
        -i|--install)
            INSTALL=true
            shift
            ;;
        -p|--prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        -q|--qt-version)
            QT_VERSION="$2"
            shift 2
            ;;
        -h|--help)
            echo "用法: $0 [选项]"
            echo ""
            echo "选项:"
            echo "  -d, --debug         构建调试版本"
            echo "  -c, --clean         清理构建目录"
            echo "  -i, --install       构建后安装"
            echo "  -p, --prefix DIR    设置安装前缀 (默认: /usr)"
            echo "  -q, --qt-version N 指定Qt版本 (5 或 6)"
            echo "  -h, --help          显示此帮助信息"
            echo ""
            echo "示例:"
            echo "  $0                  # 构建发布版本"
            echo "  $0 -d               # 构建调试版本"
            echo "  $0 -i -p /usr/local # 构建并安装到/usr/local"
            echo "  $0 -q 5             # 强制使用Qt5"
            exit 0
            ;;
        *)
            echo "未知选项: $1"
            echo "使用 -h 或 --help 查看帮助"
            exit 1
            ;;
    esac
done

# 清理构建目录
if [[ "$CLEAN" == true ]]; then
    echo "清理构建目录..."
    rm -rf $BUILD_DIR
    echo "清理完成"
    exit 0
fi

# 创建构建目录
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# 配置CMake参数
CMAKE_ARGS=(
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE
    -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX
)

if [[ -n "$QT_VERSION" ]]; then
    CMAKE_ARGS+=(-DQT_VERSION_MAJOR=$QT_VERSION)
fi

echo "配置CMake..."
echo "构建类型: $BUILD_TYPE"
echo "安装前缀: $INSTALL_PREFIX"
if [[ -n "$QT_VERSION" ]]; then
    echo "Qt版本: $QT_VERSION"
fi

# 运行CMake配置
cmake "${CMAKE_ARGS[@]}" ..

# 编译
echo "开始编译..."
make -j$(nproc)

echo "编译完成!"

# 安装
if [[ "$INSTALL" == true ]]; then
    echo "安装到 $INSTALL_PREFIX..."
    if [[ "$INSTALL_PREFIX" == "/usr" || "$INSTALL_PREFIX" == "/usr/local" ]]; then
        sudo make install
    else
        make install
    fi
    echo "安装完成!"
fi

echo ""
echo "可执行文件位置: $BUILD_DIR/bin/"
if [[ "$BUILD_TYPE" == "Debug" ]]; then
    EXECUTABLE="media-debuger-qt5"
else
    EXECUTABLE="media-debuger-qt5"
fi

# 根据实际构建的Qt版本确定可执行文件名
if [[ -f "bin/media-debuger-qt6" ]]; then
    EXECUTABLE="media-debuger-qt6"
elif [[ -f "bin/media-debuger-qt5" ]]; then
    EXECUTABLE="media-debuger-qt5"
fi

echo "运行程序: ./$BUILD_DIR/bin/$EXECUTABLE"