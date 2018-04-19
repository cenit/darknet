# -*- coding: utf-8 -*-
"""
Created on Tue Apr 17 19:46:36 2018

@author: nico
"""

import cv2
from scipy.spatial.distance import squareform, pdist
import ctypes as ct #from ctypes import *
import os, sys

class BOX(ct.Structure):
    _fields_ = [("x", ct.c_float),
                ("y", ct.c_float),
                ("w", ct.c_float),
                ("h", ct.c_float)]

class DETECTION(ct.Structure):
    _fields_ = [("bbox", BOX),
                ("classes", ct.c_int),
                ("prob", ct.POINTER(ct.c_float)),
                ("mask", ct.POINTER(ct.c_float)),
                ("objectness", ct.c_float),
                ("sort_class", ct.c_int)]


class IMAGE(ct.Structure):
    _fields_ = [("w", ct.c_int),
                ("h", ct.c_int),
                ("c", ct.c_int),
                ("data", ct.POINTER(ct.c_float))]

class METADATA(ct.Structure):
    _fields_ = [("classes", ct.c_int),
                ("names", ct.POINTER(ct.c_char_p))]



lib = ct.CDLL("libdarknet.so", ct.RTLD_GLOBAL)
lib.network_width.argtypes = [ct.c_void_p]
lib.network_width.restype = ct.c_int
lib.network_height.argtypes = [ct.c_void_p]
lib.network_height.restype = ct.c_int

get_network_boxes = lib.get_network_boxes
get_network_boxes.argtypes = [ct.c_void_p, ct.c_int, ct.c_int, ct.c_float, ct.c_float, ct.POINTER(ct.c_int), ct.c_int, ct.POINTER(ct.c_int)]
get_network_boxes.restype = ct.POINTER(DETECTION)

free_detections = lib.free_detections
free_detections.argtypes = [ct.POINTER(DETECTION), ct.c_int]

load_net = lib.load_network
load_net.argtypes = [ct.c_char_p, ct.c_char_p, ct.c_int]
load_net.restype = ct.c_void_p

do_nms_obj = lib.do_nms_obj
do_nms_obj.argtypes = [ct.POINTER(DETECTION), ct.c_int, ct.c_int, ct.c_float]

load_meta = lib.get_metadata
lib.get_metadata.argtypes = [ct.c_char_p]
lib.get_metadata.restype = METADATA

rgbgr_image = lib.rgbgr_image
rgbgr_image.argtypes = [IMAGE]

predict_image = lib.network_predict_image
predict_image.argtypes = [ct.c_void_p, IMAGE]
predict_image.restype = ct.POINTER(ct.c_float)

def c_array(ctype, values):
    arr = (ctype*len(values))()
    arr[:] = values
    return arr

def array_to_image(arr):
    arr = arr.transpose(2,0,1)
    c = arr.shape[0]
    h = arr.shape[1]
    w = arr.shape[2]
    arr = (arr/255.0).flatten()
    data = c_array(ct.c_float, arr)
    im = IMAGE(w,h,c,data)
    return im

def detect_frame(net, meta, image, thresh=.5, hier_thresh=.5, nms=.45):
    #im = load_image(image, 0, 0)
    num = ct.c_int(0)
    pnum = ct.pointer(num)
    predict_image(net, image)
    dets = get_network_boxes(net, image.w, image.h, thresh, hier_thresh, None, 0, pnum)
    num = pnum[0]
    if (nms): do_nms_obj(dets, num, meta.classes, nms);

    res = []
    for j in range(num):
        for i in range(meta.classes):
            if dets[j].prob[i] > thresh and meta.names[i] == b"person":
                res.append( (dets[j].bbox.x, dets[j].bbox.y, dets[j].bbox.w, dets[j].bbox.h) )
    free_detections(dets, num)
    return res

def detect_stream(ip_stream, thresh=.5, hier_thresh=.5, nms=.45, outfile=False):
    stream = cv2.VideoCapture(ip_stream)
    try:
        print "Search IP connection..."
        success, image = stream.read()
    except:
#        print("ERROR: IP address (%s) not responding."%ip_stream)
        raise OSError
#    print("Established!")
    net = load_net(b'cfg/yolov3.cfg', b"yolov3.weights", 0)
    meta = load_meta(b"cfg/coco.data")
    if outfile:
        outfile = ip_stream.split("//")[1].split("/")[0] + ".out"
#        with open(outfile, "w") as out_stream:
#            out_stream.write("frame,x,y\n")
#            frame = 0
#            while success:#True:
#                try:
#                    print("Search IP connection...", end="")
#                    success, image = stream.read()
#                except:
#                    print('Warning! IP address temporary down')
#                    continue
#                print("Established!")
#                print("Process frame %d"%frame)
#                image = array_to_image(image)
#                rgbgr_image(image)
#                coords = detect_frame(net, meta, image)
#                for coord in coords:
#                    out_stream.write("%d,%.3f,%.3f\n"%(frame, coord[0], coord[1]))
#                frame += 1
    else:
        image = array_to_image(image)
        rgbgr_image(image)
        #coords = [[x, y] for x,y,w,h in detect_frame(net, meta, image)]
        while True:
            try:
                print "Search IP connection..."
                success, image = stream.read()
            except cv2.error as e:
                print 'Warning! IP address temporary down'
                continue
#            print("Established!")
            cv2.imshow("office", image)


local = os.path.abspath(".")

ip_address = "rtsp://root:camera@131.154.12.187:554/rtpstream/config1=u"
outfile = ip_address.split("//")[1].split("@")[0] + ".out"
print "Output filename %s"%outfile
outfile = os.path.join(local, outfile)
detect_stream(ip_address, thresh=.5, outfile=False)