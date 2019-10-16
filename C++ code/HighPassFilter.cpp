#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string.h>
#include <time.h>
#include <opencv2/imgproc/imgproc.hpp>
#include "LowPassFilter.cpp"

using namespace cv;
using namespace std;

Mat sharpening_filter(String image_path, string kernel_type="laplacian_with_diagonals", int kernel_size=3, string pad="zero")
{
    /*
    This function is mainly used to sharpen images
    image path: Takes the input path of the image stored
    kernel Type: laplacian_with_diagonal elements filled/without filled
    Kernel size: size of the kernel which you want to apply to the image
    pad: type of padding to be applied on the image (zero,symmetric,reflect)
    */

    if (image_path == "")
        image_path = "/home/sriharsha.p/Image_Processing_Hands_on/Pictures/DIP3E_Original_Images_CH03/Fig0338(a)(blurry_moon).tif";

    Mat image,pad_image;
    image = imread(image_path,0);
    image.convertTo(image,CV_32F);
    int pad_size = kernel_size/2;    

    if(pad=="zero")
    {
       copyMakeBorder(image, pad_image, pad_size, pad_size, pad_size, pad_size, BORDER_CONSTANT, cv::Scalar(0.0));
    }    

    Mat kernel;

    if(kernel_type == "laplacian_with_diagonals")
    {
        int center = int(kernel_size/2);
        kernel = Mat::ones(kernel_size, kernel_size, CV_32F);
        kernel.at<float>(center,center) = kernel.at<float>(center,center) * (-1) * (pow(kernel_size,2) -1);
        kernel = kernel/8;
    }

    Mat local_region(kernel_size,kernel_size,CV_32F);

    for(int i=0; i< image.rows; i++)
    {
        for(int j=0; j< image.cols; j++)
        {    
            local_region = pad_image(cv::Rect(j, i, kernel_size, kernel_size));
            image.at<float>(i,j) =  image.at<float>(i,j) - cv::sum(local_region.mul(kernel))[0];
        }
    }
    //double min, max;
    //cv::minMaxIdx(image, &min, &max);
    //std::cout<<"maximum is : "<< max << " minimum is : "<< min<<endl;
    //image = (image - min);
    //image = image/(max - min);
    cv::normalize(image,image,0,1,cv::NORM_MINMAX);
    //cv::minMaxIdx(image, &min, &max);
    //std::cout<<"maximum is : "<< max << " minimum is : "<< min<<endl;
    return image;
}

Mat unsharp_masking(string image_path, int kernel_size=31, float mask_weightage=1)
{
    if (image_path == "")
        image_path = "/home/sriharsha.p/Image_Processing_Hands_on/Pictures/softone.png";

    Mat image,guassian_blur,mask,scaled_mask,unsharp_masked_image;
    image = imread(image_path,0);
    guassian_blur = noise_filter(image_path,"Guassian",kernel_size);
    imshow("guassian_blur",guassian_blur);
    mask = image - guassian_blur;
    cv::normalize(mask,scaled_mask,0,255,cv::NORM_MINMAX);
    imshow("scaled_mask",scaled_mask);
    unsharp_masked_image = image + mask_weightage * mask;
    return unsharp_masked_image;
}

Mat edge_enhancement(string image_path, string gradient="sobel")
{
    /*
    This function applies sobel gradient on the image and gives us the edges of the image
    on which we can clearly see the boundaries and mainly used in defect detection around edges
    image path: Takes the input path of the image stored
    gradient : Default sobel gradient is used
    */

    if (image_path == "")
        image_path = "/home/sriharsha.p/Image_Processing_Hands_on/Pictures/DIP3E_Original_Images_CH03/Fig0342(a)(contact_lens_original).tif";

    Mat image, pad_image, sobel_x, sobel_y;
    image = imread(image_path,0);
    image.convertTo(image,CV_32F);
        
    sobel_x = Mat::ones(3,3,CV_32F);
    sobel_x.row(0) = -1.0;
    sobel_x.row(1) = 0.0;
    sobel_x.at<float>(0,1) = -2.0;
    sobel_x.at<float>(2,1) = 2.0;
    
    sobel_y = sobel_x.t();
    
    copyMakeBorder(image, pad_image, 1, 1, 1, 1, BORDER_CONSTANT, cv::Scalar(0));

    Mat local_region;
    float gradient_x,gradient_y;
    
    for(int i=0; i< image.rows; i++)
    {
        for(int j=0; j< image.cols; j++)
        {  
            local_region = pad_image(cv::Rect(j, i, 3, 3));
            gradient_x = cv::sum( local_region.mul(sobel_x))[0];
            gradient_y = cv::sum(local_region.mul(sobel_y))[0];
            image.at<float>(i,j) = sqrt(pow(gradient_x,2) + pow(gradient_y,2));
        }
    }
            
    double min, max;
    cv::minMaxIdx(image, &min, &max);
    std::cout<<"maximum is : "<< max << " minimum is : "<< min<<endl;
    image = (image - min);
    image = image/(max - min);
    //image = image * 255;
    cv::minMaxIdx(image, &min, &max);
    std::cout<<"maximum is : "<< max << " minimum is : "<< min<<endl;
    return image;  
} 

int main()
{
    //string image_path = "/home/sriharsha.p/Image_Processing_Hands_on/Pictures/DIP3E_Original_Images_CH03/Fig0338(a)(blurry_moon).tif";
    //string image_path = "/home/sriharsha.p/Image_Processing_Hands_on/Pictures/softone.png";
    string image_path = "/home/sriharsha.p/Image_Processing_Hands_on/Pictures/DIP3E_Original_Images_CH03/Fig0342(a)(contact_lens_original).tif";

    Mat image,image_new;
    image = imread(image_path,0);
    
    clock_t start,end;

    start = clock();
    //image_new = sharpening_filter(image_path);
    //image_new = unsharp_masking(image_path,31,2);
    image_new = edge_enhancement("");
    end = clock();

    cout<<"Time taken in nanosecs is: "<<end-start<<endl;

    imshow("original_image",image);
    imshow("transformed_image",image_new);

    waitKey();
    return 0;
}