#include <iostream>
#include <opencv2/imgcodecs/imgcodecs.hpp>

#include "median_filter.hpp"

const std::string OUT_IMAGE = "out.jpg";
const std::string USAGE = "Usage: image_path threshold\n  "
                          "image_path: absolute path to RGB image for median filtering\n  "
                          "threshold: integer value specifying median filter blurring\n";

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Expected 2 arguments\n" << USAGE;
        return -1;
    }

    std::string image_path;
    try
    {
        image_path = cv::samples::findFile(argv[1]);
    } catch (cv::Exception &e)
    {
        std::cerr << "Couldn't find data file by path \'" << argv[1] << "\'\n" << USAGE;
        return -1;
    }

    int threshold;
    try
    {
        threshold = std::stoi(argv[2]);
    } catch (std::invalid_argument &e)
    {
        std::cerr << "Expected integer threshold, found \'" << argv[2] << "\'\n" << USAGE;
        return -1;
    }

    cv::Mat img = cv::imread(image_path);
    cv::Mat out;

    median_filter filter;
    filter.blur(img, out, threshold);

    imwrite(OUT_IMAGE, out);

    return 0;
}