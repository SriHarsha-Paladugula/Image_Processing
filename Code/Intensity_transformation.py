#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Sep 25 18:03:59 2019

@author: sriharsha.p
"""

from PIL import Image
import numpy as np
import matplotlib.pyplot as plt
import time

def intensity_transformation(image_path, transformation_type, power=0.25):
    '''
    This function is used to apply the transformation_type mentioned 
    to the image on the path specified and will display the image 
    after tranformation
    
    image_path : should provide the path of the image file on which 
    you wish to see the transformation
    
    transformation: should provide the type of transformation you wish
    to see applied on the image
    '''
    image_array = np.array(Image.open(image_path))
    
    if(transformation_type == 'negative'):
        '''
        neagtive image can be obtained formulae 'L-1-r'
        where L-1 is the intensity levels in the range [0, L-1]
        r is the intensity level of the pixel
        '''
        negative_image = -image_array-1
        transformed_image = Image.fromarray(negative_image)
        return transformed_image.show()
    
    if(transformation_type == 'log'):
        log_transformation = np.log(image_array)
        #changing the pixel values of -inf to zero as log of zero is undefined
        log_transformation[np.isneginf(log_transformation)] = 0
        #changing the scale of transformation to 255
        log_transform_scaling = (log_transformation * (255/np.log(255))).astype('uint8')
        transformed_image = Image.fromarray(log_transform_scaling)
        return transformed_image.show()
    
    if(transformation_type == 'gamma'):
        gamma_transformation = np.power(image_array, power)
        #changing the scale of transformation to 255
        gamma_transform_scaling = (gamma_transformation * (255/np.amax(gamma_transformation))).astype('uint8')
        transformed_image = Image.fromarray(gamma_transform_scaling)
        return transformed_image.show()
    
def contrast_stretching(image_path, r1, s1, r2, s2):
    '''
    The locations of points (r1,s1) and (r2,s2) control the shape of the transformation function.
    If r1=s1 and r2=s2 the transformation is a linear function that produces no changes in intensity.
    If r1=r2 and s1=0 and s2=L-1 the transformation becomes a thresholding function that creates a binary image.
    Intermediate values of (r1, s1) and (r2, s2) produce various degrees of spread in the intensity levels 
    of the output image, thus affecting its contrast
    '''
    image_array = np.array(Image.open(image_path))
    new_image_array = image_array.copy()
    
    new_image_array[image_array<r1] = new_image_array[image_array<r1]*int(s1/r1)
    
    new_image_array[np.logical_and(image_array >= s1 ,image_array <= s2)] =\
    (((new_image_array[np.logical_and(image_array >= s1, image_array <= s2)]-r1)*int((s2-s1)/ (r2-r1))) + s1)
    
    new_image_array[image_array>r2] = ((new_image_array[image_array>r2] - r2) * int(255-s2/255-r2)) + s2
    
    transformed_image = Image.fromarray(new_image_array)
    return transformed_image.show()

def intensity_level_slicing(image_path, range_of_interest, binary = False):
    '''
    image_path : should provide the path of the image file on which 
    you wish to see the transformation
    
    range_of_interest: is the start and end of the pixel range you want to preserve
    during intensity level slicing given in form of array eg: [85, 145] for kidney image
    
    binary: True when you want a binary image as output
    
    '''
    image_array = np.array(Image.open(image_path))

    if(binary):
        image_array[image_array<range_of_interest[0]] = 0
        image_array[image_array<range_of_interest[1]] = 0
        #image_array[image_array!=0] = 255
        transformed_image = Image.fromarray(image_array)
    else :
        image_array[np.logical_and(image_array >= range_of_interest[0] ,image_array <= range_of_interest[1])] =\
        image_array[np.logical_and(image_array >= range_of_interest[0] ,image_array <= range_of_interest[1])]=0
        transformed_image = Image.fromarray(image_array)
    return transformed_image.show()

def get_histogram(image, bins=256):
    # array with size of bins, set to zeros
    histogram = np.zeros(bins)
    
    # loop through pixels and sum up counts of pixels
    for pixel in image:
        histogram[pixel] += 1
    
    # return our final result
    return histogram

def cumsum(hist):
    a = iter(hist)
    b = [next(a)]
    for i in a:
       b.append(b[-1] + i)
    return np.array(b)
    
def histogram_equalization(image_path):
    image_array = np.array(Image.open(image_path))
    flat = image_array.flatten()
    hist = get_histogram(flat,256)
    fig = plt.figure(figsize=(8,4))
    
    fig.add_subplot(1, 2, 1)
    plt.title('Histogram Before Equalization')
    plt.xlabel('Pixel Intensity level')
    plt.ylabel('No. of pixels with intensity level')
    plt.plot(hist)
      
    cumulative_sum = cumsum(hist)
    
    # re-normalize cumsum values to be between 0-255

    # numerator & denomenator
    nj = (cumulative_sum - cumulative_sum.min()) * 255
    N = cumulative_sum.max() - cumulative_sum.min()

    # re-normalize the cdf
    cumulative_sum = nj / N
    cumulative_sum = cumulative_sum.astype('uint8')
    
    # get the value from cumulative sum for every index in flat, and set that as img_new
    img_new = cumulative_sum[flat]
    fig.add_subplot(1, 2, 2)
    plt.title('Histogram after Equalization')
    plt.xlabel('Pixel Intensity level')
    plt.ylabel('No. of pixels with intensity level')
    plt.plot(get_histogram(img_new))
    
    img_new = np.reshape(img_new, image_array.shape)
    
    transformed_image = Image.fromarray(img_new)
    return transformed_image.show()

image_path='/home/sriharsha.p/Image_Processing_Hands_on/Pictures/DIP3E_Original_Images_CH03/Fig0326(a)(embedded_square_noisy_512).tif'

def local_histogram_equalization(image_path=image_path, kernel_size=3, C=22.8, k0=0, k1=0.1, k2=0, k3=0.1):
    '''
    Let f(x, y) denote the value of an image at any image coordinates (x, y), and let
    g(x, y) be the corresponding value in the enhanced image at those coordinates. Then,
    
    if k0*global_mean <= local_mean at (x,y) <= k1*global_mean and k2*global_std <= local_std at (x,y) <= k3*global_std
        g(x,y) = C*f(x,y) 
    else
        g(x,y) = f(x,y) 
        
    image_path: '/home/sriharsha.p/Image_Processing_Hands_on/Pictures/DIP3E_Original_Images_CH03/Fig0326(a)(embedded_square_noisy_512).tif'    
    '''
    start_time = time.time()
    image_array = np.array(Image.open(image_path))
    rows,columns = image_array.shape
    global_mean = round(image_array.mean())
    global_std  = round(image_array.std())
    
    pad_image =np.zeros((rows+kernel_size-1,columns+kernel_size-1),dtype='uint8')
    pad_image[1:rows+1, 1:columns+1] = np.array(image_array)
    
    for i in range(1,rows):
        for j in range(1, columns):
            local_region = pad_image[i:i+kernel_size, j:j+kernel_size]
            local_mean = round(local_region.mean())
            local_std  = round(local_region.std())
            mean_threshold = False
            std_threshold = False
            if (k0*global_mean <= local_mean <= k1*global_mean):
                mean_threshold = True
            if (k2*global_std <= local_std <= k3*global_std):
                std_threshold = True    
            if(mean_threshold and std_threshold):
                image_array[i,j] = image_array[i,j] * C
            
    transformed_image =  Image.fromarray(image_array)
    end_time = time.time()
    print("Total time taken in secs: ", end_time-start_time)
    return transformed_image.show()      
   
         
            
    