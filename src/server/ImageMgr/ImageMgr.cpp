//
// Created by winfidonarleyan on 10.09.22.
//

#include "ImageMgr.h"
#include "Log.h"
#include <sstream>
#include <jpeglib.h>

ImageMgr* ImageMgr::instance()
{
    static ImageMgr instance;
    return &instance;
}

bool ImageMgr::GetBinaryMirrorImage(std::string_view fromBinaryData, std::string& toBinaryData)
{
    WarheadJpegImage jpegImage{};
    return GetImageData(fromBinaryData, jpegImage) && MirrorImage(jpegImage) && SaveImageData(jpegImage, toBinaryData);
}

bool ImageMgr::GetImageData(std::string_view fromBinaryData, WarheadJpegImage& jpegImage)
{
    if (fromBinaryData.empty())
        return false;

    // init decoder
    jpeg_decompress_struct cinfo{};
    jpeg_error_mgr jerr{};

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, (unsigned char*)fromBinaryData.data(), fromBinaryData.length());
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    int row_size = cinfo.output_width * cinfo.output_components;

    JSAMPARRAY row_data = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_size, 1);

    auto* out_buf = (uint8_t*)malloc(row_size * cinfo.output_height);
    jpegImage.JpegData = out_buf;

    // read row by row
    while (cinfo.output_scanline < cinfo.output_height)
    {
        //read a single row
        jpeg_read_scanlines(&cinfo, row_data, 1);

        //prepare destination pointer
        void* dst = out_buf + row_size * (cinfo.output_scanline - 1); // do -1 as it has already bben increased by readind method

        // move the row data into output buffer
        memmove(dst, row_data[0], row_size);
    }

    // return some data
    jpegImage.Width = cinfo.image_width;
    jpegImage.Height = cinfo.image_height;
    jpegImage.Components = cinfo.output_components;
    jpegImage.ColorSpace = static_cast<uint32>(cinfo.out_color_space);

    // clean up
    (*cinfo.mem->free_pool)((j_common_ptr) &cinfo, JPOOL_IMAGE);

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    return true;
}

bool ImageMgr::MirrorImage(WarheadJpegImage& image)
{
    if (!image.JpegData || image.Components != 3)
        return false;

    uint32 rowSize = image.Width * image.Components;

    // Mirror rows
    for (uint32 i = 0; i < image.Height; i++)
    {
        uint8* row = image.JpegData + i * rowSize;

        for (uint32 j = 0; j < image.Width / 2; j++)
        {
            uint8 tmp_buf[3];

            uint8* pix1 = row + j * image.Components; // Left
            uint8* pix2 = row + rowSize - (j + 1) * image.Components; // Right

            // swap pixels
            memmove(&tmp_buf, pix1, image.Components);
            memmove(pix1, pix2, image.Components);
            memmove(pix2, &tmp_buf, image.Components);
        }
    }

    return true;
}

bool ImageMgr::SaveImageData(WarheadJpegImage const& image, std::string& toBinaryData)
{
    // init encoder
    jpeg_compress_struct cinfo{};
    jpeg_error_mgr jerr{};
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    unsigned char *mem = nullptr;
    unsigned long mem_size = 0;
    jpeg_mem_dest(&cinfo, &mem, &mem_size);

    cinfo.image_width = image.Width;
    cinfo.image_height = image.Height;
    cinfo.input_components = image.Components;
    cinfo.in_color_space = static_cast<J_COLOR_SPACE>(image.ColorSpace);

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 90, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    int row_size = cinfo.image_width * cinfo.input_components;
    JSAMPARRAY row_data = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_size, 1);

    // write raw by row
    while (cinfo.next_scanline < cinfo.image_height)
    {
        // prepare a source pointer
        void* src = image.JpegData + row_size * cinfo.next_scanline;

        // move data from input buffer into single row buffer
        memmove(row_data[0], src, row_size);

        // write it
        jpeg_write_scanlines(&cinfo, row_data, 1);
    }

    // clean up
    (*cinfo.mem->free_pool)((j_common_ptr)&cinfo, JPOOL_IMAGE);

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    toBinaryData = std::string{ (char const*)mem, mem_size };
    return true;
}
