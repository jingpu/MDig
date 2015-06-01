#include "MDApp.hpp"

#include "ResourceLocator.hpp"
  
MDApp::MDApp() : net_(PathForResource("mdnet.ntwk"))
{
    has_been_initialized_ = false;
    useBatch_ = true;
}
    
void MDApp::initialize()
{   
    has_been_initialized_ = true;
}

std::vector<std::string > MDApp::process_frame(ColorImage& frame)
{
  assert(has_been_initialized_ && "Must be initialized before calling process_frame!");
  assert(frame.type()==CV_8UC3 && "Frame type must be RGB!");

  cv::Mat grayscaleImage;
  convert_to_grayscale(frame, grayscaleImage);
   
  std::vector<std::vector<Mat> > numberImages;
  seg_.segment(grayscaleImage, numberImages);

  std::vector<std::string > result;
  for (const auto& digitImages : numberImages) {
    std::vector<char> digits = net_.extract_digits(digitImages, useBatch_);
    std::string s(digits.begin(), digits.end()); // convert char vector to string
    result.push_back(s);
  }

  return result;
}

void MDApp::convert_to_grayscale(const ColorImage& colorImage, GrayscaleImage& grayscaleImage)
{
    cv::cvtColor(colorImage, grayscaleImage, CV_RGB2GRAY);
}
