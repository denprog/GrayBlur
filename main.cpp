#include <iostream>
#include <QImage>
#include <chrono>
#include <boost/program_options.hpp>
#include "pool.h"

int main(int ac, char** av)
{
    std::string file_name;
    uint threads = 0;

    try
    {
        boost::program_options::positional_options_description pos;
        pos.add("filename", 1).add("threads", -1);

        boost::program_options::variables_map vm;
        auto parsed = boost::program_options::command_line_parser(ac, av).positional(pos).run();

        if (parsed.options.size() >= 1)
            file_name = parsed.options[0].value[0];
        if (parsed.options.size() == 2)
            threads = std::stoi(parsed.options[1].value[0]);
    }
    catch (std::exception& ex)
    {
        std::cout << "Error parsing command line: " << ex.what() << "\n";
        return 1;
    }

    if (file_name.empty())
    {
        std::cout << "File name missed";
        return 1;
    }

    if (threads == 0)
        threads = std::thread::hardware_concurrency();

    //открыть файл
    QImage image(file_name.c_str());
    if (image.isNull())
    {
        std::cout << "Error open image";
        return 0;
    }

    //сконвертировать в BMP и взять буфер
    QImage source_image = image.convertToFormat(QImage::Format_RGB888);
    std::vector<uchar> source_buf;
    for (int i = 0; i < source_image.width() * source_image.height() * 3; ++i)
        source_buf.push_back(source_image.bits()[i]);

    std::vector<uchar> res_buf;
    res_buf.reserve(source_buf.size());

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    Pool pool(&source_buf[0], source_buf.size(), &res_buf[0], source_buf.size(), threads);
    pool.Start(); //запуск алгоритмов
    pool.Join(); //подождать окончания

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << " мкс" << std::endl;

    //сконвертировать в JPG
    QImage res_image(res_buf.data(), source_image.width(), source_image.height(), QImage::Format_RGB888);
    res_image.save("out.jpg");

    return 0;
}
