#include "JA_Projekt.h"

JA_Projekt::JA_Projekt(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    //connecting ui to tasks
    connect(ui.LoadImageButton, SIGNAL(clicked()), this, SLOT(clicked_LoadButton()));
    connect(ui.SaveImageButton, SIGNAL(clicked()), this, SLOT(clicked_SaveButton()));
    connect(ui.FilterOnButton, SIGNAL(clicked()), this, SLOT(clicked_FilterOnButton()));
    connect(ui.HistogramButton, SIGNAL(clicked()), this, SLOT(clicked_HistogramButton()));
    connect(ui.OutputHistogramButton, SIGNAL(clicked()), this, SLOT(clicked_OutputHistogramButton()));

    ui.MessageText->setAlignment(Qt::AlignCenter);
}

void JA_Projekt::clicked_LoadButton()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Choose BMP file"), "", "*.bmp");
    fileNameString = filename.toStdString();

    read(fileNameString);
}

void JA_Projekt::clicked_SaveButton()
{
    write(WriteFileString);
}

void JA_Projekt::clicked_HistogramButton()
{
    draw_histogram(inputData);
}

void JA_Projekt::clicked_OutputHistogramButton()
{
    draw_histogram(outputData);
}

void JA_Projekt::clicked_FilterOnButton()
{
    if (!inputData.empty()) 
    {
        int temp = 0;
        typedef void(*FilterOnFunction)(unsigned char*, unsigned int, unsigned int, unsigned char*);
        HMODULE FilterLib = NULL;
        FilterOnFunction FilterFunction = NULL;
        try
        {
            if (ui.CppRadioButton->isChecked())
            {
                FilterLib = LoadLibraryA("..\\x64\\Debug\\JAProjektCpp.dll");
                if (FilterLib == NULL)
                    throw std::exception("Error loading dll: JAProjektCpp.dll");
                temp = 1;
            }
            else if (ui.AsmRadioButton->isChecked())
            {
                FilterLib = LoadLibraryA("..\\x64\\Debug\\JAProjektAsm.dll");
                if (FilterLib == NULL)
                    throw std::exception("Error loading dll: JAProjektAsm.dll");
                temp = 2;
            }
            else
            {
                throw std::exception("Nothing checked");
            }
            FilterFunction = (FilterOnFunction)GetProcAddress(FilterLib, "filterOn");
            if (FilterFunction == NULL)
                throw std::exception("Error reading dll");
        }
        catch (const std::exception& ex)
        {
            QString message(ex.what());
            ui.MessageText->setText("<font color='red'>" + message + "</font>");
            return;
        }

        std::chrono::time_point<std::chrono::high_resolution_clock> start, end;

        start = std::chrono::high_resolution_clock::now();
        FilterFunction(inputData.data(), bmp_info_header.width, bmp_info_header.height, outputData.data());
        end = std::chrono::high_resolution_clock::now();

        FreeLibrary(FilterLib);

        auto duration = end - start;
        std::stringstream ss;
        ss << duration.count() << " nanosec";
        QString durationString(ss.str().c_str());

        if (temp == 1)
            cppString = durationString;
        else if (temp == 2)
            asmString = durationString;

        ui.ExecutionText->setText("Execution time:\nC++:\n" + cppString + "\n\nASM:\n" + asmString);

        ui.MessageText->setText("<font color='red'>Filter on</font>");
    }
    else 
    {
        ui.MessageText->setText("<font color='red'>Load correct bmp file</font>");
    }
    
}

void JA_Projekt::read(std::string fname)
{
    std::ifstream inp{ fname, std::ios_base::binary };
    if (inp) 
    {
        inp.read((char*)&file_header, sizeof(file_header));
        if (file_header.file_type != 0x4D42) 
        {
            throw std::runtime_error("Error! Unrecognized file format.");
        }
        inp.read((char*)&bmp_info_header, sizeof(bmp_info_header));

        // Jump to the pixel data location
        inp.seekg(file_header.offset_data, inp.beg);

        // Adjust the header fields for output.
        // Some editors will put extra info in the image file, we only save the headers and the data.
        if (bmp_info_header.bit_count == 24)
        {
            bmp_info_header.size = sizeof(BMPInfoHeader);
            file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
        }
        else
        {
            throw std::runtime_error("Program can only treat 24 bit color bitmap.");
        }
        file_header.file_size = file_header.offset_data;

        if (bmp_info_header.height < 0) 
        {
            throw std::runtime_error("The program can treat only BMP images with the origin in the bottom left corner!");
        }

        inputData.resize(bmp_info_header.width * bmp_info_header.height * bmp_info_header.bit_count / 8);

        // Here we check if we need to take into account row padding
        if (bmp_info_header.width % 4 == 0) 
        {
            inp.read((char*)inputData.data(), inputData.size());
            file_header.file_size += static_cast<uint32_t>(inputData.size());
        }
        else 
        {
            row_stride = bmp_info_header.width * bmp_info_header.bit_count / 8;
            uint32_t new_stride = make_stride_aligned(4);
            std::vector<uint8_t> padding_row(new_stride - row_stride);

            for (int y = 0; y < bmp_info_header.height; ++y) 
            {
                inp.read((char*)(inputData.data() + row_stride * y), row_stride);
                inp.read((char*)padding_row.data(), padding_row.size());
            }
            file_header.file_size += static_cast<uint32_t>(inputData.size()) + bmp_info_header.height * static_cast<uint32_t>(padding_row.size());
        }

        outputData = inputData;
        ui.MessageText->setText("<font color='red'>Image loaded</font>");
    }
    else 
    {
        ui.MessageText->setText("<font color='red'>Unable to open <br>the input image file</font>");
        //throw std::runtime_error("Unable to open the input image file.");
    }
}

void JA_Projekt::write(std::string fname)
{
    std::ofstream of{ fname, std::ios_base::binary };
    if (of)
    {
        if (bmp_info_header.bit_count == 24)
        {
            if (bmp_info_header.width % 4 == 0)
            {
                write_headers_and_data(of);
            }
            else
            {
                uint32_t new_stride = make_stride_aligned(4);
                std::vector<uint8_t> padding_row(new_stride - row_stride);

                write_headers(of);

                for (int y = 0; y < bmp_info_header.height; ++y)
                {
                    of.write((const char*)(outputData.data() + row_stride * y), row_stride);
                    of.write((const char*)padding_row.data(), padding_row.size());
                }
            }

            ui.MessageText->setText("<font color='red'>Image saved</font>");
        }
        else
        {
            //throw std::runtime_error("The program can treat only 24 bits per pixel BMP files");
            ui.MessageText->setText("<font color='red'>Unable to open the<br>output image file.</font>");
        }
    }
    else
    {
        //throw std::runtime_error("Unable to open the output image file.");
        ui.MessageText->setText("<font color='red'>Unable to open the<br>output image file.</font>");
    }
}

void JA_Projekt::draw_histogram(std::vector<uint8_t>& inputData)
{
    ui.horizontalLayoutWidget->layout()->removeWidget(chartView);
    chart->removeAxis(axisX);
    chart->removeAxis(axisY);

    Histogram histogram(inputData);
    
    QStackedBarSeries* pSeries = new QStackedBarSeries();

    QBarSet* setred = new QBarSet("Red");
    QBarSet* setgreen = new QBarSet("Green");
    QBarSet* setblue = new QBarSet("Blue");
    setred->setColor(Qt::red);
    setgreen->setColor(Qt::green);
    setblue->setColor(Qt::blue);

    uint pixelCount = inputData.size();
    double pixelCount_f = static_cast<double>(pixelCount);

    double totalMaxValue = 0.0;

    for (int i = 0; i < 256; i++)
    {
        auto calcPart = [=](int value, int& maxValue) -> int
        {
            int temp = value - maxValue;
            if (temp > 0)
            {
                maxValue = value;
                return temp;
            }
            else
            {
                return 0;
            }
        };

        int redPart = 0, greenPart = 0, bluePart = 0, maxPart = 0;
        redPart = calcPart(histogram.redDistribution[i], maxPart);
        greenPart = calcPart(histogram.greenDistribution[i], maxPart);
        bluePart = calcPart(histogram.blueDistribution[i], maxPart);

        *setred << redPart * 100.0 / pixelCount_f;
        *setgreen << greenPart * 100.0 / pixelCount_f;
        *setblue << bluePart * 100.0 / pixelCount_f;

        double redPart_f = redPart * 100.0 / pixelCount_f;
        double greenPart_f = redPart * 100.0 / pixelCount_f;
        double bluePart_f = redPart * 100.0 / pixelCount_f;

        double m1 = std::max(redPart_f, greenPart_f);
        double m2 = std::max(redPart_f, bluePart_f);
        double maxValue = std::max(m1, m2);
        if (maxValue > totalMaxValue)
        {
            totalMaxValue = maxValue;
        }
    }

    pSeries->append(setred);
    pSeries->append(setgreen);
    pSeries->append(setblue);

    
    chart->removeAllSeries();
    chart->setTitle("Histogram barw RGB");
    chart->setAnimationOptions(QChart::SeriesAnimations);


    QStringList categories;
    categories << "64" << "128" << "192" << "256";
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    pSeries->attachAxis(axisX);

    axisY->setRange(0, totalMaxValue);
    chart->addAxis(axisY, Qt::AlignLeft);

    chart->addSeries(pSeries);
    chart->legend()->setVisible(false);

    chartView->setRenderHint(QPainter::Antialiasing);
    
    ui.horizontalLayoutWidget->layout()->addWidget(chartView);

    ui.MessageText->setText("<font color='red'>Displaying histogram</font>");
}

void JA_Projekt::write_headers(std::ofstream& of)
{
    of.write((const char*)&file_header, sizeof(file_header));
    of.write((const char*)&bmp_info_header, sizeof(bmp_info_header));
}

void JA_Projekt::write_headers_and_data(std::ofstream& of)
{
    write_headers(of);
    of.write((const char*)outputData.data(), outputData.size());
}

uint32_t JA_Projekt::make_stride_aligned(uint32_t align_stride)
{
     uint32_t new_stride = row_stride;
     while (new_stride % align_stride != 0) 
     {
         new_stride++;
     }
     return new_stride;
}