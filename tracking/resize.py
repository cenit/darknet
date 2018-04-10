# -*- coding: utf-8 -*-
"""
Created on Sun Apr  8 15:13:54 2018

@author: NICO
"""

import os
import cv2

local = os.path.abspath(".")
images_dir = "outdoor"
new_dir = "mini_outdoor"

files = (f
         for f in os.listdir( os.path.join(local, images_dir)) 
         if os.path.isfile(os.path.join(local, images_dir, f))
         )
for f in files:
	print("Process %s"%f)
	im = cv2.imread(os.path.join(local, images_dir, f))
	im = cv2.resize(im, (500, 375))
	cv2.imwrite(os.path.join(local, new_dir, f), im)
