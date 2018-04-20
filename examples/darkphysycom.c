#include "darknet.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef _WIN32
#include "timeutils.h"
#endif

extern void run_detector(int argc, char** argv);

#define ARGC 7
#define ARGL 200

int main()
{
  int argc = ARGC;
  char** argv;
  int i;
  argv = (char**) malloc(argc*sizeof(char*));
  for(i=0; i<argc; ++i) argv[i] = (char*) malloc(ARGL*sizeof(char));
  
  argv[0] = "./darkphysycom";
  argv[1] = "detector";
  argv[2] = "demo";
  argv[3] = "cfg/coco.data";
  argv[4] = "cfg/yolov3.cfg";
  argv[5] = "yolov3.weights";
  argv[6] = "rtsp://root:camera@131.154.10.192:554/rtpstream/config1=u";

  run_detector(argc, argv);

  return 0;
}
