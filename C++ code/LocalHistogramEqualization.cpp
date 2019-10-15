#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string.h>
#include <time.h>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

cv::Mat local_histogram_equalization(string image_path, int kernel_size=3, float C=22.8, float k0=0, float k1=0.1, float k2=0, float k3=0.1)
{
    if(image_path == "")
        image_path = "/home/sriharsha.p/Image_Processing_Hands_on/Pictures/DIP3E_Original_Images_CH03/Fig0326(a)(embedded_square_noisy_512).tif";

    Mat image,pad_image;
    image = imread(image_path, 0); 
    
    cv::Scalar mean, std;
    cv::meanStdDev(image, mean, std);
    int GlobalMean,GlobalStd,LocalMean,LocalStd;
    GlobalMean = cvRound(mean[0]);
    GlobalStd = cvRound(std[0]);

    int pad = (int) (kernel_size/2);

    copyMakeBorder(image, pad_image, pad, pad, pad, pad, BORDER_CONSTANT, Scalar(0));

    bool mean_threshold,std_threshold;

    for(int i=0; i < image.rows; i++)
    {
        for(int j=0; j < image.rows; j++)
        {
            cv::Mat subImg = pad_image(cv::Rect(i, j, kernel_size, kernel_size));
            cv::meanStdDev(subImg, mean, std);
            LocalMean = cvRound(mean[0]);
            LocalStd  = cvRound(std[0]);
            mean_threshold = false;
            std_threshold = false;
            if (k0*GlobalMean <= LocalMean <= k1*GlobalMean)
                mean_threshold = true;
            if (k2*GlobalStd <= LocalStd <= k3*GlobalStd)
                std_threshold = true;
            if(mean_threshold && std_threshold)
                image.at<uchar>(i,j) = saturate_cast<uchar>(image.at<uchar>(i,j) * C);
        }
    }
    return image; 
} 

int main()
{
    string image_path="";
    clock_t start,end;
    start = clock();
    Mat new_image = local_histogram_equalization(image_path);
    end = clock();

    cout<<"Time taken in nano secs is : "<<end-start<<endl;

    imshow("Equilized Image",new_image);

    waitKey();
 
    return 0;
}