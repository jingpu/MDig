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
   
  std::vector<cv::Mat> digitPatches;
  std::vector<std::vector<int> > numberIndices;

  seg_.segment(grayscaleImage, digitPatches, numberIndices);
  std::vector<char> digits = net_.extract_digits(digitPatches, useBatch_);

  std::vector<std::string > result;
  // back tracing the indices of digits to form the number strings
  for(const auto& digitIndices : numberIndices) {
    size_t stringLen = digitIndices.size();
    std::string s(stringLen, 'x');
    for(int i = 0; i < stringLen; ++i)
      s[i] = digits[digitIndices[i]];
    result.push_back(s);
  }

  return result;
}

void MDApp::convert_to_grayscale(const ColorImage& colorImage, GrayscaleImage& grayscaleImage)
{
  cv::cvtColor(colorImage, grayscaleImage, CV_RGB2GRAY);
}
