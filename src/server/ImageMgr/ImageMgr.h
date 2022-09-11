//
// Created by winfidonarleyan on 10.09.22.
//

#ifndef WARHEADIMAGEMIRROR_IMAGEMGR_H
#define WARHEADIMAGEMIRROR_IMAGEMGR_H

#include "Define.h"
#include <vector>
#include <string_view>

struct WarheadJpegImage
{
    uint8* JpegData{ nullptr };
    uint32 Width{};
    uint32 Height{};
    uint32 Components{};
    uint32 ColorSpace{};
};

class ImageMgr
{
public:
    static ImageMgr* instance();

    bool GetBinaryMirrorImage(std::string_view fromBinaryData, std::string& toBinaryData);


private:
    bool GetImageData(std::string_view fromBinaryData, WarheadJpegImage& image);
    bool SaveImageData(WarheadJpegImage const& image, std::string& toBinaryData);
    bool MirrorImage(WarheadJpegImage& image);

    ImageMgr() = default;
    ~ImageMgr() = default;
    ImageMgr(ImageMgr const&) = delete;
    ImageMgr(ImageMgr&&) = delete;
    ImageMgr& operator=(ImageMgr const&) = delete;
    ImageMgr& operator=(ImageMgr&&) = delete;
};

#define sImageMgr ImageMgr::instance()

#endif //WARHEADIMAGEMIRROR_IMAGEMGR_H
