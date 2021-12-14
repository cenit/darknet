#!/bin/bash

# CONTAINER
#1) build_docker_container.sh
nvidia-docker build --no-cache -t thermal:darknet .

#2) run_docker_container.sh
docker run --runtime=nvidia -it --name darknet_thermal -v ~/Downloads/FLIR_ADAS_12_11_18:/home/Downloads thermal:darknet

#3) build darknet
make clean
make
make install

#4) edit preprocess_flir_dataset.sh to make sure folders are properly defined

#5) exit the container by using "Ctrl+P and Q". This leaves the container still running

#6) start training (edit start_training.sh to use the correct number of GPUs - 3 by default)
nvidia-docker exec -d darknet_thermal bash -c "cd /home/object-detection/ ; ./preprocess_flir_dataset.sh ; ./start_training.sh"
