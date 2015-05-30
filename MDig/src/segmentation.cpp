#include "segmentation.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

void Segmentation::segment(Mat image) {
        Mat threshold_output;
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;

        namedWindow("Contours", CV_WINDOW_AUTOSIZE);
        threshold(image, threshold_output, 100, 255, THRESH_BINARY);
        imshow("Contours", threshold_output);
        waitKey(0);

        findContours(image, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

        cout << contours.size()  << endl;

        vector<vector<Point> > contours_poly(contours.size() );
        vector<Rect> boundRect(contours.size());
        for (int i = 0; i < contours.size(); i++) {
            approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true);
            boundRect[i] = boundingRect( Mat(contours_poly[i]) );
        }


        RNG rng(123);
        for (int i = 0; i < contours.size(); i++) {
            if (boundRect[i].width * boundRect[i].height < 100) continue;
            cout << boundRect[i] << endl;
            Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255) );
            drawContours(image, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
            rectangle(image, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
        }
        
        imshow("Contours", image);
}





