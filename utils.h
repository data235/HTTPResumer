#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <curl/curl.h>

namespace utils
{
    void setDirectConnection(CURL *curl);
    std::string extractFilenameFromUrl(const std::string &url);
    bool fileExists(const std::string &fileName);
    std::string getFilenameFromHttpResponse(const std::string &url);
    std::string getFilenameFromUrl(const std::string &url);
    std::string formatBytes(curl_off_t bytes);
    std::string toLower(const std::string &str);
    size_t findCaseInsensitive(const std::string &haystack, const std::string &needle);
    std::string generateUniqueFilename(const std::string &baseName);
    void clearScreen();
}

#endif // UTILS_H
