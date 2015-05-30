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
    
    ~ConvNetInput()
    {
        jpcnn_destroy_image_buffer(data);
    }
};
    
ConvNet::ConvNet(const std::string& network_path)
{
    net_ = jpcnn_create_network(network_path.c_str());
    assert(net_!=nullptr);
}
    
ConvNet::~ConvNet()
{
    jpcnn_destroy_network(net_);
}
    
ConvNetFeatures ConvNet::extract_features(const GrayscaleImage& image)
{
    ConvNetFeatures features = {.features = nullptr, .length = 0};
    ConvNetInput input(image);
    char** predictionsLabels;
    int predictionsLabelsLength;
    jpcnn_classify_image(net_,
                         input.data,
                         JPCNN_SKIP_RESCALE,
                         0,
                         &(features.features),
                         &(features.length),
                         &predictionsLabels,
                         &predictionsLabelsLength);
    return features;
}
  
