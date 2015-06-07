#include "MDApp.hpp"

#include "ResourceLocator.hpp"
#include <time.h>
  
static const char* TAG = "MDApp";

// from android samples
/* return current time in milliseconds */
static inline double now_ms(void) {

    struct timespec res;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &res);
    //clock_gettime(CLOCK_THREAD_CPUTIME_ID, &res);
    //clock_gettime(CLOCK_MONOTONIC, &res);
    return 1000.0 * res.tv_sec + (double) res.tv_nsec / 1e6;

}


MDApp::MDApp() : net_(PathForResource("mdnet.ntwk"))
{
    has_been_initialized_ = true;
    useBatch_ = false;
}
    
void MDApp::initialize()
{   
	assert(0);
    has_been_initialized_ = true;
}

std::vector<std::string > MDApp::process_frame(ColorImage& frame)
{
  double beginTime;
  double endTime;
  double elapsed_ms;

  assert(has_been_initialized_ && "Must be initialized before calling process_frame!");
  cv::Mat grayscaleImage;
  convert_to_grayscale(frame, grayscaleImage);
   
  std::vector<cv::Mat> digitPatches;
  std::vector<std::vector<int> > numberIndices;
  seg_.segment(grayscaleImage, digitPatches, numberIndices);


  beginTime = now_ms();
  std::vector<char> digits;
  for (int i = 0; i < 1; ++i)
    digits = net_.extract_digits(digitPatches, useBatch_);
  endTime = now_ms();
  elapsed_ms = double(endTime - beginTime)/1;
  LOG_DEBUG(TAG, "recognition %.1f msec.", elapsed_ms);

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
	assert(colorImage.type()==CV_8UC3 && "Frame type must be RGB!");
    cv::cvtColor(colorImage, grayscaleImage, CV_RGB2GRAY);
}
