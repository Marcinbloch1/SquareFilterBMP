// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the JAPROJEKTASM_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// JAPROJEKTASM_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef JAPROJEKTASM_EXPORTS
#define JAPROJEKTASM_API __declspec(dllexport)
#else
#define JAPROJEKTASM_API __declspec(dllimport)
#endif



extern "C" JAPROJEKTASM_API void filterOn(unsigned char* inputData,
    unsigned int width, unsigned int height, unsigned char* outputData);
