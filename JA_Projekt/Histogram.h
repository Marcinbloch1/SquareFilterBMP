#pragma once
#include <qvector.h>

class Histogram
{
private:
	QVector<int> redDistribution = QVector<int>(256);
	QVector<int> greenDistribution = QVector<int>(256);
	QVector<int> blueDistribution = QVector<int>(256);

public:
	Histogram(std::vector<uint8_t> &data);

	friend class JA_Projekt;
};