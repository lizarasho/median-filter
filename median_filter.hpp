#pragma once

#include <opencv2/core/mat.hpp>

struct median_filter
{
    void blur(const cv::Mat &in, cv::Mat &out, int threshold = 0);

private:
    const int D = 3;

    void process(const std::vector<std::vector<uchar>> &in, std::vector<std::vector<uchar>> &out, uchar t);

    void process_row(int row, int m, uchar t,
                     const std::vector<std::vector<uchar>> &in, std::vector<std::vector<uchar>> &out,
                     const std::function<uchar(const std::vector<std::vector<uchar>> &in, int i, int j)> &safe_getter,
                     const std::function<uchar(const std::vector<std::vector<uchar>> &in, int i, int j)> &getter);

    void fill_cols(std::vector<std::vector<uchar>> &cols, std::vector<std::vector<uchar>> const &in,
                   int begin_ind, int cur_row, int from_col, int to_col,
                   const std::function<uchar(const std::vector<std::vector<uchar>> &in, int i, int j)> &getter);

    void update_out(int i, int j, uchar current_value, uchar t, const std::vector<uchar> &col,
                    const std::vector<uchar> &middle, std::vector<std::vector<uchar>> &out);

    uchar get_median(const std::vector<uchar> &a, const std::vector<uchar> &middle);

    void merge(const std::vector<uchar> &a, const std::vector<uchar> &b, std::vector<uchar> &result);

    void sort(std::vector<uchar> &a);

    uchar safe_get(const std::vector<std::vector<uchar>> &a, int i, int j);

    bool valid(int ind, size_t n);
};
