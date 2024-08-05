#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <curl/curl.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <functional>
#include <string>

class Downloader
{
public:
    Downloader(const std::string &url, const std::string &outputPath, const std::string &stateFileName);
    ~Downloader();

    void setUrl(const std::string &url);
    void setFilename(const std::string &filename);
    void startDownload();
    void pauseDownload() const;
    void resumeDownload();
    void saveDownloadState() const;
    void loadDownloadState();

    void setProgressCallback(std::function<void(int64_t, int64_t)> callback) { progressCallback_ = callback; }

private:
    CURL *curlHandle_;
    std::string url_;
    std::string outputPath_;
    std::string stateFileName_;
    std::function<void(int64_t, int64_t)> progressCallback_;
    int64_t contentLength_; // Total length of the content being downloaded
    int64_t resumeOffset_;  // Offset to resume download from
    uint64_t totalSize_;
    FILE *file_;

    static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp);
    static int progressCallback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
    static size_t headerCallback(char *buffer, size_t size, size_t nitems, void *userdata);
    static int debugFunction(CURL *handle, curl_infotype type, char *data, size_t size, void *userptr);
    void setupCurlOptions(int64_t resumeOffset = 0);
    void initializeCurl();
};

#endif // DOWNLOADER_H
