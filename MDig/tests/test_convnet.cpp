#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>

#include "ConvNet.hpp"
#include "ResourceLocator.hpp"

using namespace std;
using namespace cv;


int main(int argc, char** argv)
{
  Mat colorImage = imread("res/1.JPEG", CV_LOAD_IMAGE_COLOR);
  Mat grayscaleImage;
  cv::cvtColor(colorImage, grayscaleImage, CV_RGB2GRAY);

  //Construct convolutional neural network.
  ConvNet net(PathForResource("mdnet.ntwk"));

  ConvNetFeatures feature = net.extract_features(grayscaleImage);

  int max_idx = -1;
  float max_prob = 0;
  for(int i = 0; i < feature.length; ++i) {
    if(feature.features[i] > max_prob) {
      max_prob = feature.features[i];
      max_idx = i;
    }
  }
  cout << max_idx << '(' << max_prob << ')' << endl;

}
