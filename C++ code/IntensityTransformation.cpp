#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string.h>
#include <time.h>

using namespace cv;
using namespace std;

Mat intensity_transformation(string image_path, string transformation_type, float power=0.25)
{
    /*
    This function is used to apply the transformation_type mentioned 
    to the image on the path specified and will display the image 
    after tranformation
    
    image_path : should provide the path of the image file on which 
    you wish to see the transformation
    
    transformation: should provide the type of transformation you wish
    to see applied on the image
    */

    Mat image; 

    if(transformation_type == "negative")
    {
        if (image_path == "")
            image_path = "/home/sriharsha.p/Image_Processing_Hands_on/Pictures/clock_bw.jpeg";

        image = imread( image_path, 0);    
        
        
        /*for (uint row_index=0; row_index < image.rows; row_index++)
        {
            for(uint column_index=0; column_index < image.cols; column_index++)
            {
                image.at<uchar>(row_index, column_index) = 255 - image.at<uchar>(row_index, column_index);
            }
        }*/ //takes around 2500 nanosecs

        image = 255-image; //takes around 850 nanosecs
    }


    if(transformation_type == "log")
    {
        if (image_path == "")
            image_path = "/home/sriharsha.p/Image_Processing_Hands_on/Pictures/DIP3E_Original_Images_CH03/Fig0305(a)(DFT_no_log).tif";

        image = imread( image_path, 0);    

        image = image + 1;
        image.convertTo(image,CV_32F);
        cv::log(image,image);
        cv::convertScaleAbs(image,image);
        cv::normalize(image,image,0,255,cv::NORM_MINMAX);
    }
     
    return image;
}

int main()
{
    string image_path="";
    string transformation_type = "log";
    clock_t start,end;
    start = clock();
    Mat image = intensity_transformation("",transformation_type);
    end = clock();
    cout<<"Time taken in secs is: "<< end-start <<endl;
    imshow("transformed_image", image);
    waitKey(0);
    return 0;
}