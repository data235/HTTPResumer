#include <gtest/gtest.h>
#include "utils.h"

TEST(UtilsTests, GetFilenameFromHttpResponse)
{
    std::string url = "http://www.example.com/file.zip";
    std::string filename = utils::getFilenameFromHttpResponse(url);
    std::cout << "filename: " << filename << std::endl;
    EXPECT_EQ(filename, "");
}

TEST(UtilsTests, GetFilenameFromUrl)
{
    std::string url = "http://www.example.com/file.zip";
    std::string filename = utils::getFilenameFromUrl(url);
    EXPECT_EQ(filename, "");
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}