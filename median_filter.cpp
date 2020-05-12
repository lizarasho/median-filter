#include <thread>

#include "image_channels.hpp"
#include "median_filter.hpp"

void median_filter::blur(const cv::Mat &in, cv::Mat &out, const int threshold)
{
    const image_channels channels(in);
    const uchar t = (uchar) (std::min(std::max(0, threshold), 255));

    auto r = channels.get_r();
    auto g = channels.get_g();
    auto b = channels.get_b();

    std::vector<std::vector<uchar>> r_filtered;
    std::vector<std::vector<uchar>> g_filtered;
    std::vector<std::vector<uchar>> b_filtered;

    auto action = [this](const std::vector<std::vector<uchar>> &in, std::vector<std::vector<uchar>> &out, const uchar t)
    {
        process(in, out, t);
    };

    std::thread r_filter(action, std::ref(r), std::ref(r_filtered), t);
    std::thread g_filter(action, std::ref(g), std::ref(g_filtered), t);
    std::thread b_filter(action, std::ref(b), std::ref(b_filtered), t);

    r_filter.join();
    g_filter.join();
    b_filter.join();

    image_channels(in.rows, in.cols, in.channels(), &r_filtered, &g_filtered, &b_filtered).toMat(out);
}

void median_filter::process(const std::vector<std::vector<uchar>> &in, std::vector<std::vector<uchar>> &out, uchar t)
{
    auto n = in.size();
    auto m = in.front().size();

    out.assign(n, std::vector<uchar>(m));

    auto safe_getter = [this](const std::vector<std::vector<uchar>> &in, int i, int j) -> uchar
    { return safe_get(in, i, j); };

    auto getter = [this](const std::vector<std::vector<uchar>> &in, int i, int j) -> uchar
    { return in[i][j]; };

    process_row(0, m, t, in, out, safe_getter, safe_getter);

    for (int i = 1; i < n - 1; i++)
    {
        process_row(i, m, t, in, out, safe_getter, getter);
    }

    process_row(n - 1, m, t, in, out, safe_getter, safe_getter);
}

void median_filter::process_row(
        const int row, const int m, const uchar t,
        const std::vector<std::vector<uchar>> &in, std::vector<std::vector<uchar>> &out,
        const std::function<uchar(const std::vector<std::vector<uchar>> &in, int i, int j)> &safe_getter,
        const std::function<uchar(const std::vector<std::vector<uchar>> &in, int i, int j)> &getter)
{
    std::vector<std::vector<uchar>> cols((size_t) D + 1, std::vector<uchar>((size_t) D));
    std::vector<uchar> middle((size_t) 2 * D - 2);

    fill_cols(cols, in, 0, row, 0, 1, safe_getter);

    for (int j = 0; j < m - 1; j += 2)
    {
        fill_cols(cols, in, 2, row, j + 1, j + 2, getter);
        merge(cols[1], cols[2], middle);

        update_out(row, j, in[row][j], t, cols[0], middle, out);
        update_out(row, j + 1, in[row][j + 1], t, cols[3], middle, out);

        cols[0].swap(cols[2]);
        cols[1].swap(cols[3]);
    }

    if (m % 2 != 0)
    {
        fill_cols(cols, in, 2, row, m - 1, m - 1, getter);
        merge(cols[1], cols[2], middle);
        update_out(row, m - 1, in[row][m - 1], t, cols[0], middle, out);
    }
}

void median_filter::fill_cols(
        std::vector<std::vector<uchar>> &cols, std::vector<std::vector<uchar>> const &in,
        const int begin_ind, const int cur_row, const int from_col, const int to_col,
        const std::function<uchar(const std::vector<std::vector<uchar>> &in, int i, int j)> &getter)
{
    for (int k = from_col, cur_ind = begin_ind; k <= to_col; k++, cur_ind++)
    {
        for (int l = cur_row - 1; l <= cur_row + 1; l++)
        {
            cols[cur_ind][l - (cur_row - 1)] = getter(in, l, k);
        }
        sort(cols[cur_ind]);
    }
}

void median_filter::update_out(const int i, const int j, const uchar current_value, const uchar t,
                               const std::vector<uchar> &col,
                               const std::vector<uchar> &middle, std::vector<std::vector<uchar>> &out)
{
    auto median = get_median(col, middle);
    out[i][j] = (abs(current_value - median) > t) ? median : current_value;
}

uchar median_filter::get_median(const std::vector<uchar> &a, const std::vector<uchar> &middle)
{
    auto median_ind = (a.size() + middle.size() + 1) / 2;
    int i = 0;
    int j = 0;

    for (int k = 0; k < median_ind - 1; k++)
    {
        if (a[i] <= middle[j])
            i++;
        else
            j++;
    }

    return (i != a.size() && a[i] <= middle[j]) ? a[i] : middle[j];
}

void median_filter::merge(const std::vector<uchar> &a, const std::vector<uchar> &b, std::vector<uchar> &result)
{
    int i = a.front() <= b.front();
    int j = 1 - i;

    for (uchar &e : result)
    {
        if (j == b.size() || (i != a.size() && a[i] <= b[j]))
            e = a[i++];
        else
            e = b[j++];
    }
}

void median_filter::sort(std::vector<uchar> &a)
{
    if (a[0] > a[1]) std::swap(a[0], a[1]);
    if (a[1] > a[2])
    {
        std::swap(a[1], a[2]);
        if (a[0] > a[1]) std::swap(a[0], a[1]);
    }
}

uchar median_filter::safe_get(const std::vector<std::vector<uchar>> &a, const int i, const int j)
{
    return valid(i, a.size()) && valid(j, a[i].size()) ? a[i][j] : (uchar) 0;
}

bool median_filter::valid(const int ind, const size_t n)
{
    return ind >= 0 && ind < n;                     // 2 comparision operations
}