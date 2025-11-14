# MediaDebuger

MediaDebuger 是一个基于 Qt 的多媒体文件分析调试工具，主要用于查看和分析多媒体文件的各种信息，包括格式、编码、流信息等。该项目使用 FFprobe 作为底层工具来获取媒体文件的详细信息，并通过图形界面展示给用户。

## 功能特性

- **全面的媒体分析**:
  - 通过 FFprobe 集成获取详细的格式、编码器和流信息
  - 帧级元数据检查
  - 编解码器参数和能力分析

- **高级UI组件**:
  - 媒体播放器(PlayerWG): 嵌入式播放器，支持跳转和帧分析
  - 搜索面板(SearchWG): 支持跨媒体属性的多列搜索
  - 日志查看器(LogWG): 带文本高亮的实时日志查看
  - JSON格式化工具: 美观展示和分析媒体元数据
  - 表格视图: 结构化展示媒体属性

- **核心功能**:
  - UI组件间的状态同步
  - 可配置的多级别日志系统
  - 最近打开文件的历史记录管理
  - 导出为JSON、CSV等多种格式
  - 多语言支持(中英文)

- **技术亮点**:
  - 日志分析的自定义文本高亮
  - UI状态同步系统
  - 异步命令执行
  - 数据展示的模型-视图架构

## 安装说明

### 系统要求

- **CMake**: 3.16 或更高版本
- **Qt**: Qt 5.15+ 或 Qt 6.x
- **FFmpeg**: 包含开发库和 pkg-config 文件
- **编译器**: 支持C++17的GCC、Clang或MSVC
- **X11**: Linux桌面集成支持（libx11-dev）

### Linux系统依赖

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential cmake pkg-config
sudo apt install qt5-default libqt5widgets5 libqt5gui5 libqt5core5
sudo apt install qt6-base-dev qt6-tools-dev
sudo apt install libavcodec-dev libavformat-dev libavutil-dev
sudo apt install libswscale-dev libswresample-dev libavfilter-dev
sudo apt install libx11-dev

# CentOS/RHEL/Fedora
sudo yum groupinstall "Development Tools"
sudo yum install cmake pkg-config
sudo yum install qt5-qtbase-devel qt5-qttools-devel
sudo yum install qt6-qtbase-devel qt6-qttools-devel
sudo yum install ffmpeg-devel
sudo yum install libX11-devel

# Arch Linux
sudo pacman -S base-devel cmake pkg-config
sudo pacman -S qt5-base qt5-tools
sudo pacman -S qt6-base qt6-tools
sudo pacman -S ffmpeg
sudo pacman -S libx11
```

### 构建方法

#### 方法1：使用构建脚本（推荐）

项目提供了功能完善的构建脚本 `build.sh`：

```bash
# 基本构建（发布版本）
./build.sh

# 调试版本构建
./build.sh -d

# 构建并安装
./build.sh -i

# 指定Qt版本
./build.sh -q 6

# 自定义安装前缀
./build.sh -p /usr/local

# 清理构建目录
./build.sh -c

# 查看所有可用选项
./build.sh -h
```

**构建脚本选项：**
- `-d, --debug`: 构建调试版本而非发布版本
- `-c, --clean`: 构建前清理构建目录
- `-i, --install`: 构建后安装（系统目录需要sudo权限）
- `-p, --prefix DIR`: 设置安装前缀（默认：/usr）
- `-q, --qt-version N`: 强制指定Qt版本（5或6）
- `-h, --help`: 显示帮助信息

**附加脚本：**
- `clean_build_cache.sh`: 清理所有构建产物和缓存文件

**构建脚本使用示例：**

```bash
# 开发工作流 - 清理、调试构建、测试
./build.sh -c -d

# 生产构建并自定义安装路径
./build.sh -p /opt/media-debuger -i

# 为遗留系统强制Qt5构建
./build.sh -q 5 -c

# 快速重新构建（不清理）
./build.sh

# 测试构建（调试版本 + 自定义路径）
./build.sh -d -p ~/local/media-debuger
```

**构建脚本特性：**
- **自动Qt检测**：自动检测Qt6，回退到Qt5
- **并行编译**：使用所有可用的CPU核心（`-j$(nproc)`）
- **智能安装**：系统目录自动使用sudo
- **可执行文件检测**：自动检测构建的Qt版本
- **错误处理**：提供清晰的错误消息和退出码

#### 方法2：手动CMake构建

1. 克隆仓库：

   ```bash
   git clone https://gitee.com/sunstom/media-debuger.git
   cd media-debuger
   ```

2. 创建构建目录：

   ```bash
   mkdir build && cd build
   ```

3. 配置项目：

   ```bash
   # 自动检测Qt版本（优先Qt6）
   cmake ..

   # 指定Qt版本
   cmake -DQT_VERSION_MAJOR=6 ..  # 使用Qt6
   cmake -DQT_VERSION_MAJOR=5 ..  # 使用Qt5

   # 设置构建类型
   cmake -DCMAKE_BUILD_TYPE=Release ..

   # 设置安装前缀
   cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..

   # 启用详细输出用于调试
   cmake -DCMAKE_VERBOSE_MAKEFILE=ON ..
   ```

4. 编译：

   ```bash
   # 使用所有CPU核心
   make -j$(nproc)

   # 或在Windows上使用
   cmake --build . --config Release

   # 启用详细输出
   make VERBOSE=1
   ```

5. 安装（可选）：

   ```bash
   # Linux/macOS
   sudo make install

   # 或使用CMake
   sudo cmake --install .

   # Windows（需要管理员权限）
   cmake --install . --config Release
   ```

6. 运行应用程序：

   ```bash
   # 从构建目录
   ./bin/media-debuger-qt6  # Qt6版本
   ./bin/media-debuger-qt5  # Qt5版本

   # 或安装后
   media-debuger-qt6
   ```

### 构建选项

| 选项 | 默认值 | 说明 |
|------|--------|------|
| `CMAKE_BUILD_TYPE` | `Release` | 构建类型：Release/Debug/RelWithDebInfo/MinSizeRel |
| `CMAKE_INSTALL_PREFIX` | `/usr` | 安装前缀 |
| `QT_VERSION_MAJOR` | `自动检测` | 强制Qt版本：5或6 |
| `CMAKE_VERBOSE_MAKEFILE` | `OFF` | 启用详细makefile输出 |

### 构建类型

- **Release**: 优化版本，无调试信息（默认）
- **Debug**: 调试版本，包含调试信息且无优化
- **RelWithDebInfo**: 优化版本，包含调试信息
- **MinSizeRel**: 最小体积优化版本

### Qt5/Qt6兼容性

项目通过多种机制提供全面的Qt5/Qt6兼容性：

#### Qt6支持
- 完全兼容Qt6 API，具备现代特性
- 使用Qt6改进的性能和功能
- 推荐用于新部署
- 自动UTF-8编码支持
- 增强的正则表达式引擎
- 更好的C++17一致性
- 改进的高DPI支持

#### Qt5支持
- 向后兼容Qt 5.15+
- 使用兼容性层处理API差异
- 适用于遗留系统
- 在需要时保持Qt5特定行为
- 在较旧发行版上的稳定性验证

#### 兼容性实现

项目使用多种机制确保Qt5/Qt6兼容性：

1. **自动检测**: CMake自动检测可用的Qt版本，优先选择Qt6
2. **兼容性层**: 位于[`src/common/qtcompat.h`](src/common/qtcompat.h:1)处理API差异
3. **条件编译**: 使用预处理器指令为Qt5/Qt6提供不同代码路径
4. **构建时选择**: 使用`-DQT_VERSION_MAJOR`或`-q`标志强制指定Qt版本
5. **CMake模块**: [`cmake/FindQtCompat.cmake`](cmake/FindQtCompat.cmake:1)提供统一的Qt检测
6. **统一函数**: 为翻译和构建工具提供兼容性函数

**关键兼容性特性：**
- 字符串分割行为（`QString::SkipEmptyParts` vs `Qt::SkipEmptyParts`）
- 文本流编码（Qt6默认为UTF-8，Qt5需要显式编解码器）
- 消息模式处理（`qSetMessagePattern` vs `QLoggingCategory`）
- 代理模型中的正则表达式vs通配符模式
- 翻译文件处理（Qt5/Qt6的不同API）
- MOC/UIC/RCC集成差异

**[`qtcompat.h`](src/common/qtcompat.h:1)中的兼容性宏：**
- `QT_SKIP_EMPTY_PARTS`: 统一的字符串分割行为
- `QT_SET_TEXT_STREAM_CODEC`: 处理编解码器差异
- `QT_SET_MESSAGE_PATTERN`: 消息模式兼容性
- `QT_SET_FILTER_REGEXP`: 正则表达式兼容性

**可执行文件命名：**
- Qt6构建：`media-debuger-qt6`
- Qt5构建：`media-debuger-qt5`

**版本特定功能：**
- **仅Qt6**: 现代信号/槽语法，改进的属性系统
- **Qt5兼容**: 为兼容性维护遗留代码模式
- **两个版本**: 核心功能在版本间保持一致

**兼容性测试：**
```bash
# 测试两个Qt版本
./build.sh -q 6 -c  # 构建Qt6版本
./build.sh -q 5 -c  # 构建Qt5版本

# 比较功能
./build/bin/media-debuger-qt6  # Qt6版本
./build/bin/media-debuger-qt5  # Qt5版本
```

### 高级构建配置

#### FFmpeg集成

项目通过自定义兼容性模块[`cmake/FindFFmpegCompat.cmake`](cmake/FindFFmpegCompat.cmake:1)使用FFmpeg：

**必需的FFmpeg组件：**
- libavcodec（编解码器支持）
- libavformat（格式处理）
- libavutil（工具函数）
- libswscale（缩放/转换）
- libswresample（音频重采样）
- libavfilter（过滤）

**FFmpeg检测：**
- 使用pkg-config自动检测
- 创建统一的`FFMpeg::FFmpeg`目标
- 处理包含目录和编译标志

#### 项目结构

```
media-debuger/
├── CMakeLists.txt              # 主CMake配置
├── build.sh                    # 构建脚本
├── clean_build_cache.sh        # 清理脚本
├── cmake/                      # CMake模块
│   ├── FindQtCompat.cmake      # Qt5/Qt6兼容性
│   └── FindFFmpegCompat.cmake  # FFmpeg检测
├── src/                        # 源代码
│   ├── common/                 # 核心组件
│   │   ├── qtcompat.h          # Qt兼容性宏
│   │   └── ...
│   ├── widgets/                # UI组件
│   ├── model/                  # 数据模型
│   └── main.cpp               # 应用程序入口
├── third_part/QJsonModel/       # 第三方JSON库
├── assets/                     # 资源和图标
├── translations/               # 翻译文件
└── debian/                     # Debian打包
```

### 故障排除

#### 常见问题

**1. 找不到Qt**
```
CMake Error: Could not find a package configuration file provided by "Qt6"
```

**解决方案：**
- 安装Qt开发包：
  ```bash
  # Ubuntu/Debian
  sudo apt install qt6-base-dev qt6-tools-dev  # Qt6
  sudo apt install qt5-default qttools5-dev  # Qt5
  
  # CentOS/RHEL/Fedora
  sudo yum install qt6-qtbase-devel qt6-qttools-devel  # Qt6
  sudo yum install qt5-qtbase-devel qt5-qttools-devel  # Qt5
  
  # Arch Linux
  sudo pacman -S qt6-base qt6-tools  # Qt6
  sudo pacman -S qt5-base qt5-tools  # Qt5
  ```
- 设置Qt环境变量：
  ```bash
  export Qt6_DIR=/path/to/qt6/lib/cmake/Qt6
  export Qt5_DIR=/path/to/qt5/lib/cmake/Qt5
  
  # 对于自定义Qt安装
  export PATH=/path/to/qt/bin:$PATH
  export LD_LIBRARY_PATH=/path/to/qt/lib:$LD_LIBRARY_PATH
  ```

**2. 找不到FFmpeg**
```
Package libavcodec was not found in the pkg-config search path.
```

**解决方案：**
- 安装FFmpeg开发包：
  ```bash
  # Ubuntu/Debian
  sudo apt install libavcodec-dev libavformat-dev libavutil-dev
  sudo apt install libswscale-dev libswresample-dev libavfilter-dev
  
  # CentOS/RHEL/Fedora
  sudo yum install ffmpeg-devel
  
  # Arch Linux
  sudo pacman -S ffmpeg
  ```
- 设置PKG_CONFIG_PATH：
  ```bash
  export PKG_CONFIG_PATH=/path/to/ffmpeg/lib/pkgconfig:$PKG_CONFIG_PATH
  
  # 对于自定义FFmpeg安装
  export LD_LIBRARY_PATH=/path/to/ffmpeg/lib:$LD_LIBRARY_PATH
  ```

**3. 编译错误**
```
error: 'QSomeClass' was not declared in this scope
```

**解决方案：**
- 验证Qt版本兼容性
- 确保包含了正确的头文件
- 检查Qt5/Qt6 API差异，使用[`src/common/qtcompat.h`](src/common/qtcompat.h:1)
- 使用构建脚本以便更容易调试
- 检查CMake输出中的缺失依赖项

**4. 构建脚本问题**

**权限被拒绝：**
```bash
chmod +x build.sh
chmod +x clean_build_cache.sh
```

**构建失败：**
```bash
# 清理并重新构建
./build.sh -c
./build.sh -d  # 尝试调试构建获取更多信息

# 检查构建日志
./build.sh -d 2>&1 | tee build.log
```

**5. 翻译问题**

由于Qt5/Qt6兼容性问题，翻译处理暂时禁用。这影响：
- `.ts`文件编译
- `.qm`文件生成
- 翻译安装

**临时解决方案：**应用程序仍可正常工作，但没有翻译，直到此问题解决。

**6. 运行时问题**

**应用程序无法启动：**
```bash
# 检查可执行文件是否存在
ls -la build/bin/

# 检查依赖项
ldd build/bin/media-debuger-qt6

# 使用调试输出运行
./build/bin/media-debuger-qt6 --help
```

**缺少库：**
```bash
# 更新库缓存
sudo ldconfig

# 检查Qt库路径
export QT_PLUGIN_PATH=/path/to/qt/plugins
export QML2_IMPORT_PATH=/path/to/qt/qml
```

**7. X11嵌入问题（Linux）**

如果嵌入式播放器无法工作：
```bash
# 检查X11开发库
sudo apt install libx11-dev libxext-dev libxfixes-dev

# 验证X11显示
echo $DISPLAY
xrandr --listmonitors
```

#### 高级故障排除

**详细构建输出：**
```bash
# 使用构建脚本
./build.sh -d -c

# 手动CMake详细输出
cmake -DCMAKE_VERBOSE_MAKEFILE=ON ..
make VERBOSE=1
```

**依赖项验证：**
```bash
# 检查Qt安装
qmake --version
qtdiag

# 检查FFmpeg安装
pkg-config --list-all | grep ffmpeg
ffmpeg -version
ffprobe -version
```

**清理环境：**
```bash
# 移除所有构建产物
./clean_build_cache.sh
rm -rf build/

# 重新开始
./build.sh -c
```

#### 调试构建

启用详细输出以诊断问题：

```bash
# 使用构建脚本
./build.sh -d  # 调试构建
./build.sh -c  # 先清理

# 手动CMake
cmake -DCMAKE_VERBOSE_MAKEFILE=ON ..
make VERBOSE=1
```

#### 性能优化

对于生产构建：

```bash
# 优化的发布版本构建
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON \
      ..

# 或使用构建脚本
./build.sh
```

#### 获取帮助

如果遇到问题：
1. 检查构建日志中的具体错误消息
2. 确保所有依赖项正确安装
3. 尝试调试构建获取更详细的输出
4. 验证Qt和FFmpeg版本兼容
5. 查看[`CMAKE_BUILD.md`](CMAKE_BUILD.md:1)获取详细构建说明
6. 检查`cmake/`目录中的兼容性模块

## 使用说明

1. **文件操作**:
   - 通过菜单或拖放打开媒体文件
   - 在历史面板查看最近打开的文件
   - 将媒体信息导出为多种格式

2. **媒体分析**:
   - 在主面板查看详细的格式信息
   - 检查单个流和编解码器
   - 分析帧级元数据

3. **高级功能**:
   - 使用内置播放器检查视频帧
   - 在所有媒体属性中搜索
   - 查看和过滤应用日志
   - 自定义UI布局和偏好设置

## 开发

### 项目结构

- `common/`: 核心组件和工具
  - `zffprobe`: FFprobe集成
  - `zlogger`: 日志系统
  - UI状态管理的同步器

- `widgets/`: UI组件
  - 播放器、搜索、日志等面板
  - 特定格式的展示组件

- `model/`: 数据模型
  - 媒体信息、日志和搜索模型

### 贡献指南

欢迎贡献代码和文档。请遵循以下步骤：

1. Fork 仓库。
2. 创建新分支。
3. 提交更改。
4. 发起 Pull Request。

## 许可证

本项目采用 MIT 许可证。详情请参阅 LICENSE 文件。
