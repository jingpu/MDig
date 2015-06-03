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

bool box_compare(Rect a, Rect b) {
    if (a.x < b.x) return true;
    else return false;
}

void Segmentation::segment(Mat image, vector<Mat>& digitPatches, vector<vector<int> > &numberIndices) {
        Mat threshold_output;
        Mat rescale_output;
        int threshold_value;       
    
        clock_t beginTime;
        clock_t endTime;
        double elapsed_secs;
      
        beginTime = clock();
        //namedWindow("Contours", CV_WINDOW_AUTOSIZE);
        Segmentation::preprocess(image, rescale_output, threshold_output, threshold_value);
        endTime = clock();
        elapsed_secs = double(endTime - beginTime) / CLOCKS_PER_SEC;
        cout << "preprocess:" << elapsed_secs << endl;

        vector<Rect> boxes;
        beginTime = clock();
        Segmentation::bounding_box(threshold_output, boxes);
        Segmentation::merge_box(boxes);
        //for (int i = 0; i < boxes.size(); i++) 
       	//    cout << boxes[i] << endl;        
 
        /*for (int i = 0; i < boxes.size(); i++) {
            rectangle(rescale_output, boxes[i].tl(), boxes[i].br(), Scalar(100, 100, 100), 2, 8, 0);
        }
        //imwrite("results/contours.jpg", rescale_output);
        imshow("Contours", rescale_output);       
        waitKey(0);
        */
        endTime = clock();
        elapsed_secs = double(endTime - beginTime) / CLOCKS_PER_SEC;
        cout << "bbox: " << elapsed_secs << endl;

        beginTime = clock();
        Segmentation::get_digit(rescale_output, threshold_output, boxes, digitPatches, numberIndices);
        
        //cout << digits.size() << endl;
        //for (int i = 0; i < digits.size(); i++) {
        //    cout << i << endl;
        //    for (int j = 0; j < digits[i].size(); j++) { 
        //        imshow("Contours", digits[i][j]);
        //        waitKey(0);
        //    }
        //}
        Segmentation::pad_rescale(digitPatches, threshold_value);
        endTime = clock();
        elapsed_secs = double(endTime - beginTime) / CLOCKS_PER_SEC;
        cout << "get digit: " << elapsed_secs << endl;
}

void Segmentation::preprocess(Mat &image, Mat &rescale_output, Mat &output, int &threshold_value) {
        clock_t beginTime;
        clock_t endTime;
        double elapsed_secs;

        if (image.rows > 480 || image.cols > 640) {
            resize(image, rescale_output, Size(640, 480), 0,0, INTER_NEAREST);
        }
        else 
            image.copyTo(rescale_output);
        double minVal, maxVal;
        Point minLoc;
        Point maxLoc;
        /*cout << rescale_output << endl;
        imshow("flip image", rescale_output);
        waitKey(0);
        output = Scalar::all(255)- rescale_output;
        cout << output << endl;
        imshow("flip image", output);
        waitKey(0);
        minMaxLoc(rescale_output, &minVal, &maxVal, &minLoc, &maxLoc);
        cout << "min val: " << minVal << endl;
        cout << "max val: " << maxVal << endl;
        threshold(output, output, 120, 255, THRESH_BINARY);
        imshow("flip image", output);
        waitKey(0); */
        //cout << output << endl;
        Mat edges;
        beginTime = clock();
        blur(rescale_output, edges, Size(3,3));
       // imshow("blur image", edges);
       // waitKey(0);
        Canny(edges, edges, 13, 39, 3);
        endTime = clock();
        elapsed_secs = double(endTime - beginTime) / CLOCKS_PER_SEC;
        cout << "canny: " << elapsed_secs << endl;
        //imshow("edge image", edges);
        //waitKey(0);
        //output.create(Size(640,480), CV_8UC1); 
        //output = Scalar::all(0);
        //edges.copyTo(output);
        //imshow("flip image", output);
        //waitKey(0);
        Mat output_tmp = Scalar::all(255)- rescale_output;
        output.create(rescale_output.size(), CV_8UC1);
        output = Scalar::all(0);
        //adaptiveThreshold(output, output, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY,11, 0);
        vector<uchar> pixels;
        /*for(int i=0; i<output.rows; i++) {
           for(int j=0; j<output.cols; j++) {
         
           }  
        } */ 
        vector<Rect> boxes;
        Segmentation:bounding_box(edges, boxes);
        /*for (int i=0; i < boxes.size(); i++) {
            rectangle(edges, boxes[i].tl(), boxes[i].br(), Scalar(100, 100, 100), 2, 8, 0);
        }
        imshow("bbox edges", edges);
        waitKey(0);*/
        beginTime = clock();
        // reduce image size
        int top=boxes[0].y;
        int bottom=boxes[0].y+boxes[0].height;
        int left=boxes[0].x;
        int right=boxes[0].x+boxes[0].width;

        for (int k=1; k<boxes.size(); k++) {
           if(boxes[k].y < top)
              top = boxes[k].y;
           if(boxes[k].y+boxes[k].height > bottom)
              bottom = boxes[k].y+boxes[k].height; 
           if(boxes[k].x < left)
              left = boxes[k].x;
           if(boxes[k].x+boxes[k].width > right)
              right = boxes[k].x+boxes[k].width;
        }
        
        //imshow("rect",rescale_output(Rect(left, top, right-left, bottom-top)));
        //waitKey(0);
        //rescale_output = tmp;
        //tmp.copyTo(output);
        
        for(int i=top; i<=bottom; i++) {
           for(int j=left; j<=right; j++) {
              bool in_box = false;
              for(int k=0; k<boxes.size(); k++) {
                 if (i>=boxes[k].y && i<=boxes[k].y+boxes[k].height 
                   && j>=boxes[k].x && j<=boxes[k].x+boxes[k].width) {
                    in_box = true;
                    break;
                 }
              }
              if (in_box) {
                 pixels.push_back(output_tmp.at<uchar>(i,j));
                 /*if (output.at<uchar>(i,j) < white_threshold)
                    output.at<uchar>(i,j) = 0;
                 else
                    output.at<uchar>(i,j)=255; */
              }
              //else 
             //       output.at<uchar>(i,j)=0;
           } 
        }
        double sum = accumulate(pixels.begin(), pixels.end(),0.0);
        double mean = sum/pixels.size();
        double sq_sum = inner_product(pixels.begin(),pixels.end(), pixels.begin(), 0.0);
        double stdev = sqrt(sq_sum/pixels.size() - mean*mean);
        cout << mean << "," << stdev << endl; 
        threshold_value = mean+stdev/2;      
  
        for(int k=0; k<boxes.size(); k++) {
           threshold(output_tmp(boxes[k]), output_tmp(boxes[k]), threshold_value, 255, THRESH_BINARY);   
           output_tmp(boxes[k]).copyTo(output(boxes[k])); 
           /*for (int i=boxes[k].y; i<=boxes[k].y+boxes[k].height; i++ ) {
               for (int j=boxes[k].x; j<=boxes[k].x+boxes[k].width; j++) {
                   if (output.at<uchar>(i,j) < threshold_value) {
                      output.at<uchar>(i,j) = 0;
                   }else 
                      output.at<uchar>(i,j) =255;
               }
           }*/
        }
        endTime = clock();
        elapsed_secs = double(endTime - beginTime) / CLOCKS_PER_SEC;
        cout << "scan image: " << elapsed_secs << endl;
        //imshow("threshold image", output);
        //waitKey(0); 
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
    
    /*namedWindow("Contours", CV_WINDOW_AUTOSIZE);
    RNG rng(123);
    for (int i = 0; i < contours.size(); i++) {
        if (boundRect[i].width * boundRect[i].height < 100) continue;
        //cout << boundRect[i] << endl;
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255) );
        drawContours(image, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
        rectangle(image, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
    }
        
    imshow("Contours", image);
    waitKey(0);
 */
}

void Segmentation::merge_box(vector<Rect> &boxes) {
    int i = 0;
    while (i < boxes.size()) {
        int j = i + 1;
        while (j < boxes.size()) {
            //cout << boxes[i] << "  " << boxes[j] << endl;
            double height_diff = abs((boxes[i].height - boxes[j].height) / (double)boxes[i].height);
            double hpos_diff = abs((boxes[i].y - boxes[j].y) / (double)boxes[i].height);
            int w_diff = boxes[i].x < boxes[j].x ? boxes[j].x - (boxes[i].x + boxes[i].width) : boxes[i].x - (boxes[j].x + boxes[j].width);
            double wpos_diff = abs((double)w_diff / (boxes[i].width + boxes[j].width) * 2);
            //cout << boxes[i].x << " " << boxes[j].x << " " << height_diff << "  " << hpos_diff << "  " << w_diff << "  " << wpos_diff << endl;
            if (height_diff < 0.5 && hpos_diff < 0.5 && wpos_diff < 1.5) {
              int x = boxes[i].x < boxes[j].x ? boxes[i].x : boxes[j].x;
              int width = (boxes[i].x + boxes[i].width > boxes[j].x + boxes[j].width ? boxes[i].x + boxes[i].width : boxes[j].x + boxes[j].width) - x;
              int y = boxes[i].y < boxes[j].y ? boxes[i].y : boxes[j].y;
              int height = (boxes[i].y + boxes[i].height > boxes[j].y + boxes[j].height ? boxes[i].y + boxes[i].height : boxes[j].y + boxes[j].height) - y;
              Rect merge(x, y, width, height);
              boxes[i] = merge;
              boxes.erase(boxes.begin() + j);
              j = i + 1;
            } else {
              j++;
            }
        }
        i++;
    }  

    sort(boxes.begin(), boxes.end(), box_compare);            
}

void Segmentation::get_digit(const Mat &image, const Mat &thresh_output, const vector<Rect> &boxes, vector<Mat>& digitPatches, vector<vector<int> > &numberIndices) {
   for (int i = 0; i < boxes.size(); i++) {
       vector<int> digitIndices;
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
		  digitIndices.push_back(digitPatches.size()); // Trick here: digitPatches.size() is the index of the next appended element
		  digitPatches.push_back(new_digit);
               } else {
                  prev_in_digit = true;
               }
               start = end + 1;
               continue;
           }
       }
       if (prev_in_digit) {
           if (in_digit) {
               end = boxes[i].x + boxes[i].width;
           } else {
               end = (begin_segment + boxes[i].x + boxes[i].width) / 2;
           }
           //cout << start << ' ' << boxes[i].y << ' ' << end << ' ' << boxes[i].height << endl;
           Rect box(start, boxes[i].y, end - start, boxes[i].height);
           Mat new_digit;
           image(box).copyTo(new_digit);
	   digitIndices.push_back(digitPatches.size()); // Trick here: digitPatches.size() is the index of the next appended element
	   digitPatches.push_back(new_digit);
       }
       numberIndices.push_back(digitIndices);
   }    
}

bool Segmentation::check_all_zeros(const Mat &image, int row_start, int row_end, int col) {
   for (int i = row_start; i <= row_end; i++) {
       if (image.at<uint8_t>(i, col) != 0) return false;
   }
   return true;
}


void Segmentation::pad_rescale(vector<Mat>  &patches, int &threshold_value) {
  for (auto&& im : patches) {
    int top, bottom, left, right;
    im = Scalar::all(255)- im;
    threshold(im, im, threshold_value, 255, THRESH_BINARY);
    int num_row = im.rows;
    int num_col = im.cols;
    if (num_row >20 || num_col > 20) {
      resize(im, im, Size(20, 20), 0,0, INTER_NEAREST);
    }
    top = 0.5*(28-im.rows);
    bottom = 28-im.rows-top;
    right = 0.5*(28-im.cols);
    left = 28-im.cols-right;
    copyMakeBorder(im,im, top, bottom, left, right, BORDER_CONSTANT);
    //imwrite("results/patch"+to_string(i)+to_string(j)+".jpg", im);
    //imshow("Contours", im);
    //waitKey(0);
  }
}
 /*
void Segmentation::pad_rescale(vector<vector<Mat> > &digits, int &threshold_value) {
   int top, bottom, left, right;
   //namedWindow("digit", CV_WINDOW_AUTOSIZE);
   for (int i=0; i<digits.size(); i++){
       for (int j=0; j<digits[i].size(); j++) {
           digits[i][j] = Scalar::all(255)- digits[i][j];
           threshold(digits[i][j], digits[i][j], threshold_value, 255, THRESH_BINARY);
           int num_row = digits[i][j].rows;
           int num_col = digits[i][j].cols;
           cout << num_row << "," << num_col << endl;
           if (num_row >20 || num_col > 20) {
              resize(digits[i][j], digits[i][j], Size(20.0, 20), 0,0, INTER_NEAREST);
           }
           top = 0.5*(28-digits[i][j].rows);
           bottom = 28-digits[i][j].rows-top;
           right = 0.5*(28-digits[i][j].cols);
           left = 28-digits[i][j].cols-right;
           copyMakeBorder(digits[i][j],digits[i][j], top, bottom, left, right, BORDER_CONSTANT);
           imwrite("results/6patch"+to_string(i)+to_string(j)+".jpg", digits[i][j]);
           imshow("Contours", digits[i][j]);
           waitKey(0);
           
       }
   } 
  
} */





