#include "pool.h"
#include <assert.h>
#include <cstring>
#include <cmath>

//Pool

Pool::Pool(unsigned char* source_buf, const uint source_buf_size,
    unsigned char* res_buf, const uint res_buf_size,
    const uint _pool_size) :
    pool_size(_pool_size),
    source_buffer(source_buf, source_buf_size),
    grayscale_buffer(new unsigned char[source_buf_size], source_buf_size),
    res_buffer(res_buf, res_buf_size)
{
}

Pool::~Pool()
{
    delete[] grayscale_buffer.buffer;
}

void Pool::Start()
{
    for (uint i = 0; i < pool_size; ++i)
        threads.emplace_back(std::thread(&Pool::Thread, this));
}

void Pool::Join()
{
    for (uint i = 0; i < pool_size; ++i)
        threads[i].join();
}

void Pool::Thread()
{
    uint cur_pos = 0;
    while (cur_pos < source_buffer.size)
    {
        cur_pos = source_buffer.cur_pos.fetch_add(BUFFER_PART); //начинать с новой текущей позиции в буфере
        GreyScale(cur_pos);
    }

    std::vector<double> kernel = GaussKernel(samples, sigma);
    cur_pos = 0;
    while (cur_pos < grayscale_buffer.size)
    {
        cur_pos = grayscale_buffer.cur_pos.fetch_add(BUFFER_PART);
        GaussBlur(cur_pos, kernel);
    }
}

void Pool::GreyScale(uint pos)
{
    for (uint i = pos; i < pos + BUFFER_PART - 2; ++i)
    {
        if (i + 2 >= source_buffer.size)
            return;

        double r = source_buffer.buffer[i];
        double g = source_buffer.buffer[i + 1];
        double b = source_buffer.buffer[i + 2];
        unsigned char gray = (unsigned char)(r * 0.3 + g * 0.58 + b * 0.11);
        memset(&grayscale_buffer.buffer[i], gray, 3);
    }
}

void Pool::GaussBlur(uint pos, std::vector<double>& kernel)
{
    int sample_side = samples / 2;
    for (uint i = pos; i < pos + BUFFER_PART; ++i)
    {
        if (i >= source_buffer.size)
            return;

        double sample = 0;
        int sample_ctr = 0;
        for (long j = i - sample_side; j <= i + sample_side; j++)
        {
            if (j > 0 && j < grayscale_buffer.size)
            {
                int index = sample_side + (j - i);
                sample += kernel[index] * grayscale_buffer.buffer[j];
                sample_ctr++;
            }
        }
        res_buffer.buffer[i] = sample / (double)sample_ctr;
    }
}

double Pool::Gauss(double sigma, double x)
{
    double exp_val = -1 * (pow(x, 2) / pow(2 * sigma, 2));
    double divider = sqrt(2 * M_PI * pow(sigma, 2));
    return (1 / divider) * exp(exp_val);
}

std::vector<double> Pool::GaussKernel(int samples, double sigma)
{
    std::vector<double> v;

    bool double_center = false;
    if (samples % 2 == 0)
    {
        double_center = true;
        samples--;
    }
    int steps = (samples - 1) / 2;
    double step_size = (3 * sigma) / steps;

    for (int i = steps; i >= 1; i--)
        v.push_back(Gauss(sigma, i * step_size * -1));

    v.push_back(Gauss(sigma, 0));
    if (double_center)
        v.push_back(Gauss(sigma, 0));

    for (int i = 1; i <= steps; i++)
        v.push_back(Gauss(sigma, i * step_size));

    return v;
}
