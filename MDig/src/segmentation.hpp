#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

class Segmentation {
    public:
        void segment(Mat image, vector<vector<Mat> > &digits);
        void bounding_box(Mat &image, vector<Rect> &boxes);
        void get_digit(Mat &image, Mat &thresh_output, vector<Rect> &boxes, vector<vector<Mat> > &digits);    
        void pad_rescale(vector<vector<Mat> > &digits);    
        bool check_all_zeros(Mat &image, int row_start, int row_end, int col);
};

