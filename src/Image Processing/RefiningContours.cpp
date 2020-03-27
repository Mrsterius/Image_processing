#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<opencv/highgui.h>
#include<opencv/cv.h>
#include<fstream>
#include<algorithm>
#include<functional>
using namespace std;
using namespace cv;

#include "HoughLinesP.h"
#include "RefiningContours.hpp"
RNG rng(12345);

vector<int> get_actual_difference(vector<Point> contour)
{
    
    vector<int> xrate(contour.size()-1);
    vector<int> yrate(contour.size()-1);
    vector<int> diffRates(contour.size()-1);
    
    
    for(int i=0;i<contour.size()-1;i++)
    {
        Point currentPoint=contour[i];
        Point nextPoint=contour[i+1];
        
        xrate[i]= nextPoint.x-currentPoint.x;
        yrate[i]= nextPoint.y-currentPoint.y;
        
        diffRates[i]=abs(xrate[i])-abs(yrate[i]);
        
    }
    
    return diffRates;
}

vector<int> get_difference(vector<int> v)
{
    vector<int> diffDiffRates(v.size()-1);
    
    for(int i=0;i<v.size()-1;i++)
    {
        diffDiffRates[i]= abs(v[i+1]-v[i]);
    }
    
    return  diffDiffRates;
}

vector<int> difference_of_difference(vector<Point> contour)
{
    vector<int> actual_diff = get_actual_difference(contour);
    
    cout<<endl<<"Difference"<<endl;
    for(int i=0;i<actual_diff.size();i++)
    {
        cout<<actual_diff[i];
    }
    return get_difference(actual_diff);
}

struct sequence
{
    vector<Point> noise;
    vector<Point> noise_seq;
    vector<Point> left_of_noise;
    vector<Point> right_of_noise;
    vector<vector<Point> > noise_Seq;
    
    vector<vector<Point> > left_of_noise_Seq;
    vector<vector<Point> > right_of_noise_Seq;
    
    vector<Point> good_points;
    //    vector<vector<Point> > good_segments;
}  s;

struct sequence rectify_end_points(vector<Point> tmp , struct sequence noise_end)
{
    vector<Point> temporary;
    
    if(tmp.size()<threshold_seq)   //Noise
    {
        
        for(int i=0; i<noise_end.noise_Seq[0].size();i++)
        {
            temporary.push_back(noise_end.noise_Seq[0][i]);
        }
        
        noise_end.noise_Seq[0].clear();
        for(int i=0;i<noise_end.noise_Seq[noise_end.noise_Seq.size()-1].size();i++)
        {
            noise_end.noise_Seq[0].push_back(noise_end.noise_Seq[noise_end.noise_Seq.size()-1][i]);
        }
        noise_end.noise_Seq[noise_end.noise_Seq.size()-1].clear();
        
        for(int i=0; i<tmp.size();i++)
        {
            noise_end.noise_Seq[0].push_back(tmp[i]);
            
        }
        
        for(int i=0; i<temporary.size();i++)
        {
            noise_end.noise_Seq[0].push_back(temporary[i]);
        }
        //give good points to left of noise[0]
        
        for(int i=0;i<length;i++)
        {
            noise_end.left_of_noise_Seq[0].push_back(noise_end.good_points[noise_end.good_points.size()-1-i]);
        }
        //let remain right of noise[0]
    }
    else    //Good Points
    {
        
        for(int i=0;i<length;i++)
        {
            noise_end.left_of_noise.push_back(noise_end.good_points[noise_end.good_points.size()-1-i]);
        }
        
        
        
        for(int i=0; i<tmp.size();i++)
        {
            noise_end.good_points.push_back(tmp[i]);
            if(i<5)
                noise_end.right_of_noise.push_back(tmp[i]);
                }
        
        
        
        for(int i=0;i<length;i++)
        {
            noise_end.left_of_noise_Seq[0].push_back(noise_end.good_points[noise_end.good_points.size()-1-i]);
        }
        
        
        
        noise_end.left_of_noise_Seq.push_back(noise_end.left_of_noise);
        noise_end.left_of_noise.clear();
        noise_end.right_of_noise_Seq.push_back(noise_end.right_of_noise);
        noise_end.right_of_noise.clear();
    }
    
    return noise_end;
}

struct sequence run_get_noise(vector<Point> contour, vector<int> diff_of_diff)
{
    //Number of bumps
    int tol = 0;
    int seq_of = diff_of_diff[0];           //Stores current sequence 0 | 1 | 2
    int map_val[] = {0,0,0};                // Stores frequency of occuring 0 | 1 | 2
    int flag=NON_NOISE;
    
    vector<Point> tmp;// noise;//good_points;
    
    cout<<diff_of_diff.size()<<endl;
    
    
    for(int i=0; i < diff_of_diff.size();i++)
    {
        //cout<<tmp.size()<<endl;
        //cout<<"("<<i<<") ";
        //cout<<diff_of_diff[i];
        
        
        tmp.push_back(contour[i]);
        
        map_val[diff_of_diff[i]]++;
        if(diff_of_diff[i] != seq_of)
        {
            //count=0;
            if(tol == threshold_tol)    //Number of bumps > 1
            {
                if (map_val[seq_of] <= threshold_seq)  //seq_of less than 10
                {
                    
                    flag = NOISE;
                    
                    for (int j = 0;j<tmp.size() - threshold_tol; j++)
                    {
                        s.noise.push_back(tmp[j]);
                        if( (j==0 || j==tmp.size()-threshold_tol-1) && 0!=tmp.size()-threshold_tol-1)
                        {
                            s.noise_seq.push_back(tmp[j]);
                        }
                        else
                            continue;
                        
                        tmp.erase(tmp.begin()+j);
                        //NOISE
                        //cout<<"1";
                        
                    }
                    
                }
                
                else
                {
                    if(flag==NOISE)
                    {
                        
                        s.noise_Seq.push_back(s.noise);
                        s.noise.clear();
                        
                        
                        int pos_length_good_points = s.good_points.size();
                        
                        if(pos_length_good_points==0)
                        {
                            cout<<"Check for Good Points later On"<<endl;
                            
                        }
                        
                        else
                        {
                            for(int j=0; j<length;j++)
                            {
                                s.left_of_noise.push_back(s.good_points[s.good_points.size()-1-j]);
                            }
                        }
                        
                        s.left_of_noise_Seq.push_back(s.left_of_noise);
                        s.left_of_noise.clear();
                        for (int j = 0;j<tmp.size() - threshold_tol; j++)
                        {
                            
                            if(j<5)
                                s.right_of_noise.push_back(tmp[j]);
                            
                            
                            
                            s.good_points.push_back(tmp[j]);
                            
                            tmp.erase(tmp.begin()+j);
                            // REQUIRED POINTS
                            
                            //cout<<"0";
                            
                        }
                        
                        s.right_of_noise_Seq.push_back(s.right_of_noise);
                        s.right_of_noise.clear();
                    }
                    
                    //                    s.good_segments.push_back(s.good_points);
                    //                    s.good_points.clear();
                    
                    
                    //                    calledOnce = 1 ;
                }
                
                
                map_val[0] = 0;
                map_val[1] = 0;
                map_val[2] = 0;
                seq_of = diff_of_diff[i-threshold_tol];
                i -= threshold_tol;
                tmp.clear();
                tol = 0;
                //cout<<"--1"<<endl;
                
                
            }
            else
                tol ++;
        }
        else
        {
            
            tol = 0;
            
            
            
        }
    }
    
    if(!s.noise.empty())
    {
        s.noise_Seq.push_back(s.noise);
        s.noise.clear();
    }
    
    if(!tmp.empty())
    {
        s = rectify_end_points(tmp , s);
    }
    
    return s;
    
    }
    
    
    
    
    
//    int compare(vector<vector<Point> > good_segments)
//    {
//        int length_vectors[good_segments.size()];
//        for(int i=0; i<good_segments.size();i++)
//        {
//            length_vectors[i] = good_segments[i].size();
//            
//        }
//        
//        const int N = sizeof(length_vectors) / sizeof(int);
//        int position=  distance(length_vectors, max_element(length_vectors, length_vectors+ N));
//        
//        return position;
//    }
    
    float get_slope( vector<Point> points)
    {
        vector<int> slope_seq(points.size());
        int slope;
        int max_occuring_slope;
        int map_val[]= {0,0,0,0};
        for(int i = 0;i<points.size();i++)
        {
            slope = round(atan2(points[i].y-points[i+1].y,points[i].x-points[i+1].x)* 180.0/ CV_PI);
            
            if(slope==0 || slope==180)
            {
                slope_seq.push_back(0);
                map_val[0]++;
            }
            else if(slope==90 || slope==-90)
            {
                slope_seq.push_back(90);
                map_val[3]++;
            }
            else if(slope==45 || slope==-135)
            {
                slope_seq.push_back(45);
                map_val[1]++;
            }
            else if(slope==135 || slope==-45)
            {
                slope_seq.push_back(135);
                map_val[2]++;
            }
            else
                cout<<"Slope Not Found;; Slope ="<<slope<<endl;
            
            
        }
        
        //find_maximum_Occuring_SLOPE
        
        const int N = sizeof(map_val) / sizeof(int);
        
        
        int position=  distance(map_val, max_element(map_val, map_val + N));
        
        switch(position)
        {
            case 0:
                max_occuring_slope = 0;
                break;
            case 1:
                max_occuring_slope = 45;
                break;
            case 2:
                max_occuring_slope = 135;
                break;
            case 3:
                max_occuring_slope = 90;
                break;
            default:
                cout<<"Slope not found";
        }
        
        return max_occuring_slope;
    }
    
    vector<Point> run_merge(vector<vector<Point> > good_segments, Mat noiseDrawing)
    {
        
        int max_segment_index=compare(good_segments);
        vector<Point> longest_good_segment= good_segments[max_segment_index];
        
        float slope = get_slope(longest_good_segment);
        int length = 0;
        Point p;
        for(int i=0; i<good_segments.size();i++)
        {
            if(i == max_segment_index)
                continue;
            
            if(i<max_segment_index)
            {
                int count=i;
                while(max_segment_index-count!=1)
                {
                    count++;
                    length += good_segments[count].size();
                    
                }
                
                //                for(int j=length; j<length+good_segments[i].size();j++)
                //                {
                //
                //                    good_segments[i][j].x =  (int)round(longest_good_segment[0].x + j * cos(slope * CV_PI / 180.0));
                //                    good_segments[i][j].y =  (int)round(longest_good_segment[0].y + j * sin(slope * CV_PI / 180.0));
                //
                ////                    good_segments[i][j].x = p.x;
                ////                    good_segments[i][j].y = p.y;
                //
                //                }
                p.x =  (int)round(longest_good_segment[0].x + (length + good_segments[0].size()) * cos(slope * CV_PI / 180.0));
                p.y =  (int)round(longest_good_segment[0].y+ (length + good_segments[0].size()) * sin(slope * CV_PI / 180.0));
                
                
                line(noiseDrawing, longest_good_segment[0], p, Scalar(127,0,127));
                imwrite("../Images/Harmony Residencyhr1Merged_points.png",noiseDrawing);
                
            }
            
            if(i>max_segment_index)
            {
                int count=i;
                while(abs(max_segment_index-count)!=1)
                {
                    count--;
                    length += good_segments[count].size();
                    
                }
                
                
                for(int j=length; j<length+good_segments[i].size();j++)
                {
                    
                    p.x =  (int)round(longest_good_segment[longest_good_segment.size()-1].x + j * cos(slope * CV_PI / 180.0));
                    p.y =  (int)round(longest_good_segment[longest_good_segment.size()-1].y+ j * sin(slope * CV_PI / 180.0));
                    
                    good_segments[i][j].x = p.x;
                    good_segments[i][j].y = p.y;
                    
                }
            }
            
            
        }
        
        vector<Point> final_good_segment;
        for(int i=0; i<good_segments.size();i++)
        {
            for(int j=0; j<good_segments[i].size();j++)
                final_good_segment.push_back(good_segments[i][j]);
        }
        
        return final_good_segment;
    }
    
    
    
    cv::Point computeIntersect(cv::Vec4i a, cv::Vec4i b)
    {
        int x1 = a[0], y1 = a[1], x2 = a[2], y2 = a[3];
        int x3 = b[0], y3 = b[1], x4 = b[2], y4 = b[3];
        
        if (float d = ((float)(x1-x2) * (y3-y4)) - ((y1-y2) * (x3-x4)))
        {
            cv::Point pt;
            pt.x = ((x1*y2 - y1*x2) * (x3-x4) - (x1-x2) * (x3*y4 - y3*x4)) / d;
            pt.y = ((x1*y2 - y1*x2) * (y3-y4) - (y1-y2) * (x3*y4 - y3*x4)) / d;
            return pt;
        }
        else
            return cv::Point(-1, -1);
    }
    
    vector<Point> run_corner_clean( vector<Point> left_of_noise, vector<Point> right_of_noise, Mat noiseDrawing)
    {
        
        vector<Point> corrected_points;
        Vec4i a,b;
        
        
        a[0] = left_of_noise[0].x;
        a[1] = left_of_noise[0].y;
        a[2] = left_of_noise[left_of_noise.size()-1].x;
        a[3] = left_of_noise[left_of_noise.size()-1].y;
        b[0] = right_of_noise[0].x;
        b[1] = right_of_noise[0].y;
        b[2] = right_of_noise[right_of_noise.size()-1].x;
        b[3] = right_of_noise[right_of_noise.size()-1].y;
        
        Point intersection = computeIntersect(a, b);
        if(intersection == cv::Point(-1, -1))
            cout<<"PARALLEL LINES";
        
        
        line(noiseDrawing, intersection, right_of_noise[0], Scalar(255,255,255));
        line(noiseDrawing, intersection, left_of_noise[0], Scalar(255,255,255));
        imwrite("../Images/Harmony Residencyhr1Corner_clean_points.png",noiseDrawing);
        
        
        return corrected_points;
    }
    
    void run_parallel_clean( vector<Point> left_of_noise, vector<Point> right_of_noise, vector<Point> parallel_noise, Mat noiseDrawing)
    {
        float slope_left = get_slope(left_of_noise);
        //        float slope_right = get_slope(right_of_noise);
        cout<<"GOT SLOPE"<<endl;
        
        // calculate mid point
        
        cout<<"PARALLEL NOISE SIZE"<<parallel_noise.size()<<endl;
        
        Point found;
        
        found = parallel_noise[parallel_noise.size()/2];
        cout<<"FOUND"<<endl;
        // draw a line perpendicular to this slope at the found point
        
        float slope;
        
        int left_width = 10;
        int right_width = 10;
        if(slope_left<90)
        {
            slope = slope_left+90;
        }
        else
            slope = slope_left-90;
        
        // extend lines from left_of_noise and right_of_noise
        
        Point point_right,point_left;
        
        
        point_right.x =  (int)round(found.x + right_width * cos(slope* CV_PI / 180.0));
        point_right.y =  (int)round(found.y + right_width * sin(slope * CV_PI / 180.0));
        
        point_left.x =  (int)round(found.x + left_width * cos(slope* CV_PI / 180.0));
        point_left.y =  (int)round(found.y + left_width * sin(slope * CV_PI / 180.0));
        
        // compute intersection from perpendicular line to these 2 lines and join them
        
        Vec4i a,b,c;
        a[0] = found.x;
        a[1] = found.y;
        a[2] = point_left.x;
        a[3] = point_left.y;
        b[0] = left_of_noise[0].x;
        b[1] = left_of_noise[0].y;
        b[2] = left_of_noise[left_of_noise.size()-1].x;
        b[3] = left_of_noise[left_of_noise.size()-1].y;
        
        Point found_left = computeIntersect(a, b);
        
        if(found_left == Point(-1,-1))
        {
            cout<<"LEFT INTERSECTION DOES NOT EXIST";
        }
        
        a[2] = point_right.x;
        a[3] = point_right.y;
        
        c[0] = right_of_noise[right_of_noise.size()-1].x;
        c[1] = right_of_noise[right_of_noise.size()-1].y;
        c[2] = right_of_noise[0].x;
        c[3] = right_of_noise[0].y;
        
        Point found_right = computeIntersect(a,c);
        
        if(found_right == Point(-1,-1))
        {
            cout<<"RIGHT INTERSECTION DOES NOT EXIST";
            
        }
        
        line(noiseDrawing, found_right, found_left, Scalar(255,255,255));
        line(noiseDrawing, left_of_noise[0], found_left, Scalar(255,255,255));
        line(noiseDrawing, right_of_noise[0],found_right, Scalar(255,255,255));
        //        cv::LineIterator it(noiseDrawing, found_left,found_right, 8);
        //        std::vector<cv::Vec3b> buf(it.count);
        //        std::vector<cv::Point> points(it.count);
        //
        //        for(int i = 0; i < it.count; i++, ++it)
        //        {
        //            buf[i] = *(const cv::Vec3b)*it;
        //            points[i] = it.pos();
        //        }
        namedWindow("PARALLEL CLEANED", CV_WINDOW_NORMAL);
        imshow("PARALLEL CLEANED",noiseDrawing);
        imwrite("../Images/Harmony Residencyhr1Parallel_points.png",noiseDrawing);
        
    }
    
    
    int run_take_decision(vector<Point> left_of_noise,vector<Point> right_of_noise, vector<Point> noise, Mat noiseDrawing)
    {
        int slope_left = abs(get_slope(left_of_noise));
        int slope_right = abs(get_slope(right_of_noise));
        if(slope_left!=slope_right)
        {
            cout<<"RUNNING CORNER_CLEAN"<<endl;
            vector<Point> corner_points =  run_corner_clean( left_of_noise, right_of_noise, noiseDrawing);
        }
        else
        {
            Vec4i a,b;
            a[0] = left_of_noise[0].x;
            a[1] = left_of_noise[0].y;
            a[2] = left_of_noise[left_of_noise.size()-1].x;
            a[3] = left_of_noise[left_of_noise.size()-1].y;
            
            b[0] = right_of_noise[0].x;
            b[1] = right_of_noise[0].y;
            b[2] = right_of_noise[right_of_noise.size()-1].x;
            b[3] = right_of_noise[right_of_noise.size()-1].y;
            Point intersection =  computeIntersect(a, b);
            
            Point join_left, join_right;
            
            
            if(intersection != cv::Point(-1,-1))
            {
                line(noiseDrawing, left_of_noise[0], right_of_noise[0], Scalar(255,255,255));
                cout<<"RUNNING JOIN_POINTS"<<endl;
            }
            else
            {
                cout<<"RUNNING PARALLEL_CLEAN"<<endl;
                run_parallel_clean( left_of_noise,right_of_noise, noise, noiseDrawing);
                
            }
        }
        return 1;
    }
    
    
//    int ceilSearch(vector<int> arr, int x)
//    {
//        
//        
//        if(x <= arr[0])
//            return 0;
//        
//        for(int i = 0; i < arr.size(); i++)
//        {
//            if(arr[i] == x)
//                return i;
//            
//            if(arr[i] < x && arr[i+1] >= x)
//                return i+1;
//        }
//        
//        /* If we reach here then x is greater than the last element
//         of the array,  then return -1. */
//        return -1;
//    }

    
    
    
//    vector<Point> get_turning_points(vector<Point> noise, vector<int> diff_of_diff,vector<Point> seq_pos)
//    {
//        int length_turning_points=0;
//        vector<Point> turning_points;
//        
//        for(int i=0;i<noise.size();i++)
//        {
//            
//            if(diff_of_diff[i]!=0)
//            {
//                turning_points.push_back(noise[i]);
//                length_turning_points++;
//            }
//
//            // Find percentage of turning Points in Noise.
//        }
//        
//        
//        return turning_points;
//    }

    
    
    
//void test_get_noise()
//{
//    //
//    string seq = "010000000000000000000000000000000000000000000000000000000101000111021000001110100000000000000000000000000000000000000000001010000000000000000000000101110000000000000000000000011000000011000001100000111211112100000000000000000000000000000000000000000000011011010100011000000000000000000001000100010002100011011000000000001101100110000000000000000000000000000000001100000001100000000000000000000000000000000000000000000000000000000000000000000000000000001";
//    vector<int> d_o_d;
//    cout<<seq<<endl;
//    for (int i=0;i<seq.length();i++)
//    {
//        d_o_d.push_back(int(seq[i])-'0');
//        //cout<<d_o_d[i];
//    }
//    vector<int> noise = get_noise(d_o_d);
//    cout<<endl<<"NOISE"<<endl;
//
//    for(int i=0; i<noise.size();i++)
//    {
//        cout<<noise[i];
//    }
//
//}

    void get_noise_for_all(vector<vector<Point> > contours, Mat Contours)
    {
        
        Mat noiseDrawing= Contours.clone();
        cvtColor(noiseDrawing,noiseDrawing,CV_GRAY2BGR);
        Mat turnsDrawing= Contours.clone();
        cvtColor(turnsDrawing, turnsDrawing, CV_GRAY2BGR);
        Mat sequenceDrawing= Contours.clone();
        cvtColor(sequenceDrawing, sequenceDrawing, CV_GRAY2BGR);
        Mat cleanDrawing = Mat::zeros(Contours.rows, Contours.cols, CV_8UC3);
        vector<int> tmp;
        
        
        for(int i=0;i<contours.size();i++)
        {
            
            
            
            if(contourArea(contours[i])<500)
                continue;
            //                exit(10);
            cout<<"STEP 1"<<endl;
            tmp=difference_of_difference(contours[i]);
            cout<<endl;
            cout<<"Difference_of_Difference"<<endl;
            
            for(int i=0;i<tmp.size();i++)
            {
                cout<<tmp[i];
            }
            cout<<"STEP 2"<<endl;
            cout<<endl<<"Noise"<<endl;
            s.noise_Seq.clear();
            s.left_of_noise_Seq.clear();
            s.right_of_noise_Seq.clear();
            s.good_points.clear();
            struct sequence get = run_get_noise(contours[i], tmp);
            cout<<"STEP 3"<<endl;
            //if(get.right_of_noise.empty() )
            //            if(get.left_of_noise.empty())
            //                continue;
            
            // plot_noise
            
            
            int radius=0;
            
            //        cout<<"SIZE OF CONTOURS"<<contours.size()<<endl;
            
            for(int i=0; i<get.good_points.size();i++)
            {
                
                Scalar color = Scalar( 255,255,255);
                circle( cleanDrawing, get.good_points[i], (int)radius, color, 2, 8, 0 );
                
            }
            
            for(int i=0; i<get.left_of_noise_Seq.size();i++)
                for(int j=0; j<get.left_of_noise_Seq[i].size();j++)
                {
                    Scalar color = Scalar( 0,255,0);
                    circle( noiseDrawing, get.left_of_noise_Seq[i][j], (int)radius, color, 2, 8, 0 );
                    
                }
            
            for(int i=0; i<get.right_of_noise_Seq.size();i++)
                for(int j=0; j<get.right_of_noise_Seq[i].size();j++)
                {
                    Scalar color = Scalar( 255,0,0);
                    circle( noiseDrawing, get.right_of_noise_Seq[i][j], (int)radius, color, 2, 8, 0 );
                    
                }
            cout<<"STEP 5"<<endl;
            
            for(int i=0; i<get.noise_Seq.size();i++)
            {
                for(int j=0; j<get.noise_Seq[i].size();j++)
                {
                    Scalar color = Scalar( 0,0,255 );
                    circle( noiseDrawing, get.noise_Seq[i][j], (int)radius, color, 2, 8, 0 );
                    
                }
                
            }
            
            //        vector<vector<Point> > dummy_good_segments;
            //
            //        dummy_good_segments.push_back(get.good_segments[2]);
            //        dummy_good_segments.push_back(get.good_segments[3]);
            //        //        vector<Point> merged_segment;
            //        merged_segment = run_merge(dummy_good_segments, noiseDrawing);
            
            //        imwrite("../Images/Harmony Residencyhr1Good_points.png",noiseDrawing);
            //
            if(get.left_of_noise_Seq.size() != get.noise_Seq.size())
            {
                cout<<"ISSUE"<<endl<<get.noise_Seq.size()<<endl;
                //                continue;
            }
            
            for(int i=0; i<get.left_of_noise_Seq.size();i++)
            {
                run_take_decision(get.left_of_noise_Seq[i], get.right_of_noise_Seq[i], get.noise_Seq[i], noiseDrawing);
                
            }
            cout<<"STEP 6";
         
        }
      
      
        namedWindow("CORNER POINTS",CV_WINDOW_NORMAL);
        imshow("CORNER POINTS",noiseDrawing);
        
        namedWindow("SEQUENCE POINTS",CV_WINDOW_NORMAL);
        imshow("SEQUENCE POINTS",sequenceDrawing);
        
        imwrite("../Images/Harmony Residencyhr1Sequence_points_new.png",sequenceDrawing);
        
        imwrite("../Images/Harmony Residencyhr1noise_points.png",noiseDrawing);
        imwrite("../Images/Harmony Residencyhr1Square_points.png",noiseDrawing);
        //        imwrite("../Images/Harmony Residencyhr1Square_Cleaned_points.png",cleanDrawing);
        
        //       detectLinesProbabilistic(sequenceDrawing);
        
    }
    
    void detectContours(Mat img)
    {
        
        cv::Mat bw;
        Mat img_gray;
        // Convert to grayscale
        
        cv::cvtColor(img, img_gray, CV_BGR2GRAY);
      
        
        bitwise_not(img_gray,img_gray);
        
        //        imwrite("../Images/Harmony Residencyhr1Contrast_inverted.png",img_gray);
        
        threshold(img_gray, edges, 127, 255, THRESH_BINARY);
        //        detectLinesProbabilistic(img_gray);
        Mat Contours;
        
        
        medianBlur(img_gray, img_gray, 31);
        
        //        imwrite("/Users/himanshuhansaria/Downloads/hr1Median_blurred.png",img_gray);
        
        // Use Canny instead of threshold to catch squares with gradient shading
        
        Canny(img_gray, Contours, 50, 50);
        //cv::Canny(img_gray, Contours, 3000,3000, 5);
        cvNamedWindow("Canny",CV_WINDOW_NORMAL);
        cv::imshow("Canny",Contours);
        
        
        // threshold(img_gray, bw, 65, 255, CV_THRESH_BINARY);
        //namedWindow("Thresholded");
        //imshow("Threshold",bw);
        
        vector<vector<Point> > contours;
        findContours(Contours.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
        
        vector< vector< Point > > contours_poly( contours.size() );
        vector< Rect > boundRect (contours.size() );
        
        get_noise_for_all(contours, Contours);
        
        for( int i = 0; i < contours.size(); i++ )
        {
            //if (isContourConvex(contours[i]) == 0) //check first if the contour is convex or not.
            //{
            if (arcLength(contours[i], true)) //checks if the contour is closed or not. If true finds it's perimeter
            {
                approxPolyDP(Mat(contours[i]), contours_poly[i], 3,true);
                
            }
            //}
        }
        Mat drawing = Mat::zeros( Contours.size(), CV_8UC3 );
        
        for( int i = 0; i< contours.size(); i++ )
        {
            Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
            drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
            
        }
        
        imwrite("../Images/Harmony Residencyhr1Contours.png",drawing);
        

        int n,count=0;
        int noCorners=0;
        
        //// WRITING FILES
        
        
        for (unsigned int i = 0; i < contours.size(); i++)
        {
            //coord << "# of contour points: " << contours[i].size() << endl ;
            
            char buffer[500];
            if (contourArea(contours[i])>10)
            {
                n=sprintf(buffer,"/Users/himanshuhansaria/Desktop/HOUSING/Projects/ConstructionAreaDetect/ConstructionAreaDetect/Coordinates/cnt%d.txt",count);
                //cout<<buffer<<endl;
                ofstream w_file;
                w_file.open(buffer,ios::trunc);
                int loop_count_for_printing=0;
                
                for( int j=0;j<contours[i].size();j++)
                {
                    Point pt=contours[i][j];
                    
                    if(j!=0 && j!=contours[i].size()-1)
                    {
                        int dx1,dx2,dy1,dy2;
                        
                        Point pt0 = contours[i][j-1];
                        Point pt2 = contours[i][j+1];
                        
                        dx1 = pt.x - pt0.x;
                        dx2 = pt2.x - pt.x;
                        dy1 = pt.y - pt0.y;
                        dy2 = pt2.y - pt.y;
                        
                        if(dx1!=dx2 || dy1!=dy2)
                        {
                            
                            noCorners++;
                            loop_count_for_printing++;
                            //cout<<pt.x<<" "<<pt.y<<endl;
                            
                            //for avoiding un-necessary endlines in the ends of file
                            if(loop_count_for_printing==1)
                            {
                                w_file<<pt.x<<" "<<pt.y;
                            }
                            else
                            {
                                w_file<<"\n"<<pt.x<<" "<<pt.y;
                            }
                        }
                        
                    }
                    else
                    {
                        noCorners++;
                        loop_count_for_printing++;
                        // cout<<pt.x<<" "<<pt.y<<endl;
                        
                        //for avoiding  un-necessary endlines in the ends of file
                        if(loop_count_for_printing==1)
                        {
                            w_file<<pt.x<<" "<<pt.y;
                        }
                        else
                        {
                            w_file<<"\n"<<pt.x<<" "<<pt.y;
                        }
                    }
                    
                }
                w_file.close();
                count++;
            }
            
            
        }
        
        
    }
    
    
    int main(int argc, char** argv)
    {
        if( argc != 2)
        {
            cout <<"Supply correct Arguments" << endl;
            return -1;
        }
//
//        Mat img = imread("../Images/Harmony Residency/hr1.jpg", CV_LOAD_IMAGE_UNCHANGED);
        Mat img = imread(argv[1], CV_LOAD_IMAGE_UNCHANGED);
        //
        if (img.empty())
        {
            cout << "Error!" << endl;
            system("pause");
            return -1;
        }
        img.convertTo(img, -1, 2, 50); //max the contrast and brightness (double)
        namedWindow("Contrasted Image");
        imshow("Contrasted Image", img);
        imwrite("../Images/Harmony Residencyhr1Contrasted_Image.png",img);
        
        //cvtColor(img, img, CV_BGR2GRAY); //change the color image to grayscale image
        
        //Mat img_hist_equalized;
        //equalizeHist(img, img_hist_equalized); //equalize the histogram
        
        //   namedWindow("Histogram Equalized", CV_WINDOW_AUTOSIZE);
        //  imshow("Histogram Equalized", img_hist_equalized);
        detectContours(img);
        
        //detectLines(img);
        
        waitKey(0);
        destroyAllWindows();
        
        return 0;
    }