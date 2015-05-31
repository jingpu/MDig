#ifndef __MD__ConvNet__
#define __MD__ConvNet__

#include "Common.hpp"
#include <vector>

// Our Convolutional Neural Network is powered by
// DeepBeliefSDK from Jetpac (recently acquired by Google).
// More info: https://github.com/jetpacapp/DeepBeliefSDK
class ConvNet
{
public:
    
  ConvNet(const std::string& network_path);
    
  virtual ~ConvNet();
    
  ConvNetFeatures extract_features(const GrayscaleImage& image);
  ConvNetFeatures extract_batched_features(const std::vector<GrayscaleImage>& images);
    
  ConvNet(const ConvNet& other) = delete;
    
  ConvNet& operator=( const ConvNet&) = delete;
    
private:
    
  void* net_;
};

#endif 
