#pragma once

#include <QtWidgets/QMainWindow>
#include <QFileDialog>
#include <QtCharts/qbarset.h>
#include <QtCharts/qbarseries.h>
#include <QtCharts/qchartview.h>
#include <QtCharts/qlegend.h>
#include <QtCharts/qbarcategoryaxis.h>
#include <QtCharts/qvalueaxis.h>
#include <QtCharts/qstackedbarseries.h>
#include <QtCharts/qlineseries.h>
#include <QtCharts/qscatterseries.h>
#include <qsignalmapper.h>
#include <cstdio>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <string>
#include "ui_JA_Projekt.h"
#include "Histogram.h"
#include <Windows.h>
#include <sstream>

QT_CHARTS_USE_NAMESPACE

#pragma pack(push, 1)
struct BMPFileHeader {
    uint16_t file_type{ 0x4D42 };          // File type always BM which is 0x4D42 (stored as hex uint16_t in little endian)
    uint32_t file_size{ 0 };               // Size of the file (in bytes)
    uint16_t reserved1{ 0 };               // Reserved, always 0
    uint16_t reserved2{ 0 };               // Reserved, always 0
    uint32_t offset_data{ 0 };             // Start position of pixel data (bytes from the beginning of the file)
};

struct BMPInfoHeader {
    uint32_t size{ 0 };                      // Size of this header (in bytes)
    int32_t width{ 0 };                      // width of bitmap in pixels
    int32_t height{ 0 };                     // width of bitmap in pixels
                                             //       (if positive, bottom-up, with origin in lower left corner)
                                             //       (if negative, top-down, with origin in upper left corner)
    uint16_t planes{ 1 };                    // No. of planes for the target device, this is always 1
    uint16_t bit_count{ 0 };                 // No. of bits per pixel
    uint32_t compression{ 0 };               // 0 or 3 - uncompressed. THIS PROGRAM CONSIDERS ONLY UNCOMPRESSED BMP images
    uint32_t size_image{ 0 };                // 0 - for uncompressed images
    int32_t x_pixels_per_meter{ 0 };
    int32_t y_pixels_per_meter{ 0 };
    uint32_t colors_used{ 0 };               // No. color indexes in the color table. Use 0 for the max number of colors allowed by bit_count
    uint32_t colors_important{ 0 };          // No. of colors used for displaying the bitmap. If 0 all colors are required
};
#pragma pack(pop)



class JA_Projekt : public QMainWindow
{
    Q_OBJECT

private:
    Ui::JA_ProjektClass ui;
    std::string fileNameString;
    std::string WriteFileString = "output.bmp";
    uint32_t row_stride{ 0 };
    uint pixelCount;
    QString cppString = "0 nanosec";
    QString asmString = "0 nanosec";

public:
    BMPFileHeader file_header;
    BMPInfoHeader bmp_info_header;
    std::vector<uint8_t> inputData;
    std::vector<uint8_t> outputData;

    QChart* chart = new QChart();
    QChartView* chartView = new QChartView(chart);
    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    QValueAxis* axisY = new QValueAxis();

private:
    void write_headers(std::ofstream& of);
    void write_headers_and_data(std::ofstream& of);
    
    // Add 1 to the row_stride until it is divisible with align_stride
    uint32_t make_stride_aligned(uint32_t align_stride);
    

public:
    JA_Projekt(QWidget* parent = Q_NULLPTR);
    void read(std::string fname);
    void write(std::string fname);
    void draw_histogram(std::vector<uint8_t> &inputData);

private slots:
    void clicked_LoadButton();
    void clicked_SaveButton();
    void clicked_HistogramButton();
    void clicked_OutputHistogramButton();
    void clicked_FilterOnButton();
};
