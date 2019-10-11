#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Oct  9 19:18:11 2019

@author: sriharsha.p
"""

from PIL import Image
import numpy as np
import time

def noise_filter(image_path, kernel_type='Guassian', kernel_size=21, pad='zero'):
    '''
    This function is mainly used to remove the nosie from images
    image path: Takes the input path of the image stored
    kernel Type: Guassian,box,median
    Kernel size: size of the kernel which you want to apply to the image
    pad: type of padding to be applied on the image (zero,symmetric,reflect)
    '''
    start_time = time.time()
    image_array = np.array(Image.open(image_path))
    rows,columns = image_array.shape 
        
    if(pad == 'zero'):
        #pad_image =np.zeros((rows+kernel_size-1,columns+kernel_size-1),dtype='uint8')
        #pad_image[1:rows+1, 1:columns+1] = np.array(image_array)
        pad_size = int((kernel_size-1)/2)
        pad_image = np.pad(image_array, (pad_size,pad_size), 'constant', constant_values=0)
    if(pad == 'symmetric'):
        pad_size = int((kernel_size-1)/2)
        pad_image = np.pad(image_array,(pad_size,pad_size),'symmetric')
    if(pad == 'reflect'):
        pad_size = int((kernel_size-1)/2)
        pad_image = np.pad(image_array,(pad_size,pad_size),'reflect')
            
    if(kernel_type != 'median'):
        if( kernel_type == 'box'):
            kernel = np.ones((kernel_size, kernel_size))/(kernel_size**2)
        if( kernel_type == 'Guassian'):
            sigma = round(kernel_size/3.0)/2 #rounding to nearest 0.5
            #to get the x and y cordinates from which gaussian distribution is calculated
            x,y =  np.mgrid[-kernel_size//2 + 1:kernel_size//2 + 1, -kernel_size//2 + 1:kernel_size//2 + 1]
            #calculate the guassian values corresponding the x and y cordinates got earlier
            prenormalized_kernel = np.exp(-((x**2 + y**2)/(2.0*sigma**2)))
            #normalized kernal with sum of all the values inside the kernel
            kernel = prenormalized_kernel/prenormalized_kernel.sum()
        
    for i in range(0,rows):
        for j in range(0, columns):
            local_region = pad_image[i:i+kernel_size, j:j+kernel_size]
            image_array[i,j] = np.multiply(local_region, kernel).sum()
            
    else:
        for i in range(0,rows):
            for j in range(0, columns):
                local_region = pad_image[i:i+kernel_size, j:j+kernel_size]
                image_array[i,j] = np.median(local_region)
                
    transformed_image =  Image.fromarray(image_array)
    end_time = time.time()
    print("Total time taken in secs: ", end_time-start_time)
    transformed_image.show()
    return transformed_image             
