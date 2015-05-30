#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "segmentation.hpp"

using namespace std;
using namespace cv;


int main(int argc, char** argv)
{
   Mat im = imread("res/combine_img.JPEG");
   Mat src_gray;
   cvtColor(im, src_gray, CV_BGR2GRAY);
   imshow("input", src_gray);
   waitKey(0);
   Segmentation seg;
   seg.segment(src_gray);



	/*** Part 1: Image Classification ***/
//	ar::Classifier classifier;
//
//	vector<Mat> pics;
//	pics.push_back(imread("art-pictures/convergence.jpg"));
//	pics.push_back(imread("art-pictures/concetto-spaziale.jpg"));
//	pics.push_back(imread("art-pictures/mona-lisa.jpg"));
//	pics.push_back(imread("art-pictures/nighthawks.jpg"));
//	pics.push_back(imread("art-pictures/the-scream.jpg"));
//	for (const auto& im : pics) {
//		int label = classifier.classify(im);
//		ArtInfo info = classifier.get_art_info(label);
//		cout << info.title << " by " << info.artist << endl;
//	}


  cv::waitKey(0);

}
