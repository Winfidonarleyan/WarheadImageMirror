//
// Created by winfidonarleyan on 10.09.22.
//

#ifndef WARHEADIMAGEMIRROR_IMAGEMGR_H
#define WARHEADIMAGEMIRROR_IMAGEMGR_H

#include "Define.h"
#include <string_view>

struct WH_SERVER_API WarheadJpegImage;

class WH_SERVER_API ImageMgr
{
public:
    static ImageMgr* instance();

    static bool GetBinaryMirrorImage(std::string_view fromBinaryData, std::string& toBinaryData);

private:
    static bool GetImageData(std::string_view fromBinaryData, WarheadJpegImage& image);
    static bool SaveImageData(WarheadJpegImage const& image, std::string& toBinaryData);
    static bool MirrorImage(WarheadJpegImage& image);

    ImageMgr() = default;
    ~ImageMgr() = default;
    ImageMgr(ImageMgr const&) = delete;
    ImageMgr(ImageMgr&&) = delete;
    ImageMgr& operator=(ImageMgr const&) = delete;
    ImageMgr& operator=(ImageMgr&&) = delete;
};

#define sImageMgr ImageMgr::instance()

#endif //WARHEADIMAGEMIRROR_IMAGEMGR_H
