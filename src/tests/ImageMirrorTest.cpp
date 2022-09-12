/*
 *
 */

#define BOOST_TEST_MODULE ImageMirrorTest
#include <boost/test/unit_test.hpp>
#include "ImageMgr.h"
#include <fstream>

void GetBinaryDataFromFile(std::string_view path, std::string& data)
{
    std::ifstream file(path.data(), std::ios::in | std::ios::binary);
    if (!file.is_open())
        return;

    std::stringstream ss;
    ss << file.rdbuf();
    data = ss.str();
}

BOOST_AUTO_TEST_CASE(TestMirrorImage)
{
    std::string srcBinary;
    std::string srcBinaryToMirror;
    std::string mirrorBinary;

    GetBinaryDataFromFile(std::string(WARHEAD_TEST_IMG_DIR) + "/DefaultImage.jpg", srcBinary);
    GetBinaryDataFromFile(std::string(WARHEAD_TEST_IMG_DIR) + "/MirrorImage.jpg", mirrorBinary);

    BOOST_CHECK(!srcBinary.empty());
    BOOST_CHECK(!mirrorBinary.empty());

    auto result = sImageMgr->GetBinaryMirrorImage(srcBinary, srcBinaryToMirror);
    BOOST_CHECK(result);

    BOOST_CHECK_EQUAL(srcBinaryToMirror, mirrorBinary);
}
