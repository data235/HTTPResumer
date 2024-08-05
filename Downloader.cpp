#include "Downloader.h"
#include "utils.h"
#include "config.h"
#include <nlohmann/json.hpp>
#include <csignal>

using utils::extractFilenameFromUrl;
using utils::fileExists;
using utils::findCaseInsensitive;
using utils::formatBytes;
using utils::getFilenameFromHttpResponse;
using utils::getFilenameFromUrl;
using utils::generateUniqueFilename;

Downloader::Downloader(const std::string &url, const std::string &outputPath, const std::string &stateFileName)
    : curlHandle_(nullptr), url_(url), outputPath_(outputPath), stateFileName_(stateFileName),
      contentLength_(0), resumeOffset_(0), totalSize_(0), file_(NULL)
{
    initializeCurl();
}

void Downloader::initializeCurl()
{
    curlHandle_ = curl_easy_init();
    if (!curlHandle_)
    {
        throw std::runtime_error("Failed to initialize curl");
    }
}

Downloader::~Downloader()
{
    curl_easy_cleanup(curlHandle_);
}

void Downloader::setUrl(const std::string &url)
{
    url_ = url;
}

void Downloader::setFilename(const std::string &filename)
{
    outputPath_ = filename;
}

void Downloader::startDownload()
{
    if (fileExists(stateFileName_))
    {
        try
        {
            loadDownloadState();
            resumeDownload();
            return;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Failed to resume download: " << e.what() << std::endl;
        }
    }

    if (outputPath_.empty())
    {
        outputPath_ = getFilenameFromUrl(url_);
        outputPath_ = generateUniqueFilename(outputPath_);
    }
    resumeDownload();
    // TODO: Update meta data before saving the state
    saveDownloadState();
}

void Downloader::pauseDownload() const
{
    saveDownloadState();
}

void Downloader::resumeDownload()
{
    if (!curlHandle_)
    {
        throw std::runtime_error("libcurl handle is not initialized");
    }

    setupCurlOptions(resumeOffset_);

    file_ = fopen(outputPath_.c_str(), "ab");
    if (!file_)
    {
        throw std::runtime_error("Failed to open output file");
    }

    CURLcode res = curl_easy_perform(curlHandle_);
    fclose(file_);

    if (res != CURLE_OK)
    {
        std::cerr << "Failed to perform HTTP request: " << curl_easy_strerror(res) << std::endl;
    }
    else
    {
        long response_code;
        // Get respond code
        curl_easy_getinfo(curlHandle_, CURLINFO_RESPONSE_CODE, &response_code);
        std::cout << "Server respond code: " << response_code << std::endl;
        if (response_code == 206)
        {
            std::cout << "Server supports range requests and responded with partial content." << std::endl;
        }
        else
        {
            std::cout << "Server did not support range requests, or responded with full content." << std::endl;
        }
    }

    saveDownloadState();
}

void Downloader::setupCurlOptions(int64_t resumeOffset)
{
    curl_easy_setopt(curlHandle_, CURLOPT_URL, url_.c_str());
    curl_easy_setopt(curlHandle_, CURLOPT_FOLLOWLOCATION, 1L); // Redirect following
    curl_easy_setopt(curlHandle_, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curlHandle_, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curlHandle_, CURLOPT_RESUME_FROM_LARGE, resumeOffset);
    curl_easy_setopt(curlHandle_, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curlHandle_, CURLOPT_XFERINFODATA, this); // Set progress data
    // curl_easy_setopt(curlHandle_, CURLOPT_PROGRESSFUNCTION, progressCallback); // Deprecated
    curl_easy_setopt(curlHandle_, CURLOPT_XFERINFOFUNCTION, progressCallback);
    curl_easy_setopt(curlHandle_, CURLOPT_PROGRESSDATA, this);
    curl_easy_setopt(curlHandle_, CURLOPT_HEADERDATA, this);
    curl_easy_setopt(curlHandle_, CURLOPT_HEADERFUNCTION, headerCallback);
    curl_easy_setopt(curlHandle_, CURLOPT_DEBUGFUNCTION, debugFunction);
    curl_easy_setopt(curlHandle_, CURLOPT_VERBOSE, 1L); // Enable verbose output for debugging
}

size_t Downloader::writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    Downloader *downloader = static_cast<Downloader *>(userp);
    size_t realsize = size * nmemb;
    FILE *file = static_cast<FILE *>(userp);
    if (downloader->file_)
    {
        size_t written = fwrite(contents, size, nmemb, downloader->file_);
        downloader->resumeOffset_ += written; // Update resumeOffset_ dynamically
        return written;
    }
    return 0;
}

int Downloader::progressCallback(void *clientp, curl_off_t dltotal, curl_off_t dlnow,
                                 curl_off_t ultotal, curl_off_t ulnow)
{
    Downloader *downloader = static_cast<Downloader *>(clientp);
    downloader->contentLength_ = dltotal; // Update contentLength_ based on download progress

    // Debug: Print raw values to check for correctness
    // std::cout << "DEBUG: dltotal = " << dltotal << ", dlnow = " << dlnow << std::endl;

    // Calculate the download percentage
    double percent = dltotal ? (dlnow * 100.0 / dltotal) : 0.0;

    // Calculate the download speed
    static auto start_time = std::chrono::steady_clock::now();
    auto current_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = current_time - start_time;
    double speed = dlnow / elapsed_seconds.count();

    // Create progress bar
    int pos = static_cast<int>(BAR_WIDTH * (dlnow / static_cast<double>(dltotal)));
    std::ostringstream bar;
    bar << BAR_STR;
    for (int i = 0; i < BAR_WIDTH; ++i)
    {
        if (i < pos)
        {
            bar << BAR_STR;
        }
        else
        {
            bar << BAR_EMPTY_STR;
        }
    }
    bar << "|";

    // Print the download progress
    std::cout << "\r" << std::fixed << std::setprecision(2) << percent << "% "
              << bar.str() << " ("
              << formatBytes(dlnow) << " / " << formatBytes(dltotal)
              << ") at " << formatBytes(speed) << "/s        ";
    std::cout.flush();
    return 0;
}

size_t Downloader::headerCallback(char *buffer, size_t size, size_t nitems, void *userdata)
{
    Downloader *downloader = static_cast<Downloader *>(userdata);
    std::string header(buffer, size * nitems);

    // Print the header to check if it's being called
    std::cout << "Received header: " << header << std::endl;

    const std::string contentLengthPrefix = "content-length: ";
    size_t pos = findCaseInsensitive(header, contentLengthPrefix);
    if (pos != std::string::npos)
    {
        downloader->contentLength_ = std::stoll(header.substr(pos + contentLengthPrefix.size()));
    }

    const std::string contentRangePrefix = "content-range: ";

    // Check for Content-Range header
    pos = findCaseInsensitive(header, contentRangePrefix);
    if (pos != std::string::npos)
    {
        // Find the position of the slash ("/") which separates the byte range from the total size
        size_t slashPos = header.find('/', pos + contentRangePrefix.size());
        if (slashPos != std::string::npos)
        {
            // Extract the total size part after the slash
            std::string totalSizeStr = header.substr(slashPos + 1);
            try
            {
                downloader->totalSize_ = std::stoll(totalSizeStr);
            }
            catch (const std::invalid_argument &e)
            {
                std::cerr << "Error parsing total size: " << e.what() << std::endl;
            }
            catch (const std::out_of_range &e)
            {
                std::cerr << "Total size out of range: " << e.what() << std::endl;
            }
        }
    }

    return size * nitems;
}

int Downloader::debugFunction(CURL *handle, curl_infotype type, char *data, size_t size, void *userptr)
{
    if (type == CURLINFO_HEADER_IN)
    {
        if (type == CURLINFO_HEADER_IN)
        {
            std::cout << "Header In: " << std::string(data, size);
        }
    }

    return 0;
}

void Downloader::saveDownloadState() const
{
    nlohmann::json state;
    state["url"] = url_;
    state["outputPath"] = outputPath_;
    state["resumeOffset"] = resumeOffset_;
    state["contentLength"] = contentLength_;
    state["totalSize"] = totalSize_;

    std::ofstream file(stateFileName_);
    if (file.is_open())
    {
        file << state.dump(4);
        file.close();
    }
    else
    {
        throw std::runtime_error("Failed to open state file for saving");
    }
}

void Downloader::loadDownloadState()
{
    std::ifstream file(stateFileName_);
    if (file.is_open())
    {
        nlohmann::json state;
        file >> state;

        url_ = state["url"];
        outputPath_ = state["outputPath"];
        resumeOffset_ = state["resumeOffset"];
        contentLength_ = state["contentLength"];
        totalSize_ = state["totalSize"];

        file.close();
    }
    else
    {
        throw std::runtime_error("Failed to open state file for loading");
    }
}