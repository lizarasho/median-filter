#include <opencv2/core/mat.hpp>

#include "image_channels.hpp"


image_channels::image_channels(cv::Mat const &image)
        : rows(image.rows),
          cols(image.cols),
          channels_number(image.channels()),
          channel_r(new std::vector<std::vector<uchar>>((size_t) rows, std::vector<uchar>((size_t) cols))),
          channel_g(new std::vector<std::vector<uchar>>((size_t) rows, std::vector<uchar>((size_t) cols))),
          channel_b(new std::vector<std::vector<uchar>>((size_t) rows, std::vector<uchar>((size_t) cols)))
{
    uchar *data = image.data;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            auto ind = image.step * i + j * image.channels();
            (*channel_b)[i][j] = data[ind];
            (*channel_g)[i][j] = data[ind + 1];
            (*channel_r)[i][j] = data[ind + 2];
        }
    }
}

image_channels::image_channels(const int rows, const int cols, const int channels_number,
                               std::vector<std::vector<uchar>> *const channel_r,
                               std::vector<std::vector<uchar>> *const channel_g,
                               std::vector<std::vector<uchar>> *const channel_b)
        : rows(rows), cols(cols), channels_number(channels_number),
          channel_r(channel_r), channel_g(channel_g), channel_b(channel_b)
{}

std::vector<std::vector<uchar>> const &image_channels::get_r() const
{
    return *channel_r;
}

std::vector<std::vector<uchar>> const &image_channels::get_g() const
{
    return *channel_g;
}

std::vector<std::vector<uchar>> const &image_channels::get_b() const
{
    return *channel_b;
}

void image_channels::toMat(cv::Mat &out)
{
    auto step = cols * channels_number;
    auto *data = new uchar[rows * cols * channels_number];

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            auto ind = step * i + j * channels_number;
            data[ind] = (*channel_b)[i][j];
            data[ind + 1] = (*channel_g)[i][j];
            data[ind + 2] = (*channel_r)[i][j];
        }
    }
    out = cv::Mat(rows, cols, CV_8UC3, data);
}