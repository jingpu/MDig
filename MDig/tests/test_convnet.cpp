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
  Mat colorImage = imread("res/combine_img.JPEG", CV_LOAD_IMAGE_COLOR);
  Mat grayscaleImage;
  cv::cvtColor(colorImage, grayscaleImage, CV_RGB2GRAY);
   

  Segmentation seg;
  vector<vector<Mat> > digits;
  seg.segment(grayscaleImage, digits);
  vector<int> numbers;
 
  //Construct convolutional neural network.
  ConvNet net(PathForResource("mdnet.ntwk"));
  

  bool useBatch = true;

  for (int m=0; m<digits.size(); m++ ) {
    vector<char> d = net.extract_digits(digits[m], useBatch);
    cout << "number = ";
    for (char c : d) {
      cout << c;
    }
    cout << endl;

  }
}
