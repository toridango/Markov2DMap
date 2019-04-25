#pragma once

#include <array>
#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

const int MAP_HEIGHT = 400;
const int MAP_WIDTH = 408;

class TileMapReader
{
public:
	TileMapReader();
	~TileMapReader();

	bool ReadFile(char* filename, int height = MAP_HEIGHT, int width = MAP_WIDTH);
	std::vector<std::vector<int>> GetMap();
	int GetMax();
	int GetHeight();
	int GetWidth();

private:

	int VecToInt(std::vector<int> v, bool bigEndian);

private:
	int m_max;
	int m_height;
	int m_width;
	std::vector<std::vector<int>> m_map;

};

