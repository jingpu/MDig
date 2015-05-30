#include "segmentation.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>

using namespace std;
using namespace cv;

void Segmentation::segment(Mat image, vector<vector<Mat> > &digits) {
        Mat threshold_output;

        //namedWindow("Contours", CV_WINDOW_AUTOSIZE);
        threshold(image, threshold_output, 100, 255, THRESH_BINARY);

        vector<Rect> boxes;
        Segmentation::bounding_box(threshold_output, boxes);
        
        //for (int i = 0; i < boxes.size(); i++) 
       	//    cout << boxes[i] << endl;        
 
        //for (int i = 0; i < boxes.size(); i++) {
        //    rectangle(image, boxes[i].tl(), boxes[i].br(), Scalar(100, 100, 100), 2, 8, 0);
        //}
        //imshow("Contours", image);       
        //waitKey(0);

        Segmentation::get_digit(image, threshold_output, boxes, digits);
        
        //cout << digits.size() << endl;
        //for (int i = 0; i < digits.size(); i++) {
        //    cout << i << endl;
        //    for (int j = 0; j < digits[i].size(); j++) { 
        //        imshow("Contours", digits[i][j]);
        //        waitKey(0);
        //    }
        //}  
}

void Segmentation::bounding_box(Mat &image, vector<Rect> &boxes) {
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    
    findContours(image, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    vector<vector<Point> > contours_poly(contours.size() );
    vector<Rect> boundRect(contours.size());
    for (int i = 0; i < contours.size(); i++) {
        approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true);
        boundRect[i] = boundingRect( Mat(contours_poly[i]) );
    }

    for (int i = 0; i < boundRect.size(); i++) {
        if (boundRect[i].width * boundRect[i].height > 100) {
            boxes.push_back(boundRect[i]);
        }
    } 


    int i = 0;
    while (i < boxes.size()) {
        int j = i + 1;
        while (j < boxes.size()) {
            //cout << boxes[i] << "  " << boxes[j] << endl;
            double height_diff = abs(boxes[i].height - boxes[j].height);
            double hpos_diff = abs((boxes[i].y - boxes[j].y) / (double)boxes[i].height);
            int w_diff = boxes[i].x < boxes[j].x ? boxes[j].x - (boxes[i].x + boxes[i].width) : boxes[i].x - (boxes[j].x + boxes[j].width);
            double wpos_diff = abs((double)w_diff / (boxes[i].width + boxes[j].width) * 2);
            //cout << height_diff << "  " << hpos_diff << "  " << w_diff << "  " << wpos_diff << endl;
            if (height_diff < 2 && hpos_diff < 0.1 && wpos_diff < 1) {
              int x = boxes[i].x < boxes[j].x ? boxes[i].x : boxes[j].x;
              int width = (boxes[i].x + boxes[i].width > boxes[j].x + boxes[j].width ? boxes[i].x + boxes[i].width : boxes[j].x + boxes[j].width) - x;
              Rect merge(x, boxes[i].y, width, boxes[i].height);
              boxes[i] = merge;
              boxes.erase(boxes.begin() + j);
            } else {
              j++;
            }
        }
        i++;
    }              
}

void Segmentation::get_digit(Mat &image, Mat &thresh_output, vector<Rect> &boxes, vector<vector<Mat> > &digits) {
   for (int i = 0; i < boxes.size(); i++) {
       vector<Mat> box_digits;
       bool in_digit = false, prev_in_digit = false;
       int begin_segment = boxes[i].x, start, end;
       for (int j = boxes[i].x; j <= boxes[i].x + boxes[i].width; j++) {
           bool part = Segmentation::check_all_zeros(thresh_output, boxes[i].y, boxes[i].y + boxes[i].height, j);
           //cout << i << ' ' << j << ' ' << part << endl;
           if (in_digit && part) {
               begin_segment = j;
               in_digit = false;
               continue;
           }
           if (!in_digit && !part) {
               in_digit = true;
               end = (begin_segment + j - 1) / 2;
               if (prev_in_digit) {
               	  //cout << start << ' ' << boxes[i].y << ' ' << end  << ' ' << boxes[i].height << endl;
                  Rect box(start, boxes[i].y, end - start, boxes[i].height);
                  Mat new_digit;
                  image(box).copyTo(new_digit);
                  box_digits.push_back(new_digit);
               } else {
                  prev_in_digit = true;
               }
               start = end + 1;
               continue;
           }
       }
       if (prev_in_digit) {
           end = (begin_segment + boxes[i].x + boxes[i].width) / 2;
           //cout << start << ' ' << boxes[i].y << ' ' << end << ' ' << boxes[i].height << endl;
           Rect box(start, boxes[i].y, end - start, boxes[i].height);
           Mat new_digit;
           image(box).copyTo(new_digit);
           box_digits.push_back(new_digit);
       }
       digits.push_back(box_digits);
   }    
}

bool Segmentation::check_all_zeros(Mat &image, int row_start, int row_end, int col) {
   for (int i = row_start; i <= row_end; i++) {
       if (image.at<uint8_t>(i, col) != 0) return false;
   }
   return true;
}





