#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string.h>
#include <time.h>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

float median_mat(Mat local_region)
{
    Mat temp;
    local_region.copyTo(temp);
    temp = temp.reshape(0,1); // spread Input Mat to single row
    std::vector<float> vecFromMat;
    temp.copyTo(vecFromMat); // Copy Input Mat to vector vecFromMat
    std::nth_element(vecFromMat.begin(), vecFromMat.begin() + vecFromMat.size() / 2, vecFromMat.end());
    return vecFromMat[vecFromMat.size() / 2];
}

Mat noise_filter(string image_path, string kernel_type="Guassian", int kernel_size=21, string pad="zero")
{
    /*
    This function is mainly used to remove the nosie from images
    image path: Takes the input path of the image stored
    kernel Type: Guassian,median
    Kernel size: size of the kernel which you want to apply to the image
    pad: type of padding to be applied on the image (zero,symmetric,reflect)
    */
   if (image_path == "")
        image_path = "/home/sriharsha.p/Image_Processing_Hands_on/Pictures/DIP3E_Original_Images_CH03/Fig0333(a)(test_pattern_blurring_orig).tif";

   Mat image,pad_image;
   image = imread(image_path,0);
   int pad_size = kernel_size/2;

   if(pad=="zero")
   {
       copyMakeBorder(image, pad_image, pad_size, pad_size, pad_size, pad_size, BORDER_CONSTANT, cv::Scalar(0));
       pad_image.convertTo(pad_image,CV_32FC1);
   }


   Mat kernel(kernel_size, kernel_size, CV_32FC1); 

   if( kernel_type == "Guassian")
   {
        float sigma = round(kernel_size/3.0)/2;
        //float sigma=1;
        cout<<"sigma is : "<<sigma<<endl;
        float sum = 0.0;
        //to get the x and y cordinates from which gaussian distribution is calculated
        int cord_vals = kernel_size/2;
        for(int i=(cord_vals*-1); i <= cord_vals; i++)
        {
            for(int j=(cord_vals*-1); j <= cord_vals; j++)
            {
                float numerator = pow(i, 2) + pow(j, 2);
                float denominator = 2.0 * pow(sigma, 2);
                kernel.at<float>(i+cord_vals,j+cord_vals) = exp(-numerator/denominator);
                //cout<<"kernel value at i : "<<i+cord_vals<< " kernel value at j : "<<j+cord_vals<< " is: "<< kernel.at<float>(i+cord_vals,j+cord_vals) <<endl;
            }
        }
        sum = cv::sum(kernel)[0];
        //cout<<"sum is : "<<sum<<endl;
        kernel = kernel/sum;
   }

   Mat local_region(kernel_size,kernel_size,CV_32FC1);

   if(kernel_type != "median")
   {
        for(int i=0; i< image.rows; i++)
        {
            for(int j=0; j< image.cols; j++)
            {    
                local_region = pad_image(cv::Rect(j, i, kernel_size, kernel_size));
                image.at<uchar>(i,j) = saturate_cast<uchar>(cv::sum(local_region.mul(kernel))[0]);
            }
        }
   }
   else
   {
      for(int i=0; i< image.rows; i++)
        {
            for(int j=0; j< image.cols; j++)
            {    
                local_region = pad_image(cv::Rect(j, i, kernel_size, kernel_size));
                image.at<uchar>(i,j) = saturate_cast<uchar>(median_mat(local_region));
            }
        }
   }

    double min, max;
    cv::minMaxIdx(image, &min, &max);
    std::cout<<"maximum is : "<< max << " minimum is : "<< min<<endl;
   
   return image;        
}

/*
int main()
{
    string image_path = "/home/sriharsha.p/Image_Processing_Hands_on/Pictures/DIP3E_Original_Images_CH03/Fig0335(a)(ckt_board_saltpep_prob_pt05).tif";

    Mat image,image_new;
    image = imread(image_path,0);
    
    clock_t start,end;

    start = clock();
    image_new = noise_filter(image_path,"median",7);
    end = clock();

    cout<<"Time taken in nanosecs is: "<<end-start<<endl;

    imshow("original_image",image);
    imshow("transformed_image",image_new);

    waitKey();
    return 0;
}*/