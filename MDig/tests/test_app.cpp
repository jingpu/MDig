#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>

#include "MDApp.hpp"

using namespace std;
using namespace cv;


int main(int argc, char** argv)
{
  Mat frame = imread("res/real_img.jpg", CV_LOAD_IMAGE_COLOR);
  
  MDApp md;
  
  md.initialize();

  vector<string > numbers = md.process_frame(frame);

  cout << "found " << numbers.size() << " number" << endl;
  for(const auto& s : numbers)
    cout << s << endl;
  return 0;
}
