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

### Dependencies

- Qt 6.x
- FFmpeg (including FFprobe)
- CMake 3.21 or higher

### Build Steps

1. Clone the repository:

   ```bash
   git clone https://gitee.com/sunstom/media-debuger.git
   ```

2. Enter the project directory:

   ```bash
   cd media-debuger
   ```

3. Create a build directory and enter it:

   ```bash
   mkdir build && cd build
   ```

4. Configure the build system:

   ```bash
   cmake ..
   ```

5. Compile the project:

   ```bash
   cmake --build .
   ```

6. Run the application:

   ```bash
   ./MediaDebuger
   ```

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

### Project Structure

- `common/`: Core components and utilities
  - `zffprobe`: FFprobe integration
  - `zlogger`: Logging system
  - Synchronizers for UI state management

- `widgets/`: UI components
  - Player, Search, Log and other panels
  - Format-specific display widgets

- `model/`: Data models
  - Media info, logs and search models

### Contribution Guidelines

Code and documentation contributions are welcome. Please follow these steps:

1. Fork the repository.
2. Create a new branch.
3. Commit your changes.
4. Submit a Pull Request.

## License

This project is licensed under the MIT License. Please refer to the LICENSE file for details.