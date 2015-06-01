#ifndef MD_Common_hpp
#define MD_Common_hpp

#include <opencv2/opencv.hpp>
#include <cassert>
#include <functional>
#include <memory>

#include "NativeLogging.hpp"


typedef cv::Matx33f Homography;

typedef cv::Mat ColorImage;
    
typedef cv::Mat GrayscaleImage;
    
typedef std::vector<cv::Point2f> PointArray;
    
typedef std::function<void(const PointArray&, const PointArray&)> MatchHandler;

#endif
