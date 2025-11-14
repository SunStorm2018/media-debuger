# MediaDebugger

MediaDebugger is a Qt-based multimedia file analysis and debugging tool, primarily used for viewing and analyzing various information of multimedia files, including format, encoding, stream information, etc. This project uses FFprobe as the underlying tool to obtain detailed information about media files and presents it to users through a graphical interface.

## Features

- **Comprehensive Media Analysis**:
  - Detailed format, encoder and stream information via FFprobe integration
  - Frame-level metadata inspection
  - Codec parameters and capabilities analysis

- **Advanced UI Components**:
  - Media Player (PlayerWG): Embedded playback with seek and frame analysis
  - Search Panel (SearchWG): Multi-column search across media attributes
  - Log Viewer (LogWG): Real-time logging with text highlighting
  - JSON Formatter: Pretty-print and analyze media metadata
  - Table View: Structured display of media properties

- **Core Functionality**:
  - State synchronization between UI components
  - Configurable logging system with multiple levels
  - History management for recently opened files
  - Export to JSON, CSV and other formats
  - Multi-language support (English/Chinese)

- **Technical Highlights**:
  - Custom text highlighters for log analysis
  - UI state synchronization system
  - Asynchronous command execution
  - Model-View architecture for data presentation

## Installation Instructions

### System Requirements

- **CMake**: 3.16 or higher
- **Qt**: Qt 5.15+ or Qt 6.x
- **FFmpeg**: Including development libraries and pkg-config files
- **Compiler**: C++17 compatible GCC, Clang, or MSVC
- **X11**: For Linux desktop integration (libx11-dev)

### Linux System Dependencies

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

### Build Methods

#### Method 1: Using Build Script (Recommended)

The project provides a comprehensive build script `build.sh` with extensive options:

```bash
# Basic build (Release)
./build.sh

# Debug build
./build.sh -d

# Build and install
./build.sh -i

# Specify Qt version
./build.sh -q 6

# Custom install prefix
./build.sh -p /usr/local

# Clean build directory
./build.sh -c

# View all available options
./build.sh -h
```

**Build Script Options:**
- `-d, --debug`: Build debug version instead of release
- `-c, --clean`: Clean build directory before building
- `-i, --install`: Install after building (requires sudo for system directories)
- `-p, --prefix DIR`: Set installation prefix (default: /usr)
- `-q, --qt-version N`: Force specific Qt version (5 or 6)
- `-h, --help`: Show help message

**Additional Scripts:**
- `clean_build_cache.sh`: Clean all build artifacts and cache files

**Build Script Usage Examples:**

```bash
# Development workflow - clean, debug build, and test
./build.sh -c -d

# Production build with custom prefix
./build.sh -p /opt/media-debuger -i

# Force Qt5 build for legacy systems
./build.sh -q 5 -c

# Quick rebuild without cleaning
./build.sh

# Build for testing (debug + custom prefix)
./build.sh -d -p ~/local/media-debuger
```

**Build Script Features:**
- **Automatic Qt Detection**: Automatically detects Qt6 first, falls back to Qt5
- **Parallel Compilation**: Uses all available CPU cores (`-j$(nproc)`)
- **Smart Installation**: Automatically uses `sudo` for system directories
- **Executable Detection**: Automatically detects which Qt version was built
- **Error Handling**: Provides clear error messages and exit codes

#### Method 2: Manual CMake Build

1. Clone the repository:

   ```bash
   git clone https://gitee.com/sunstom/media-debuger.git
   cd media-debuger
   ```

2. Create build directory:

   ```bash
   mkdir build && cd build
   ```

3. Configure the project:

   ```bash
   # Auto-detect Qt version (prefers Qt6)
   cmake ..

   # Specify Qt version
   cmake -DQT_VERSION_MAJOR=6 ..  # Use Qt6
   cmake -DQT_VERSION_MAJOR=5 ..  # Use Qt5

   # Set build type
   cmake -DCMAKE_BUILD_TYPE=Release ..

   # Set install prefix
   cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..

   # Enable verbose output for debugging
   cmake -DCMAKE_VERBOSE_MAKEFILE=ON ..
   ```

4. Compile:

   ```bash
   # Use all CPU cores
   make -j$(nproc)

   # Or on Windows
   cmake --build . --config Release

   # With verbose output
   make VERBOSE=1
   ```

5. Install (optional):

   ```bash
   # Linux/macOS
   sudo make install

   # Or using CMake
   sudo cmake --install .

   # Windows (requires admin privileges)
   cmake --install . --config Release
   ```

6. Run the application:

   ```bash
   # From build directory
   ./bin/media-debuger-qt6  # Qt6 version
   ./bin/media-debuger-qt5  # Qt5 version

   # Or after installation
   media-debuger-qt6
   ```

### Build Options

| Option | Default | Description |
|-------|---------|-------------|
| `CMAKE_BUILD_TYPE` | `Release` | Build type: Release/Debug/RelWithDebInfo/MinSizeRel |
| `CMAKE_INSTALL_PREFIX` | `/usr` | Installation prefix |
| `QT_VERSION_MAJOR` | `Auto-detect` | Force Qt version: 5 or 6 |
| `CMAKE_VERBOSE_MAKEFILE` | `OFF` | Enable verbose makefile output |

### Build Types

- **Release**: Optimized version without debug information (default)
- **Debug**: Debug version with debug information and no optimization
- **RelWithDebInfo**: Optimized version with debug information
- **MinSizeRel**: Minimum size optimized version

### Qt5/Qt6 Compatibility

The project provides comprehensive Qt5/Qt6 compatibility through multiple mechanisms:

#### Qt6 Support
- Full Qt6 API compatibility with modern features
- Uses Qt6's improved performance and functionality
- Recommended for new deployments
- Automatic UTF-8 encoding support
- Enhanced regular expression engine
- Better C++17 conformance
- Improved high-DPI support

#### Qt5 Support
- Backward compatibility with Qt 5.15+
- Uses compatibility layer to handle API differences
- Suitable for legacy systems
- Maintains Qt5-specific behaviors where needed
- Proven stability on older distributions

#### Compatibility Implementation

The project uses several mechanisms to ensure Qt5/Qt6 compatibility:

1. **Automatic Detection**: CMake automatically detects available Qt versions with preference for Qt6
2. **Compatibility Layer**: Located in [`src/common/qtcompat.h`](src/common/qtcompat.h:1) handles API differences
3. **Conditional Compilation**: Different code paths for Qt5/Qt6 using preprocessor directives
4. **Build-time Selection**: Force specific Qt version with `-DQT_VERSION_MAJOR` or `-q` flag
5. **CMake Modules**: [`cmake/FindQtCompat.cmake`](cmake/FindQtCompat.cmake:1) provides unified Qt detection
6. **Unified Functions**: Compatibility functions for translation and build tools

**Key Compatibility Features:**
- String splitting behavior (`QString::SkipEmptyParts` vs `Qt::SkipEmptyParts`)
- Text stream encoding (Qt6 defaults to UTF-8, Qt5 needs explicit codec)
- Message pattern handling (`qSetMessagePattern` vs `QLoggingCategory`)
- Regular expression vs wildcard patterns in proxy models
- Translation file processing (different APIs for Qt5/Qt6)
- MOC/UIC/RCC integration differences

**Compatibility Macros in [`qtcompat.h`](src/common/qtcompat.h:1):**
- `QT_SKIP_EMPTY_PARTS`: Unified string splitting behavior
- `QT_SET_TEXT_STREAM_CODEC`: Handles codec differences
- `QT_SET_MESSAGE_PATTERN`: Message pattern compatibility
- `QT_SET_FILTER_REGEXP`: Regular expression compatibility

**Executable Naming:**
- Qt6 builds: `media-debuger-qt6`
- Qt5 builds: `media-debuger-qt5`

**Version-Specific Features:**
- **Qt6 Only**: Modern signal/slot syntax, improved property system
- **Qt5 Compatible**: Legacy code patterns maintained for compatibility
- **Both Versions**: Core functionality identical across versions

**Testing Compatibility:**
```bash
# Test both Qt versions
./build.sh -q 6 -c  # Build Qt6 version
./build.sh -q 5 -c  # Build Qt5 version

# Compare functionality
./build/bin/media-debuger-qt6  # Qt6 version
./build/bin/media-debuger-qt5  # Qt5 version
```

### Advanced Build Configuration

#### FFmpeg Integration

The project uses FFmpeg through a custom compatibility module [`cmake/FindFFmpegCompat.cmake`](cmake/FindFFmpegCompat.cmake:1):

**Required FFmpeg Components:**
- libavcodec (codec support)
- libavformat (format handling)
- libavutil (utility functions)
- libswscale (scaling/conversion)
- libswresample (audio resampling)
- libavfilter (filtering)

**FFmpeg Detection:**
- Uses pkg-config for automatic detection
- Creates unified `FFMpeg::FFmpeg` target
- Handles include directories and compile flags

#### Project Structure

```
media-debuger/
├── CMakeLists.txt              # Main CMake configuration
├── build.sh                    # Build script
├── clean_build_cache.sh        # Clean script
├── cmake/                      # CMake modules
│   ├── FindQtCompat.cmake      # Qt5/Qt6 compatibility
│   └── FindFFmpegCompat.cmake  # FFmpeg detection
├── src/                        # Source code
│   ├── common/                 # Core components
│   │   ├── qtcompat.h          # Qt compatibility macros
│   │   └── ...
│   ├── widgets/                # UI components
│   ├── model/                  # Data models
│   └── main.cpp               # Application entry
├── third_part/QJsonModel/       # Third-party JSON library
├── assets/                     # Resources and icons
├── translations/               # Translation files
└── debian/                     # Debian packaging
```

### Troubleshooting

#### Common Issues

**1. Qt Not Found**
```
CMake Error: Could not find a package configuration file provided by "Qt6"
```

**Solutions:**
- Install Qt development packages:
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
- Set Qt environment variables:
  ```bash
  export Qt6_DIR=/path/to/qt6/lib/cmake/Qt6
  export Qt5_DIR=/path/to/qt5/lib/cmake/Qt5
  
  # For custom Qt installations
  export PATH=/path/to/qt/bin:$PATH
  export LD_LIBRARY_PATH=/path/to/qt/lib:$LD_LIBRARY_PATH
  ```

**2. FFmpeg Not Found**
```
Package libavcodec was not found in the pkg-config search path.
```

**Solutions:**
- Install FFmpeg development packages:
  ```bash
  # Ubuntu/Debian
  sudo apt install libavcodec-dev libavformat-dev libavutil-dev
  sudo apt install libswscale-dev libswresample-dev libavfilter-dev
  
  # CentOS/RHEL/Fedora
  sudo yum install ffmpeg-devel
  
  # Arch Linux
  sudo pacman -S ffmpeg
  ```
- Set PKG_CONFIG_PATH:
  ```bash
  export PKG_CONFIG_PATH=/path/to/ffmpeg/lib/pkgconfig:$PKG_CONFIG_PATH
  
  # For custom FFmpeg installations
  export LD_LIBRARY_PATH=/path/to/ffmpeg/lib:$LD_LIBRARY_PATH
  ```

**3. Compilation Errors**
```
error: 'QSomeClass' was not declared in this scope
```

**Solutions:**
- Verify Qt version compatibility
- Ensure correct headers are included
- Check for Qt5/Qt6 API differences using [`src/common/qtcompat.h`](src/common/qtcompat.h:1)
- Use the build script for easier debugging
- Check CMake output for missing dependencies

**4. Build Script Issues**

**Permission Denied:**
```bash
chmod +x build.sh
chmod +x clean_build_cache.sh
```

**Build Failures:**
```bash
# Clean and rebuild
./build.sh -c
./build.sh -d  # Try debug build for more info

# Check build logs
./build.sh -d 2>&1 | tee build.log
```

**5. Translation Issues**

Translation processing is temporarily disabled due to Qt5/Qt6 compatibility issues. This affects:
- `.ts` file compilation
- `.qm` file generation
- Translation installation

**Workaround:** The application will still work but without translations until this is resolved.

**6. Runtime Issues**

**Application won't start:**
```bash
# Check if executable exists
ls -la build/bin/

# Check dependencies
ldd build/bin/media-debuger-qt6

# Run with debug output
./build/bin/media-debuger-qt6 --help
```

**Missing libraries:**
```bash
# Update library cache
sudo ldconfig

# Check Qt library paths
export QT_PLUGIN_PATH=/path/to/qt/plugins
export QML2_IMPORT_PATH=/path/to/qt/qml
```

**7. X11 Embedding Issues (Linux)**

If the embedded player doesn't work:
```bash
# Check X11 development libraries
sudo apt install libx11-dev libxext-dev libxfixes-dev

# Verify X11 display
echo $DISPLAY
xrandr --listmonitors
```

#### Advanced Troubleshooting

**Verbose Build Output:**
```bash
# Using build script
./build.sh -d -c

# Manual CMake with verbose output
cmake -DCMAKE_VERBOSE_MAKEFILE=ON ..
make VERBOSE=1
```

**Dependency Verification:**
```bash
# Check Qt installation
qmake --version
qtdiag

# Check FFmpeg installation
pkg-config --list-all | grep ffmpeg
ffmpeg -version
ffprobe -version
```

**Clean Environment:**
```bash
# Remove all build artifacts
./clean_build_cache.sh
rm -rf build/

# Start fresh
./build.sh -c
```

#### Debugging Builds

Enable verbose output to diagnose issues:

```bash
# Using build script
./build.sh -d  # Debug build
./build.sh -c  # Clean first

# Manual CMake
cmake -DCMAKE_VERBOSE_MAKEFILE=ON ..
make VERBOSE=1
```

#### Performance Optimization

For production builds:

```bash
# Optimized release build
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON \
      ..

# Or using build script
./build.sh
```

#### Getting Help

If you encounter issues:
1. Check the build logs for specific error messages
2. Ensure all dependencies are correctly installed
3. Try the debug build for more detailed output
4. Verify Qt and FFmpeg versions are compatible
5. Check [`CMAKE_BUILD.md`](CMAKE_BUILD.md:1) for detailed build instructions
6. Examine the compatibility modules in `cmake/` directory

## Usage Instructions

1. **File Operations**:
   - Open media files via menu or drag-and-drop
   - View recently opened files in history panel
   - Export media info to various formats

2. **Media Analysis**:
   - View detailed format info in main panel
   - Inspect individual streams and codecs
   - Analyze frame-level metadata

3. **Advanced Features**:
   - Use embedded player to inspect video frames
   - Search across all media attributes
   - View and filter application logs
   - Customize UI layout and preferences

## Development

### Project Architecture

MediaDebugger follows a modular architecture with clear separation of concerns:

#### Core Components (`src/common/`)
- **[`zffprobe`](src/common/zffprobe.h:1)**: FFprobe integration layer for media analysis
- **[`zlogger`](src/common/zlogger.h:1)**: Multi-level logging system with file output
- **[`qtcompat.h`](src/common/qtcompat.h:1)**: Qt5/Qt6 compatibility macros and utilities
- **State Synchronization**: UI state management system
  - [`checkboxsynchronizer`](src/common/checkboxsynchronizer.h:1): Checkbox state sync
  - [`lineeditsynchronizer`](src/common/lineeditsynchronizer.h:1): Text input sync
  - [`spinboxsynchronizer`](src/common/spinboxsynchronizer.h:1): Numeric input sync
- **Command Execution**: [`commandexecutor`](src/common/commandexecutor.h:1) for async operations
- **Text Processing**: [`ztexteditor`](src/common/ztexteditor.h:1) and [`ztexthighlighter`](src/common/ztexthighlighter.h:1)

#### UI Components (`src/widgets/`)
- **Media Player**: [`playerwg`](src/widgets/playerwg.h:1) - Embedded video player with frame analysis
- **File Management**: [`fileswg`](src/widgets/fileswg.h:1), [`folderswg`](src/widgets/folderswg.h:1)
- **Data Display**:
  - [`jsonfmtwg`](src/widgets/jsonfmtwg.h:1) - JSON formatter
  - [`infotablewg`](src/widgets/infotablewg.h:1) - Structured data tables
  - [`tabelfmtwg`](src/widgets/tabelfmtwg.h:1) - Table formatting
- **Search & Analysis**: [`searchwg`](src/widgets/searchwg.h:1) - Multi-column search
- **Logging**: [`logwg`](src/widgets/logwg.h:1) - Real-time log viewer
- **Configuration**: [`configwindow`](src/widgets/configwindow.h:1), [`globalconfingwg`](src/widgets/globalconfingwg.h:1)

#### Data Models (`src/model/`)
- **Media Info**: [`mediainfotabelmodel`](src/model/mediainfotabelmodel.h:1) - Media metadata model
- **File History**: [`fileshistorymodel`](src/model/fileshistorymodel.h:1) - Recent files management
- **Search**: [`multicolumnsearchproxymodel`](src/model/multicolumnsearchproxymodel.h:1) - Advanced search filtering
- **Logging**: [`logmodel`](src/model/logmodel.h:1) - Log data management

### Build System Architecture

#### CMake Modules
- **[`FindQtCompat.cmake`](cmake/FindQtCompat.cmake:1)**: Unified Qt5/Qt6 detection and configuration
- **[`FindFFmpegCompat.cmake`](cmake/FindFFmpegCompat.cmake:1)**: FFmpeg library detection with pkg-config

#### Compatibility Layer
The project uses a sophisticated compatibility system:

1. **Build-time Detection**: Automatic Qt version detection with fallback
2. **Compile-time Adaptation**: Conditional compilation based on Qt version
3. **API Abstraction**: Unified interfaces for Qt5/Qt6 differences
4. **Feature Detection**: Runtime capability checks when needed

### Third-Party Dependencies

#### QJsonModel
- **Location**: [`third_part/QJsonModel/`](third_part/QJsonModel/)
- **Purpose**: JSON data model for Qt applications
- **Integration**: CMake subdirectory with proper namespacing
- **License**: MIT (compatible with project license)

### Development Guidelines

#### Coding Standards
- **C++17**: Modern C++ features with Qt integration
- **Qt Style**: Follow Qt coding conventions
- **SPDX Headers**: All source files include license headers
- **Documentation**: Comprehensive API documentation

#### Qt5/Qt6 Compatibility Development
When adding new features:

1. **Use Compatibility Macros**: Prefer macros from [`qtcompat.h`](src/common/qtcompat.h:1)
2. **Conditional Compilation**: Use `#if QT_VERSION_MAJOR` for version-specific code
3. **Test Both Versions**: Ensure compatibility with both Qt5 and Qt6
4. **Update CMake Modules**: Modify [`FindQtCompat.cmake`](cmake/FindQtCompat.cmake:1) if new Qt components are needed

#### Adding New Dependencies
1. **Update CMakeLists.txt**: Add find_package and link_libraries calls
2. **Create Compatibility Module**: For complex dependencies, create a FindXXXCompat.cmake
3. **Update Build Script**: Modify [`build.sh`](build.sh:1) if additional steps are needed
4. **Document Dependencies**: Update README and CMAKE_BUILD.md

### Testing

#### Build Testing
```bash
# Test both Qt versions
./build.sh -q 5 -c && ./build.sh -q 6 -c

# Test different build types
./build.sh -d  # Debug
./build.sh     # Release
```

#### Runtime Testing
- Test with various media formats
- Verify UI synchronization
- Check translation loading (when enabled)
- Validate FFmpeg integration

### Contribution Guidelines

Code and documentation contributions are welcome. Please follow these steps:

1. **Fork the Repository**: Create a personal fork
2. **Create Feature Branch**: Use descriptive branch names
3. **Follow Coding Standards**: Maintain consistency with existing code
4. **Test Compatibility**: Ensure Qt5/Qt6 compatibility
5. **Update Documentation**: Include relevant documentation changes
6. **Submit Pull Request**: Provide clear description of changes

#### Pull Request Requirements
- **Testing**: Include test results for both Qt5 and Qt6
- **Documentation**: Update relevant documentation files
- **Compatibility**: Ensure no regression in Qt5/Qt6 support
- **License**: Maintain SPDX headers in all new files

### Development Tools

#### Build Scripts
- **[`build.sh`](build.sh:1)**: Main build script with comprehensive options
- **[`clean_build_cache.sh`](clean_build_cache.sh:1)**: Clean all build artifacts
- **Git Scripts**: [`git_fetch.sh`](git_fetch.sh:1), [`git_push.sh`](git_push.sh:1) for repository management

#### Documentation
- **[`CMAKE_BUILD.md`](CMAKE_BUILD.md:1)**: Detailed build instructions
- **[`docs/`](docs/)**: Technical documentation
  - [`ffmpeg.md`](docs/ffmpeg.md:1): FFmpeg integration details
  - [`ffprobe.md`](docs/ffprobe.md:1): FFprobe usage examples
  - [`show_format.md`](docs/show_format.md:1): Format display documentation
  - [`show_streams.md`](docs/show_streams.md:1): Stream analysis documentation
  - [`video_sources.md`](docs/video_sources.md:1): Video source handling
  - [`TODO.txt`](docs/TODO.txt:1): Development roadmap

## License

This project is licensed under the MIT License. Please refer to the LICENSE file for details.