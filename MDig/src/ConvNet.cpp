#include "ConvNet.hpp"
#include "libjpcnn.h"


class ConvNetInput
{
public:
    
  void* data;
    
  ConvNetInput(const GrayscaleImage& img)
  {
    assert((img.type()==CV_8UC1));
    const cv::Size size = img.size();
    const int width = size.width;
    const int height = size.height;
    uint8_t* pixelData = (uint8_t*)img.data;
    this->data = jpcnn_create_image_buffer_from_uint8_data(pixelData, width, height, 1, (1 * width), 0, 0);
  }

  ConvNetInput(const std::vector<GrayscaleImage>& images)
  {
    assert(images.size() > 0);
    const int numOfImages = images.size();
    uint8_t** pixelData = new uint8_t*[numOfImages];

    const cv::Size size = images[0].size();
    const int width = size.width;
    const int height = size.height;

    for (int i = 0; i < images.size(); ++i){
      assert(images[i].type() == CV_8UC1);
      assert(images[i].size() == size);
      pixelData[i] = (uint8_t *)images[i].data;
    }
    this->data = jpcnn_create_batched_image_buffer_from_uint8_data(pixelData, numOfImages, width, height, 1);

    delete[] pixelData;
  }
    
  ~ConvNetInput()
  {
    jpcnn_destroy_image_buffer(data);
  }
};

ConvNet::ConvNet(const std::string& network_path, float threshold)
{
  threshold_ = threshold;
  net_ = jpcnn_create_network(network_path.c_str());
  assert(net_!=nullptr);
}
    
ConvNet::~ConvNet()
{
  jpcnn_destroy_network(net_);
}



ConvNetFeatures ConvNet::extract_features(const GrayscaleImage& image)
{
  ConvNetFeatures features = {.features = nullptr, .length = 0, .labels = nullptr, .labelsLength = 0};
  ConvNetInput input(image);
  jpcnn_classify_image(net_,
		       input.data,
		       JPCNN_SKIP_RESCALE,
		       0,
		       &(features.features),
		       &(features.length),
		       &(features.labels),
		       &(features.labelsLength));
  return features;
}

ConvNetFeatures ConvNet::extract_batched_features(const std::vector<GrayscaleImage>& images)
{
  ConvNetFeatures features = {.features = nullptr, .length = 0, .labels = nullptr, .labelsLength = 0};
  ConvNetInput input(images);
  jpcnn_classify_image(net_,
		       input.data,
		       JPCNN_SKIP_RESCALE,
		       0,
		       &(features.features),
		       &(features.length),
		       &(features.labels),
		       &(features.labelsLength));
  return features;
}


char 
ConvNet::get_digit(const ConvNetFeatures& feature){
  assert(feature.length == feature.labelsLength);
  char result;

  int max_idx = -1;
  float max_prob = 0;
  for(int i = 0; i < feature.length; ++i) {
    if(feature.features[i] > max_prob) {
      max_prob = feature.features[i];
      max_idx = i;
    }
  }
  
  if(max_idx != -1 && max_prob > threshold_)
    result = (feature.labels[max_idx])[0]; // first char of the label
  else
    result = 0;

  return result;
}


std::vector<char> 
ConvNet::get_digits(const ConvNetFeatures& feature){
  assert(feature.length % feature.labelsLength == 0);
  std::vector<char> result;
  
  const int numOfDigits = feature.length / feature.labelsLength;
  for (int i = 0; i < numOfDigits; ++i) {
    char c = 0;
    
    int max_idx = -1;
    float max_prob = 0;
    for(int j = 0; j < feature.labelsLength; ++j) {
      float prob = feature.features[j + i*feature.labelsLength];
      if(prob > max_prob) {
	max_prob = prob;
	max_idx = j;
      }
    }
  
    if(max_idx != -1 && max_prob > threshold_)
      c = (feature.labels[max_idx])[0]; // first char of the label

    result.push_back(c);
  }

  return result;
}

std::vector<char> 
ConvNet::extract_digits(const std::vector<GrayscaleImage>& images, bool useBatch){
  std::vector<char> result;

  if (!useBatch) {
    for(const auto im : images) {
      ConvNetFeatures features = extract_features(im);
      result.push_back(get_digit(features));
    }
  } else {
    ConvNetFeatures features = extract_batched_features(images);   
    result = get_digits(features);
  }
  return result;
}
