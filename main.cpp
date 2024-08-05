#include "Downloader.h"
#include "utils.h"
#include "version.h"
#include <iostream>
#include <csignal>
#include <cstring>

Downloader *downloader = nullptr;

void signalHandler(int signum)
{
    if (downloader)
    {
        downloader->pauseDownload();
    }
    std::cout << "\nInterrupt signal (" << signum << ") received. Download state saved." << std::endl;
    exit(signum);
}

void printUsage()
{
    std::cout << "Usage: HTTPResumer [URL] [Output Filename] [State Filename]\n";
    std::cout << "Example: HTTPResumer https://example.com/file.zip file.zip state.json\n";
}

int main(int argc, char *argv[])
{
    std::cout << PROJECT_NAME << " Version: " << PROJECT_VERSION_STRING << std::endl;

    if (argc < 2 || argc > 4)
    {
        printUsage();
        return 1;
    }

    std::string url = argv[1];
    std::string outputPath;
    std::string stateFileName;

    if (argc >= 3)
    {
        outputPath = argv[2];
    }

    if (argc == 4)
    {
        stateFileName = argv[3];
    }
    else
    {
        stateFileName = utils::getFilenameFromUrl(url) + "-state.json";
    }

    try
    {
        downloader = new Downloader(url, outputPath, stateFileName);
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);
        downloader->startDownload();
        delete downloader;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
