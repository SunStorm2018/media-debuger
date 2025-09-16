# MediaDebugger

MediaDebugger is a Qt-based multimedia file analysis and debugging tool, primarily used for viewing and analyzing various information of multimedia files, including format, encoding, stream information, etc. This project uses FFprobe as the underlying tool to obtain detailed information about media files and presents it to users through a graphical interface.

## Features

- **Media Information Viewing**: Supports viewing format, encoder, stream information, etc., of media files.
- **Log Recording**: Provides detailed log recording functionality for debugging and issue tracking.
- **History Records**: Automatically saves records of recently opened files for quick access.
- **Search Functionality**: Supports keyword searching within media information.
- **Export Functionality**: Allows media information to be exported into multiple formats, such as JSON, tables, etc.
- **Multi-language Support**: Interface supports switching between multiple languages, including Chinese.

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

- **Open File**: Open a media file via the file menu or drag-and-drop operation.
- **View Information**: View detailed information of the media file in the main window.
- **Search Functionality**: Use the search box to enter keywords and quickly locate relevant information.
- **Export Information**: Click the export button, select the information and format to be exported, and save it as a file.
- **View Logs**: View runtime log information in the log window.

## Contribution Guidelines

Code and documentation contributions are welcome. Please follow these steps:

1. Fork the repository.
2. Create a new branch.
3. Commit your changes.
4. Submit a Pull Request.

## License

This project is licensed under the MIT License. Please refer to the LICENSE file for details.