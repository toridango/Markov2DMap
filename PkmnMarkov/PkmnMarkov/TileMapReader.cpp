

#include "TileMapReader.h"


TileMapReader::TileMapReader()
{
	m_max = -100000;
}


TileMapReader::~TileMapReader()
{
}


int TileMapReader::VecToInt(std::vector<int> v, bool bigEndian = true)
{
	int result = 0;
	for (unsigned int i = bigEndian ? v.size() : 0; bigEndian ? i-- > 0 : i++ < v.size();)
	{
		result += bigEndian ? v[i] * pow(10, v.size() - 1 - i) : v[i - 1] * pow(10, i - 1);
	}
	return result;
}


bool TileMapReader::ReadFile(char* filename, int height, int width)
{
	char ch;
	std::fstream fin(filename, std::fstream::in);
	int currDimension = 0;
	int row = -1;
	int col = 0;
	int max = -100000;
	std::vector<int> digits;

	m_height = height;
	m_width = width;
	m_map.resize(height);
	for (auto &i : m_map) i.resize(width);

	while ((fin >> std::skipws >> ch) /*&& col < 100*/) 
	{
		switch (currDimension)
		{
		case 0:
		{
			// DOCUMENT START
			if ('[' == (ch))
			{
				currDimension++;
			}
			break;
		}
		case 1:
		{
			// NEW ROW
			if ('[' == ch)
			{
				currDimension++;
				row++;
			}
			// END OF DOCUMENT
			else if (']' == ch)
			{
				currDimension--;
			}
			break;
		}
		case 2:
		{
			// CORE SCANNER
			// END OF ROW
			if (']' == ch)
			{
				// whoops almost forgot to save the number at the edge case "last element of the row"
				int v = VecToInt(digits);
				if (v > max) max = v;

				// save it, clear stack and advance index
				m_map[row][col] = v;
				digits.clear();

				// PLUS NEXT ROW STUFF 
				currDimension--;
				col = 0;
			}
			// END OF NUMBER
			else if (',' == ch)
			{
				// get the number from the stack of found digits
				int v = VecToInt(digits);

				//DEBUG STUFF
				//if(v > 1492)
				//	std::cout << v << ", ";
				if (v > max) max = v;

				// save it, clear stack and advance index
				m_map[row][col] = v;
				digits.clear();
				col++;
			}
			else if (ch == '\n')
			{
				// skip
			}
			// NEW DIGIT FOUND
			else if (isdigit(ch))
			{
				if (ch - '0' < 10)
					digits.push_back(ch - '0');
			}
			break;
		}
		default:
			return false;
		}

	}
	m_max = max;
	std::cout << "Highest value found: " << max << "\n";
	return true;
}


std::vector<std::vector<int>>  TileMapReader::GetMap()
{
	return m_map;
}

int TileMapReader::GetMax()
{
	return m_max;
}

int TileMapReader::GetHeight()
{
	return m_height;
}

int TileMapReader::GetWidth()
{
	return m_width;
}