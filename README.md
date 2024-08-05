# HTTPResumer

HTTPResumer is a C++ project that demonstrates the implementation of an HTTP downloader with pause and resume capabilities using the libcurl library. This project serves as an educational tool to explore HTTP protocol programming in C++, focusing on handling callback functions for greater control over the download process.

## Table of Contents

- [Features](#features)
- [Installation](#installation)
- [Building](#building)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgements](#acknowledgements)
- [Disclaimer](#disclaimer)

## Features

- **Pause and Resume Downloads:** Ability to pause and resume downloads, even after the application is restarted.
- **State Management:** Saves the state of downloads, including URLs, output paths, and downloaded bytes, allowing for resumption from the last saved state.
- **Progress Monitoring:** Displays download progress, including download speed and percentage completed.
- **Header Parsing:** Extracts filename and content length from HTTP response headers.

## Installation

### Prerequisites

To set up the development environment on Ubuntu, you need to install the necessary packages:

- **CMake**: A build system generator.
- **GCC/G++**: GNU C and C++ compilers.
- **libcurl**: A library for handling HTTP requests.
- **nlohmann/json**: A JSON library for state management.

You can install these dependencies on Ubuntu using the following commands:

```bash
sudo apt update
sudo apt install gcc g++ cmake libcurl4 libcurl4-openssl-dev nlohmann-json3-dev
```

## Building

### Building from Source

To build the project from source, follow these steps:

1. Clone the repository:

    ```bash
    git clone https://github.com/data235/HTTPResumer.git
    ```

2. Navigate to the project directory:

    ```bash
    cd HTTPResumer
    ```

3. Create a build directory and navigate into it:

    ```bash
    mkdir build
    cd build
    ```

4. Run CMake to generate the build files:

    ```bash
    cmake ..
    ```

5. Build the project:

    ```bash
    make
    ```

This will compile the project and generate the executable.

## Usage

To use HTTPResumer, run the executable with the URL, output filename, and state filename as arguments:

```bash
./httpresumer [URL] [Output Filename] [State Filename]

# Example
./httpresumer https://example.com/file.zip file.zip file-state.json
```

- **URL (Required):** The URL of the file to download.
- **Output Filename (Optional):** The name of the file to save the downloaded content. If not provided, the filename will be extracted from the URL or HTTP response.
- **State Filename (Optional):** The name of the JSON file to save the download state. Defaults to `[output_filename]-state.json` if not specified.

## Contributing

Contributions are welcome! Feel free to open issues or submit pull requests. For major changes, please open an issue first to discuss the proposed modifications.

## License
This project is licensed under the Apache License 2.0. See the [LICENSE](LICENSE) file for details or read the full license text at [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0).

## Acknowledgements

- [libcurl](https://curl.se/libcurl/) - The library used for handling HTTP requests.
- [nlohmann/json](https://github.com/nlohmann/json) - The JSON library used for state management.

## Disclaimer

This project is intended for educational purposes and is not designed to be a complete or production-ready downloader. Advanced downloaders like [aria2c](https://aria2.github.io/) already exist, and `curl` itself has options for resuming downloads. This project aims to provide a learning experience by offering full control over callback functions. A GUI interface for this downloader is planned for future development.