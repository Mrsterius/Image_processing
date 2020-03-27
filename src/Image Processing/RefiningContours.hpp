//
//  RefiningContours.hpp
//  ConstructWall
//
//  :author : Himanshu  Hansaria on 11/07/14.
//  Copyright (c) 2014 Himanshu  Hansaria. All rights reserved.
//

#ifndef ConstructWall_Header_h
#define ConstructWall_Header_h

//Macros
#define NOISE 1
#define NON_NOISE 0

//Global Variables
int threshold_seq = 20;                 //Width of sequence after which it is Treated as WALL
int threshold_tol = 1;                  //Tolerance of bumps, where bump= change in sequence
int length = 5;

//Function Prototypes
vector<int> get_actual_difference(vector<Point> contour);
vector<int> get_difference(vector<int> v);
vector<int> difference_of_difference(vector<Point> contour);
struct sequence rectify_end_points(vector<Point> tmp , struct sequence noise_end);
struct sequence run_get_noise(vector<Point> contour, vector<int> diff_of_diff);
int compare(vector<vector<Point> > good_segments);
float get_slope( vector<Point> points);
vector<Point> run_merge(vector<vector<Point> > good_segments, Mat noiseDrawing);
cv::Point computeIntersect(cv::Vec4i a, cv::Vec4i b);
vector<Point> run_corner_clean( vector<Point> left_of_noise, vector<Point> right_of_noise, Mat noiseDrawing);
void run_parallel_clean( vector<Point> left_of_noise, vector<Point> right_of_noise, vector<Point> parallel_noise, Mat noiseDrawing);
int run_take_decision(vector<Point> left_of_noise,vector<Point> right_of_noise, vector<Point> noise, Mat noiseDrawing);
int ceilSearch(vector<int> arr, int x);
vector<Point> get_turning_points(vector<Point> noise, vector<int> diff_of_diff,vector<Point> seq_pos);
void get_noise_for_all(vector<vector<Point> > contours, Mat Contours);
void detectContours(Mat img);

#endif

