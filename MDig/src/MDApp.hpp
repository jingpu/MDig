#ifndef __MD__MDApp__
#define __MD__MDApp__

#include "Common.hpp"
#include "ConvNet.hpp"
#include "segmentation.hpp"

#include <vector>
#include <string>

class MDApp
{
public:
    
  MDApp();
    
  virtual ~MDApp() = default;
    
  void initialize();
    
  // The main entry point into our augmented reality sub-system.
  // Note that this function mutates the input frame with the augmentations.
  // Expected input: An 8UC3 RGB image.
  std::vector<std::string > process_frame(ColorImage& frame);
    
private:
    
  bool has_been_initialized_;

  bool useBatch_;
    
  Segmentation seg_;

  ConvNet net_;
    
  void convert_to_grayscale(const ColorImage& colorImage, GrayscaleImage& grayscaleImage);
};

#endif
