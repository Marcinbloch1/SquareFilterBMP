#include "Histogram.h"

Histogram::Histogram(std::vector<uint8_t> &data)
{
	uint pixelCount = data.size();

	//counting occurences
	for (int i = 0; i < pixelCount; i += 3)
	{
		blueDistribution[data[i]]++;
		greenDistribution[data[i + 1]]++;
		redDistribution[data[i + 2]]++;
	}
}