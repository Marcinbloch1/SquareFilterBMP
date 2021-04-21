// JA_ProjektCpp.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "JA_ProjektCpp.h"


// This is an example of an exported function.
extern "C" JAPROJEKTCPP_API void filterOn(unsigned char* inputData,
    unsigned int width, unsigned int height, unsigned char* outputData)
{
    int rowNumber = 0;
    int pixelNumber = 0;
    int sumBlue = 0;
    int sumGreen = 0;
    int sumRed = 0;
    int bluePixelValue = 0;
    int greenPixelValue = 0;
    int redPixelValue = 0;
    for (rowNumber = 2; rowNumber < (height - 3); rowNumber++)
    {
        for (pixelNumber = 2; pixelNumber < (width - 3); pixelNumber++)
        {
            sumBlue = 0;
            sumGreen = 0;
            sumRed = 0;

            for (int i = -2; i <= 2; i++)
            {
                for (int j = -2; j <= 2; j++)
                {
                    bluePixelValue = inputData[3 * ((pixelNumber + j) + (width * (rowNumber + i))) + 0];
                    greenPixelValue = inputData[3 * ((pixelNumber + j) + (width * (rowNumber + i))) + 1];
                    redPixelValue = inputData[3 * ((pixelNumber + j) + (width * (rowNumber + i))) + 2];

                    sumBlue += bluePixelValue;
                    sumGreen += greenPixelValue;
                    sumRed += redPixelValue;
                }
            }

            sumBlue /= 25;
            sumGreen /= 25;
            sumRed /= 25;

            outputData[3 * (pixelNumber + (width * rowNumber)) + 0] = sumBlue;
            outputData[3 * (pixelNumber + (width * rowNumber)) + 1] = sumGreen;
            outputData[3 * (pixelNumber + (width * rowNumber)) + 2] = sumRed;
        }
    }
}
