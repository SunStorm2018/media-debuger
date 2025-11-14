# SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.16)

# 包含CMake模块路径
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Qt版本检测和配置
include(FindQtCompat)

# 项目信息
if(QT_MAJOR_VERSION EQUAL 6)
    project(QJsonModel VERSION 0.0.2 LANGUAGES CXX
        DESCRIPTION "QJsonModel is a json tree model class for Qt6/C++17 based on QAbstractItemModel. MIT License."
    )
else()
    project(QJsonModel VERSION 0.0.2 LANGUAGES CXX
        DESCRIPTION "QJsonModel is a json tree model class for Qt5/C++17 based on QAbstractItemModel. MIT License."
    )
endif()

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt标准项目设置
qt_compat_standard_project_setup()

# 启用Qt MOC
set(CMAKE_AUTOMOC ON)

# 创建QJsonModel对象库
add_library(QJsonModel OBJECT 
    QJsonModel.cpp 
    include/QJsonModel.hpp
    include/details/QUtf8.hpp
)

# 设置包含目录
target_include_directories(QJsonModel PUBLIC include)

# 链接Qt库
target_link_libraries(QJsonModel PUBLIC
    ${QT_CORE_TARGET}
    ${QT_GUI_TARGET}
    ${QT_WIDGETS_TARGET}
)
add_library(${QT_TARGET_PREFIX}::QJsonModel ALIAS QJsonModel)

# 创建静态库
add_library(QJsonModelStatic STATIC)
add_library(${QT_TARGET_PREFIX}::QJsonModelStatic ALIAS QJsonModelStatic)

if(WIN32)
    set_target_properties(QJsonModelStatic PROPERTIES OUTPUT_NAME "QJsonModelLIB")
else()
    set_target_properties(QJsonModelStatic PROPERTIES OUTPUT_NAME "QJsonModel")
endif()
target_link_libraries(QJsonModelStatic PUBLIC QJsonModel)

# 创建共享库
add_library(QJsonModelShared SHARED)
add_library(${QT_TARGET_PREFIX}::QJsonModelShared ALIAS QJsonModelShared)

set_target_properties(QJsonModelShared PROPERTIES OUTPUT_NAME "QJsonModel")
target_link_libraries(QJsonModelShared PUBLIC QJsonModel)

# 设置编译定义
target_compile_definitions(QJsonModel PRIVATE
    ${QT_COMPAT_DEFINES}
)

# 安装规则
install(TARGETS QJsonModel QJsonModelStatic QJsonModelShared
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES 
    include/QJsonModel.hpp 
    include/details/QUtf8.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/QJsonModel
)

# vim: ts=2 sw=2 noet foldmethod=indent :
