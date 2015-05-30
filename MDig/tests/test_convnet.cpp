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
  
  for (int m=0; m<digits.size(); m++ ) {
      int curr_num = 0;
      for (int n=0; n<digits[m].size(); n++) {
          //ConvNetFeatures feature = net.extract_features(grayscaleImage);
          cv::imshow("digit", digits[m][n]);
          cv::waitKey(0);
          ConvNetFeatures feature = net.extract_features(digits[m][n]);

          int max_idx = -1;
          float max_prob = 0;
          for(int i = 0; i < feature.length; ++i) {
              if(feature.features[i] > max_prob) {
                  max_prob = feature.features[i];
                  max_idx = i;
              }
          }
          cout << "=============" << m << ',' << n << "==============" << endl; 
          cout << max_idx << '(' << max_prob << ')' << endl;
          curr_num *= 10;
          curr_num += max_idx;
      }
     cout << "number = " << curr_num << endl;
     numbers.push_back(curr_num);
  }
}
