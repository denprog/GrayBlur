#ifndef POOL_H
#define POOL_H

#include <thread>
#include <vector>
#include <functional>
#include <atomic>

#define BUFFER_PART 1000

struct Buffer
{
    Buffer(unsigned char* _buffer, const uint _size) :
        buffer(_buffer),
        size(_size)
    {
        cur_pos = 0;
    }

    unsigned char* buffer;
    const uint size;
    std::atomic<uint> cur_pos; //текущая позиция выставляется потоком, который возьмет новую часть буфера
};

class Pool
{
public:
    Pool(unsigned char* source_buf, const uint source_buf_size, unsigned char* res_buf, const uint res_buf_size, const uint _pool_size);
    ~Pool();

    void Start();
    void Join();

private:
    void Thread();

    void GreyScale(uint pos);

    void GaussBlur(uint pos, std::vector<double>& kernel);
    double Gauss(double sigma, double x);
    std::vector<double> GaussKernel(int samples, double sigma);

private:
    std::vector<std::thread> threads; //пул потоков
    const uint pool_size; //количество потоков

    Buffer source_buffer; //исходный буфер
    Buffer grayscale_buffer; //промежуточный буфер с grayscale картинкой
    Buffer res_buffer; //результат с gaussian blur картинкой

    const double sigma = 0.5;
    const int samples = 100;
};

#endif // POOL_H
