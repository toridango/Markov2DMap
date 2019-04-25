#include "Markov2D.h"



Markov2D::Markov2D()
{
}


Markov2D::~Markov2D()
{
}

template <typename T>
T Markov2D::randomFrom(const T min, const T max)
{
	static std::random_device rdev;
	static std::default_random_engine re(rdev());
	typedef typename std::conditional<
		std::is_floating_point<T>::value,
		std::uniform_real_distribution<T>,
		std::uniform_int_distribution<T>>::type dist_type;
	dist_type uni(min, max);
	return static_cast<T>(uni(re));
}


void Markov2D::Analyse(TileMapReader* tmr, bool saveData, bool verbose)
{

	std::cout << "Analysing original tile map" << std::endl;
	int states = tmr->GetMax();
	std::vector<std::vector<int>> tmap = tmr->GetMap();
	std::vector<std::vector<double>> horizData;
	std::vector<std::vector<double>> vertiData;

	horizData.resize(states + 1);
	for (auto &i : horizData) i.resize(states + 1);
	vertiData.resize(states + 1);
	for (auto &i : vertiData) i.resize(states + 1);

	m_horizStateCount.resize(states + 1);
	m_vertiStateCount.resize(states + 1);

	for (int row = 0; row < tmr->GetHeight(); ++row)
	{
		for (int col = 0; col < tmr->GetWidth(); ++col)
		{
			int currTile = tmap[row][col];

			if (col + 1 != tmr->GetWidth())
			{
				m_horizStateCount[currTile]++;
				// Add one to the count of "When current tile, this other to its right"
				horizData[currTile][tmap[row][col + 1]]++;
			}

			if (row + 1 != tmr->GetHeight())
			{
				m_vertiStateCount[currTile]++;
				// Add one to the count of "When current tile, this other below it"
				vertiData[currTile][tmap[row + 1][col]]++;
			}
		}
	}


	double sumH;
	double sumV;
	for (int row = 0; row < states; ++row)
	{
		sumH = 0.0;
		sumV = 0.0;
		for (int col = 0; col < states; ++col)
		{
			horizData[row][col] /= m_horizStateCount[row];
			sumH += horizData[row][col];
			vertiData[row][col] /= m_vertiStateCount[row];
			sumV += vertiData[row][col];
		}

		if (verbose)
		{
			// Checking probabilities add up
			std::cout << " " << sumH << "-" << sumV << " |";

			// According to this:
			// nº1491 never has anything below
			// nº1489 never has anything to its right
		}
	}


	//cv::Mat m_horizData(states + 1, states + 1, CV_64F);
	//cv::Mat m_vertiData(states + 1, states + 1, CV_64F);
	//cv::Mat dst;

	m_horizData = cv::Mat(states + 1, states + 1, CV_64F);
	m_vertiData = cv::Mat(states + 1, states + 1, CV_64F);
	cv::Mat dst;

	for (int i = 0; i < m_horizData.rows; ++i)
		for (int j = 0; j < m_horizData.cols; ++j)
			m_horizData.at<double>(i, j) = horizData.at(i).at(j);

	if (saveData)
	{
		cv::FileStorage file1("horizData.yml", cv::FileStorage::WRITE);
		file1 << "hMat" << m_horizData;
	}

	for (int i = 0; i < m_vertiData.rows; ++i)
		for (int j = 0; j < m_vertiData.cols; ++j)
			m_vertiData.at<double>(i, j) = vertiData.at(i).at(j);

	if (saveData)
	{
		cv::FileStorage file2("vertiData.yml", cv::FileStorage::WRITE);
		file2 << "vMat" << m_vertiData;
	}


	if (verbose)
	{
		cv::normalize(m_horizData, dst, 0, 1, cv::NORM_MINMAX);
		cv::imshow("Horizontal Probabilities", dst);
		//cv::imwrite("horizProb.png", hMat);
		cv::waitKey(0);

		cv::normalize(m_vertiData, dst, 0, 1, cv::NORM_MINMAX);
		cv::imshow("Vertical Probabilities", dst);
		//cv::imwrite("vertiProb.png", vMat);
		cv::waitKey(0);
	}

}




void Markov2D::ReadHorizontalData(std::string filename, bool verbose)
{

	std::cout << "Loading Rightward probability data" << std::endl;
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	fs["hMat"] >> m_horizData;

	if (verbose)
	{
		cv::Mat dst;
		cv::normalize(m_horizData, dst, 0, 1, cv::NORM_MINMAX);
		cv::imshow("Horizontal Probabilities", dst);
		cv::waitKey(0);
	}
}


void Markov2D::ReadVerticalData(std::string filename, bool verbose)
{

	std::cout << "Loading Downward probability data" << std::endl;
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	fs["vMat"] >> m_vertiData;

	if (verbose)
	{
		cv::Mat dst;
		cv::normalize(m_vertiData, dst, 0, 1, cv::NORM_MINMAX);
		cv::imshow("Vertical Probabilities", dst);
		cv::waitKey(0);
	}
}




void Markov2D::GenerateMap(int width, int height, bool killNullCells)
{
	std::cout << "Generating " << width << " by " << height << " Map" << std::endl;
	cv::Mat tileMap(height, width, CV_64F);
	cv::RNG rng(cv::getTickCount());
	tileMap.at<double>(0, 0) = (double)randomFrom(0, m_horizData.rows-1);

	/*float data[9] = { 1, 2, 3, 4, 5, 6, 7, 8, 10 };
	float deta[9] = { 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	cv::Mat a = cv::Mat(1, 9, CV_32F, data);
	cv::Mat c = cv::Mat(1, 9, CV_32F, deta);
	std::cout << a << std::endl;
	std::cout << c << std::endl;
	std::cout << a+c << std::endl;
	std::cout << (a+c)/2 << std::endl;*/


	std::default_random_engine generator;

	for (int row = 0; row < height; ++row)
	{
		for (int col = 0; col < width; ++col)
		{
			// average probability vectors and use them to sample the indices using an std::discrete_distribution
			// declare name of final prob vector
			cv::Mat totalProb(1, m_horizData.rows, CV_64F);
			if (row != 0 || col != 0)
			{
				if (row == 0)
				{
					// probVector is only left's
					totalProb = m_horizData.row((int)tileMap.at<double>(row, col - 1));
				}
				else if (col == 0)
				{
					// probVector is only up's
					totalProb = m_vertiData.row((int)tileMap.at<double>(row - 1, col));
				}
				else
				{
					// probVector is avg
					//cv::Mat ups		= m_vertiData.row((int)tileMap.at<double>(row - 1, col));
					//cv::Mat lefts	= m_horizData.row((int)tileMap.at<double>(row, col - 1));

					// works even though it seems like for ups it should be row-1 etc (maybe source data is flipped?)
					// looking back counter-intuitively because data is flipped
					cv::Mat ups		= m_vertiData.row((int)tileMap.at<double>(row, col - 1));
					cv::Mat lefts	= m_horizData.row((int)tileMap.at<double>(row - 1, col));
					totalProb = (ups + lefts) / 2.0;
				}
				std::vector<int> pVec;
				pVec.resize(m_horizData.rows);
				for (int i = 0; i < totalProb.cols; i++)
				{
					pVec[i] = (int)(100 * totalProb.at<double>(0, i));
				}

				if (killNullCells)
				{
					// Neutralize null tiles
					pVec[0] = 0;
				}


				std::discrete_distribution<int> distribution(pVec.begin(), pVec.end());
				tileMap.at<double>(row, col) = distribution(generator);
			}
		}
	}

	/*const int nrolls = 10000; // number of experiments
	const int nstars = 100;   // maximum number of stars to distribute

	std::default_random_engine generator;
	std::discrete_distribution<int> distribution{ 5, 20, 10, 40, 10, 10, 5 };

	int p[10] = {};

	for (int i = 0; i<nrolls; ++i) {
		int number = distribution(generator);
		++p[number];
	}

	std::cout << "a discrete_distribution:" << std::endl;
	for (int i = 0; i<10; ++i)
		std::cout << i << ": " << std::string(p[i] * nstars / nrolls, '*') << std::endl;*/

	cv::Mat img = MakeMapImage(tileMap);

	cv::imshow("image", img);
	cv::waitKey(0);
}



cv::Mat Markov2D::MakeMapImage(cv::Mat map)
{
	cv::Size s = map.size();
	cv::Mat img = cv::Mat::zeros(s.height * 16, s.width * 16, CV_8UC3);
	for (int row = 0; row < s.height; ++row)
	{
		for (int col = 0; col < s.width; ++col)
		{
			int h = map.at<double>(row, col);
			cv::Mat i = cv::imread("./Images/blocks/" + std::to_string((int)h) + ".png");
			i.copyTo(img(cv::Rect(row*16, col*16, i.cols, i.rows)));
		}
	}
	return img;
}