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

        image = image + 1; // adding 1 to all pixels since log(0) is undefined
        image.convertTo(image,CV_32F); //converting the image to float
        cv::log(image,image); //appling log func to all the pixels in an image
        cv::convertScaleAbs(image,image); // converting any negatives to postive and chaging the type to uint8(uchar)
        cv::normalize(image,image,0,255,cv::NORM_MINMAX); //normalizing the pixel values so we can visualize the resultant image
    }

    if(transformation_type == "gamma")
    {
        double min, max;
        if (image_path == "")
            image_path = "/home/sriharsha.p/Image_Processing_Hands_on/Pictures/DIP3E_Original_Images_CH03/Fig0308(a)(fractured_spine).tif";

        image = imread( image_path, 0);    
        
        image.convertTo(image,CV_32F);// converting the image to float
        cv::pow(image, power, image); // appllying the required gamma correction
        minMaxIdx(image, &min, &max); // finding the min and max values of the elements in the matrix
        cv::convertScaleAbs(image,image,255.0/max); // converting any negatives to postive , scaling the pixel values and chaging the type to uint8(uchar)
    }
     
    return image;
}

int contrast_tranform(int x, int r1, int s1, int r2, int s2)
{
    float result;
    if(0 <= x && x <= r1){
        result = s1/r1 * x;
    }else if(r1 < x && x <= r2){
        result = ((s2 - s1)/(r2 - r1)) * (x - r1) + s1;
    }else if(r2 < x && x <= 255){
        result = ((255 - s2)/(255 - r2)) * (x - r2) + s2;
    }
    return (int)result;
}

Mat contrast_stretching(string image_path, int r1, int s1, int r2, int s2 )
{
    /*
    The locations of points (r1,s1) and (r2,s2) control the shape of the transformation function.
    If r1=s1 and r2=s2 the transformation is a linear function that produces no changes in intensity.
    If r1=r2 and s1=0 and s2=L-1 the transformation becomes a thresholding function that creates a binary image.
    Intermediate values of (r1, s1) and (r2, s2) produce various degrees of spread in the intensity levels 
    of the output image, thus affecting its contrast
    */
    
    if (image_path == "")
            image_path = "/home/sriharsha.p/Image_Processing_Hands_on/Pictures/DIP3E_Original_Images_CH03/Fig0310(b)(washed_out_pollen_image).tif";

    Mat image_new = imread( image_path, 0);
    //Mat image_new = image.clone();

    for (int image_row=0; image_row < image_new.rows; image_row++)
    {
        for (int image_column=0; image_column < image_new.cols; image_column++)
        {
            image_new.at<uchar>(image_row,image_column) = contrast_tranform(image_new.at<uchar>(image_row,image_column), r1, s1, r2, s2);
        }
    }

    cv::convertScaleAbs(image_new,image_new);
 
    return image_new;

}
int main()
{
    //string image_path="/home/sriharsha.p/Image_Processing_Hands_on/Pictures/retina.png";
    string image_path="";
    string transformation_type = "gamma";
    //Mat image = intensity_transformation(image_path,transformation_type,0.4);
    clock_t start,end;
    start = clock();
    Mat image = imread( "/home/sriharsha.p/Image_Processing_Hands_on/Pictures/DIP3E_Original_Images_CH03/Fig0310(b)(washed_out_pollen_image).tif", 0);
    double min, max;
    minMaxIdx(image, &min, &max);
    Mat image_new = contrast_stretching(image_path,min,0,max,255);
    end = clock();
    cout<<"Time taken in secs is: "<< end-start <<endl;

    namedWindow("Original Image", 1);
    imshow("Original Image", image);
 
    namedWindow("New Image", 1);
    imshow("New Image", image_new);
 
    waitKey();
 
    return 0;
}