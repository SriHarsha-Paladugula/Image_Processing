#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Oct 10 14:00:51 2019

@author: sriharsha.p
"""

from PIL import Image
import numpy as np
import time
import low_pass_filter as lp


def sharpening_filter(image_path, kernel_type='laplacian_with_diagonals', kernel_size=3, pad='zero'):
    '''
    This function is mainly used to sharpen images
    image path: Takes the input path of the image stored
    kernel Type: laplacian_with_diagonal elements filled/without filled
    Kernel size: size of the kernel which you want to apply to the image
    pad: type of padding to be applied on the image (zero,symmetric,reflect)
    '''
    start_time = time.time()
    image_array = np.array(Image.open(image_path)).astype(np.int32)
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
        
    if(kernel_type=='laplacian_with_diagonals'):
        center = int(kernel_size/2)
        kernel = np.ones((kernel_size,kernel_size))
        kernel[center,center] = kernel[center,center] * (-1) * (kernel_size**2 -1)
        kernel = kernel/8
        
    for i in range(0,rows):
        for j in range(0, columns):
            local_region = pad_image[i:i+kernel_size, j:j+kernel_size]
            # image_array[i,j] = np.abs(np.multiply(local_region,kernel).sum())
            image_array[i,j] = image_array[i,j]-(np.multiply(local_region,kernel).sum())
    
    print("min is: ",image_array.min(),"max is: ",image_array.max())       
    transformed_image =  Image.fromarray(image_array)
    end_time = time.time()
    print("Total time taken in secs: ", end_time-start_time)
    transformed_image.show()
    return transformed_image        

def unsharp_masking(image_path, kernel_size=13, mask_weightage=1):
    '''
    This function is mainly used to sharpen images by adding a mask to the given image
    The mask is obtained by subratcing the image with image obtained using guassian blur
    image path: Takes the input path of the image stored
    kernel-size: guassian kernel size to be used
    mask_weightage: when addinng the original image with mask how much weightage mask should
                    have on the resultant image
    '''
    start_time = time.time()
    image_array = np.array(Image.open(image_path)).astype(np.float32)
    guassian_blur = lp.noise_filter(image_path,'Guassian',kernel_size)
    mask = image_array - guassian_blur
    scaled_mask = ((mask - mask.min())/mask.max())*255
    scaled_mask =  Image.fromarray(scaled_mask)
    scaled_mask.show()
    unsharp_masked_image = image_array + mask_weightage * mask
    transformed_image =  Image.fromarray(unsharp_masked_image)
    end_time = time.time()
    print("Total time taken in secs: ", end_time-start_time)
    transformed_image.show()
    return transformed_image

def edge_enhancement(image_path, gradient='sobel'):
    '''
    This function applies sobel gradient on the image and gives us the edges of the image
    on which we can clearly see the boundaries and mainly used in defect detection around edges
    image path: Takes the input path of the image stored
    gradient : Default sobel gradient is used
    '''

    start_time = time.time()
    image_array = np.array(Image.open(image_path)).astype(np.float32)
    rows,columns = image_array.shape
        
    sobel_x = np.ones((3,3))
    sobel_x[0,:]=-1
    sobel_x[1,:]=0
    sobel_x[0,1] = -2
    sobel_x[2,1]=2
    
    sobel_y = np.transpose(sobel_x)
    
    
    
    pad_image = np.pad(image_array, (1,1), 'constant', constant_values=0)
    
    for i in range(0,rows):
        for j in range(0, columns):
            local_region = pad_image[i:i+3, j:j+3]
            convolution_x = np.multiply(local_region,sobel_x)
            gradient_x = convolution_x.sum()
            convolution_y = np.multiply(local_region,sobel_y)
            gradient_y = convolution_y.sum()
            image_array[i,j] = np.sqrt(gradient_x**2 + gradient_y**2)
            
    print(image_array.min(), image_array.max())
    image_array = 255*((image_array - image_array.min())/image_array.max())
    print(image_array.min(), image_array.max())
    transformed_image =  Image.fromarray(image_array)
    end_time = time.time()
    print("Total time taken in secs: ", end_time-start_time)
    transformed_image.show()  
    return transformed_image     
    
#sharpening_filter('/home/sriharsha.p/Image_Processing_Hands_on/Pictures/DIP3E_Original_Images_CH03/Fig0338(a)(blurry_moon).tif')
#unsharp_masking('/home/sriharsha.p/Image_Processing_Hands_on/Pictures/softone.png',mask_weightage=1)
#unsharp_masking('/home/sriharsha.p/Image_Processing_Hands_on/Pictures/softone.png',mask_weightage=2)
#unsharp_masking('/home/sriharsha.p/Image_Processing_Hands_on/Pictures/softone.png',mask_weightage=3)
#edge_enhancement('/home/sriharsha.p/Image_Processing_Hands_on/Pictures/DIP3E_Original_Images_CH03/Fig0342(a)(contact_lens_original).tif')