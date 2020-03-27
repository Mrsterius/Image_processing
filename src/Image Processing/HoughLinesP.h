#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

Mat src_gray;
Mat edges;
int detectLinesProbabilistic( Mat src )
{
    // Read original image
    //Mat src = imread("/Users/himanshuhansaria/Downloads/medianblurred.png");
    
    //if fail to read the image
    if (!src.data)
    {
        cout << "Error loading the image" << endl;
        return -1;
    }
    
//    cvtColor( src, src_gray, COLOR_RGB2GRAY );
    
    /// Apply Canny edge detector
    //Canny( src_gray, edges, 50, 200, 3 );
    threshold(src, edges, 120, 255, THRESH_BINARY);
    
    
    
    // Create a window
    namedWindow("My Window", CV_WINDOW_NORMAL);
    
    //Create trackbar to change Min No. Of Points to detect a line
    int iSliderValue1 = 50;
    createTrackbar("Min No.of Points", "My Window", &iSliderValue1, 100);
    
    //Create trackbar to change Max Line Length
    int iSliderValue2 = 50;
    createTrackbar("Max Allowed Gap", "My Window", &iSliderValue2, 100);
    
    while (true)
    {
        
        Mat dst;
        int min_threshold = 0;
        int min_threshold_MaxLineGap=0;
        int iMinNoOfPoints  = min_threshold + iSliderValue1;
        double dMaxLineGap = min_threshold_MaxLineGap+ iSliderValue2;
        
        vector<Vec4i> p_lines;
        cvtColor( edges, dst, COLOR_GRAY2BGR );
        
        /// 2. Use Probabilistic Hough Transform
        HoughLinesP( edges, p_lines, 1, CV_PI/180,iMinNoOfPoints , 10,dMaxLineGap);
        
        /// Show the result
        for( size_t i = 0; i < p_lines.size(); i++ )
        {
            Vec4i l = p_lines[i];
            line( dst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,0,0), 3,__LINE__ );
        }
        
        
        imshow("My Window", dst);
        imwrite("/Users/himanshuhansaria/Downloads/Harmony Residency/hr1Detected_Lines.png",dst);
        
        // Wait until user press some key for 50ms
        int iKey = waitKey(50);
        
        //if user press 'ESC' key
        if (iKey == 27)
        {
            break;
        }
    }
    
    return 0;
}