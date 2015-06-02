#ifndef __MD__ConvNet__
#define __MD__ConvNet__

#include "Common.hpp"
#include <vector>
    
struct ConvNetFeatures
{
  float* features;
  int length;
  char** labels;
  int labelsLength;
};


// Our Convolutional Neural Network is powered by
// DeepBeliefSDK from Jetpac (recently acquired by Google).
// More info: https://github.com/jetpacapp/DeepBeliefSDK
class ConvNet
{
public:
    
  ConvNet(const std::string& network_path, float threshold = 0.0);
    
  virtual ~ConvNet();
    
  std::vector<char> extract_digits(const std::vector<GrayscaleImage>& images, bool useBatch);

  ConvNet(const ConvNet& other) = delete;
    
  ConvNet& operator=( const ConvNet&) = delete;
    
private:
    
  void* net_;
  float threshold_;

  ConvNetFeatures extract_features(const GrayscaleImage& image);
  ConvNetFeatures extract_batched_features(const std::vector<GrayscaleImage>& images);

  std::vector<char> get_digits(const ConvNetFeatures&);
  char get_digit(const ConvNetFeatures&);
};

#endif 
