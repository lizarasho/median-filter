#pragma once

#include <opencv2/core/mat.hpp>

struct image_channels
{
    image_channels(const cv::Mat &image);

    image_channels(int rows, int cols, int channels_number,
                   std::vector<std::vector<uchar>> *channel_r,
                   std::vector<std::vector<uchar>> *channel_g,
                   std::vector<std::vector<uchar>> *channel_b);

    std::vector<std::vector<uchar>> const &get_r() const;

    std::vector<std::vector<uchar>> const &get_g() const;

    std::vector<std::vector<uchar>> const &get_b() const;

    void toMat(cv::Mat &out);

private:
    const int rows;
    const int cols;
    const int channels_number;
    std::vector<std::vector<uchar>> *const channel_r;
    std::vector<std::vector<uchar>> *const channel_g;
    std::vector<std::vector<uchar>> *const channel_b;
};