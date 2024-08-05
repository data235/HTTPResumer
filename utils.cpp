#include "utils.h"
#include <fstream>
#include <filesystem>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <string>
#include <curl/curl.h>

namespace utils
{
    bool fileExists(const std::string &fileName)
    {
        return std::filesystem::exists(fileName);
    }

    size_t headerCallback(char *buffer, size_t size, size_t nitems, void *userdata)
    {
        std::string header(buffer, size * nitems);

        const std::string prefix = "Content-Disposition: ";
        size_t pos = header.find(prefix);
        if (pos != std::string::npos)
        {
            pos += prefix.size();
            size_t filename_start = header.find("filename=", pos);
            if (filename_start != std::string::npos)
            {
                filename_start += strlen("filename=");
                size_t filename_end = header.find(';', filename_start);
                if (filename_end == std::string::npos)
                {
                    filename_end = header.size();
                }

                std::string filename = header.substr(filename_start, filename_end - filename_start);
                size_t quote_start = filename.find_first_of('"');
                size_t quote_end = filename.find_last_of('"');
                if (quote_start != std::string::npos && quote_end != std::string::npos && quote_end > quote_start)
                {
                    filename = filename.substr(quote_start + 1, quote_end - quote_start - 1);
                }

                *static_cast<std::string *>(userdata) = filename;
            }
        }

        return size * nitems;
    }

    std::string getFilenameFromHttpResponse(const std::string &url)
    {
        std::string filename;
        CURL *curl = curl_easy_init();

        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headerCallback);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, &filename);

            CURLcode res = curl_easy_perform(curl);

            if (res != CURLE_OK)
            {
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            }

            curl_easy_cleanup(curl);
        }
        else
        {
            std::cerr << "Failed to initialize libcurl" << std::endl;
        }

        return filename;
    }

    void setDirectConnection(CURL *curl)
    {
        curl_easy_setopt(curl, CURLOPT_PROXY, "");
    }

    std::string extractFilenameFromUrl(const std::string &url)
    {
        size_t lastSlashPos = url.find_last_of("/");
        if (lastSlashPos == std::string::npos)
        {
            throw std::runtime_error("Invalid URL: no slash found");
        }

        std::string filename = url.substr(lastSlashPos + 1);

        size_t paramPos = filename.find_first_of("?&#");
        if (paramPos != std::string::npos)
        {
            filename = filename.substr(0, paramPos);
        }

        if (filename.empty())
        {
            throw std::runtime_error("Invalid URL: filename is empty");
        }

        return filename;
    }

    std::string getFilenameFromUrl(const std::string &url)
    {
        std::string filename = getFilenameFromHttpResponse(url);
        if (!filename.empty())
        {
            return filename;
        }

        return extractFilenameFromUrl(url);
    }
    std::string formatBytes(curl_off_t bytes)
    {
        const char *sizes[] = {"B", "KB", "MB", "GB", "TB"};
        int order = 0;
        double readableSize = bytes;

        while (readableSize >= 1024 && order < sizeof(sizes) / sizeof(*sizes) - 1)
        {
            order++;
            readableSize = readableSize / 1024;
        }

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << readableSize << " " << sizes[order];
        return oss.str();
    }

    // Function to convert a string to lowercase
    std::string toLower(const std::string &str)
    {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c)
                       { return std::tolower(c); });
        return result;
    }

    // Function to find a case-insensitive substring
    size_t findCaseInsensitive(const std::string &haystack, const std::string &needle)
    {
        std::string haystackLower = toLower(haystack);
        std::string needleLower = toLower(needle);

        size_t pos = haystackLower.find(needleLower);
        return pos;
    }

    std::string generateUniqueFilename(const std::string &baseName)
    {
        std::string uniqueName = baseName;
        int counter = 1;

        while (fileExists(uniqueName))
        {
            uniqueName = baseName + "." + std::to_string(counter);
            counter++;
        }

        return uniqueName;
    }

    void clearScreen()
    {
        std::system("clear");
    }

}
