#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>

#include "ConvNet.hpp"
#include "segmentation.hpp"
#include "ResourceLocator.hpp"

using namespace std;
using namespace cv;


int main(int argc, char** argv)
{
  Mat colorImage = imread("res/real_img5.jpg", CV_LOAD_IMAGE_COLOR);
  Mat grayscaleImage;
  cv::cvtColor(colorImage, grayscaleImage, CV_RGB2GRAY);
   

  Segmentation seg;
  vector<vector<Mat> > digitImages;
  seg.segment(grayscaleImage, digitImages);
  vector<int> numbers;
 
  //Construct convolutional neural network.
  ConvNet net(PathForResource("mdnet.ntwk"));

  for (int b = 0; b < 2; ++b) {
    for (int m=0; m<digitImages.size(); m++ ) {
      bool useBatch = (b == 1);
      cout << "useBatch = " << b << endl;
      vector<Mat> v;
      v.insert(v.end(), digitImages[m].begin(), digitImages[m].end());
      vector<char> digits = net.extract_digits(v, useBatch);
      cout << "number = ";
      for (char c : digits) {
	cout << c;
      }
      cout << endl;
    }
  }
}
