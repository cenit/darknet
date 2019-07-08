#!/bin/bash

#./darknet detector train data/thermal.data yolov3-spp-custom.cfg darknet53.conv.74 -gpus 3 -dont_show  -map >> ./train_results.txt
./darknet detector train data/thermal.data yolov3-spp-custom.cfg darknet53.conv.74 -dont_show  -map >> ./train_results.txt
