

#include "TileMapReader.h"
#include "Markov2D.h"



int main()
{

	//Mat img = imread("./Images/blocks/1.png");
	//namedWindow("image", WINDOW_NORMAL);
	//imshow("image", img);

	TileMapReader* tmr = new TileMapReader();
	Markov2D* m2d = new Markov2D();

	if (!tmr->ReadFile("./Images/tileMap.json"))
	{ std::cout << "Read File failed" << std::endl; }
	m2d->Analyse(tmr, false);
	

	//m2d->ReadHorizontalData("horizData.yml");
	//m2d->ReadVerticalData("vertiData.yml");
	m2d->GenerateMap(64, 64);

	//cv::Mat img = cv::imread("./Images/blocks/1.png");
	////namedWindow("image", WINDOW_NORMAL);
	//cv::imshow("image", img);
	//cv::waitKey(0);
	//return 0;
}