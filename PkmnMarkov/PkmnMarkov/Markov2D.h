#pragma once

#include <vector>
#include <cmath>
#include <iostream>
#include <random>

#include <opencv2/opencv.hpp>

#include "TileMapReader.h"

class Markov2D
{
public:
	Markov2D();
	~Markov2D();

	template <typename T>
	T randomFrom(const T min, const T max);

	void Analyse(TileMapReader* tmr, bool saveData = true, bool verbose = false);
	void GenerateMap(int width, int height, bool killNullCells = true);
	cv::Mat MakeMapImage(cv::Mat map);

	void ReadHorizontalData(std::string filename, bool verbose = false);
	void ReadVerticalData(std::string filename, bool verbose = false);


private:
	std::vector<double> m_horizStateCount;
	std::vector<double> m_vertiStateCount;

	// For each tile: chances to find 
	// any other given tile to its right
	//std::vector<std::vector<double>> m_horizData;
	cv::Mat m_horizData;

	// For each tile: chances to find 
	// any other given tile under it
	//std::vector<std::vector<double>> m_vertiData;
	cv::Mat m_vertiData;


};

