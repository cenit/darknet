#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "image.h"

static char** demo_names;
static int demo_classes;

static network* net;
static image buff[3];
static image buff_letter[3];
static int buff_index = 0;
static CvCapture* cap;
static float fps = 0;
static float demo_thresh = 0;
static float demo_hier = .5;

static int demo_frame = 3;
static int demo_index = 0;
static float** predictions;
static float* avg;
static int demo_done = 0;
static int demo_total = 0;
double demo_time;

// physycom add
#define MAX_FRAME_INFO_TO_STORE 50
#define FRAME_NUMBER_TRACKING 5
#define MAX_LINE_LEN          20
#define ERR_NO_FILE           111
#define ERR_NO_STREAM         222

#define PUNCTILIOUS

#include "peoplebox_params.h"

static int person_name_index;
static char person_label[] = "person";
static int frame_num;

static int infos_index;

typedef struct frame_info{
  double timestamp;
  unsigned int person_number, frame_number;
} frame_info;
static frame_info *infos;

static double tnow;
static time_t tnow_t;
static struct tm *tm_tnow;
static char human_timenow[MAX_LINE_LEN];
static char json_name[200];

void* fetch_in_thread(void* ptr)
{
  int status = fill_image_from_stream(cap, buff[buff_index]);
  letterbox_image_into(buff[buff_index], net->w, net->h, buff_letter[buff_index]);
  if (status == 0)
    demo_done = 1;
  return 0;
}

void* detect_in_thread(void* ptr)
{
  float nms = .4;
  int i, j, count = 0, nboxes = 0, person_num = 0;
  
  layer l = net->layers[net->n - 1];
  network_predict(net, buff_letter[(buff_index + 2) % 3].data);

  for (i = 0; i < net->n; ++i) {
    layer l = net->layers[i];
    if (l.type == YOLO || l.type == REGION || l.type == DETECTION) {
      memcpy(predictions[demo_index] + count, net->layers[i].output, sizeof(float) * l.outputs);
      count += l.outputs;
    }
  }

  count = 0;
  fill_cpu(demo_total, 0, avg, 1);
  for (j = 0; j < demo_frame; ++j)
    axpy_cpu(demo_total, 1. / demo_frame, predictions[j], 1, avg, 1);
  for (i = 0; i < net->n; ++i) {
    layer l = net->layers[i];
    if (l.type == YOLO || l.type == REGION || l.type == DETECTION) {
      memcpy(l.output, avg + count, sizeof(float) * l.outputs);
      count += l.outputs;
    }
  }
  detection *dets = get_network_boxes(net, buff[0].w, buff[0].h, demo_thresh, demo_hier, 0, 1, &nboxes);

  if (nms > 0)
    do_nms_obj(dets, nboxes, l.classes, nms);

	tnow = what_time_is_it_now();
	tnow_t = (time_t)tnow;
	tm_tnow = localtime(&tnow_t);
	strftime (human_timenow,MAX_LINE_LEN,"%D %X",tm_tnow);

  /* people tracking */
  for (i = 0; i < nboxes; ++i)
    if (dets[i].prob[person_name_index] > demo_thresh)
      ++person_num;	

  ++frame_num;
  
#ifdef PUNCTILIOUS
  printf("\033[2J");     // clear the screen
  printf("\033[1;1H");   // move cursor to line 1 column 1
  printf("UNIX  Time   : %.3f\n",tnow);
  printf("Human Time   : %s\n",human_timenow);
  printf("Frame Number : %d\n",frame_num);
  printf("Puny humans  : %d\n",person_num);
  printf("FPS          : %.1f\n",fps);
#endif
  
  infos[infos_index].timestamp = tnow;
  infos[infos_index].person_number = person_num;
  infos[infos_index].frame_number = frame_num;
  ++infos_index;
  if(frame_num%MAX_FRAME_INFO_TO_STORE==0)
  {
    sprintf(json_name,"output/frame_info.%ld.json",tnow_t);
    FILE *info_json = fopen(json_name, "w");
    fprintf(info_json,"{\n");
    for(i=0; i<MAX_FRAME_INFO_TO_STORE; ++i){
      fprintf(info_json,"\t\"frame_%d\" : {\n",infos[i].frame_number);
      fprintf(info_json,"\t\t\"timestamp\" : %.3f,\n", infos[i].timestamp);      
      fprintf(info_json,"\t\t\"id_box\" : \"%s\",\n", PEOPLEBOX_ID);  
      fprintf(info_json,"\t\t\"detection\" : \"%s\",\n", DETECTION_TRACK);  
      fprintf(info_json,"\t\t\"sw_ver\" : %s,\n", SW_VER_TRACK);
      fprintf(info_json,"\t\t\"people_count\" : [{\"id\" : \"%s\", \"count\" : %d}],\n", ROI_ID, infos[i].person_number);
      fprintf(info_json,"\t\t\"diagnostics\" : [{\"id\" : \"coming\", \"value\" : \"soon\"}]\n");      
      (i!=MAX_FRAME_INFO_TO_STORE-1) ? fprintf(info_json,"\t},\n") : fprintf(info_json,"\t}\n");
    }
    fprintf(info_json,"}");
    fclose(info_json);
    infos_index = 0;
    memset(infos, 0, MAX_FRAME_INFO_TO_STORE*sizeof(frame_info));
    printf("Info dumped to JSON\n");
  }

  free_detections(dets, nboxes);
  demo_index = (demo_index + 1) % demo_frame;
  return 0;
}

int main()
{
  int i;
  char* cfg = "cfg/yolov3.cfg";
  char* weights = "yolov3.weights";
  char* filename = "rtsp://root:camera@131.154.10.192:554/rtpstream/config1=u";
  char* name_list = "data/coco.names";

  demo_thresh = .5;
  demo_hier = .5;
  demo_classes = 80;
  
  /* prepare frame info array */
  infos = (frame_info*)malloc(MAX_FRAME_INFO_TO_STORE*sizeof(frame_info));

/*
  // PARSE FILE LINE BY LINE
  char **lines, line[MAX_LINE_LEN];
  int line_num=0;
  FILE *infile = fopen(name_list, "r");
  while( fgets(line, MAX_LINE_LEN, infile) != NULL ) ++line_num;
  rewind(infile);
  printf("Line number : %d\n", line_num);
  lines = (char**) malloc(line_num*sizeof(char*));
  for(i=0; i<line_num; ++i)
  {
    lines[i] = (char*) malloc(MAX_LINE_LEN*sizeof(char));
    fgets(lines[i], MAX_LINE_LEN, infile);
    lines[i][strlen(lines[i])-1] = '\0';
  }
  for(i=0; i<line_num; ++i) printf("Line : %s\n", lines[i]);
  exit(222);
*/

  FILE *file = fopen(name_list, "r");
  if(!file) { fprintf(stderr, "cannot open: %s",name_list); exit(ERR_NO_FILE); }
  demo_names = (char**) malloc(demo_classes*sizeof(char*));
  for(i=0; i<demo_classes; ++i) 
  {
    demo_names[i] = fgetl(file);
    if(!strcmp(demo_names[i], person_label)) person_name_index = i;
  }
  fclose(file);

  net = load_network(cfg, weights, 0);
  set_batch_network(net, 1);
  pthread_t detect_thread;
  pthread_t fetch_thread;

  demo_total = 0;
  for (i = 0; i < net->n; ++i) {
    layer l = net->layers[i];
    if (l.type == YOLO || l.type == REGION || l.type == DETECTION) {
      demo_total += l.outputs;
    }
  }

  predictions = (float**)calloc(demo_frame, sizeof(float*));
  for (i = 0; i < demo_frame; ++i)
    predictions[i] = (float*)calloc(demo_total, sizeof(float));
  
  avg = (float*)calloc(demo_total, sizeof(float));

  printf("video stream: %s\n", filename);
  cap = cvCaptureFromFile(filename);

  if (!cap) {
    fprintf(stderr, "cannot connect to video stream.\n");
    exit(ERR_NO_STREAM);
  }

  buff[0] = get_image_from_stream(cap);
  buff[1] = copy_image(buff[0]);
  buff[2] = copy_image(buff[0]);
  buff_letter[0] = letterbox_image(buff[0], net->w, net->h);
  buff_letter[1] = letterbox_image(buff[0], net->w, net->h);
  buff_letter[2] = letterbox_image(buff[0], net->w, net->h);

  i = 0;
  demo_time = what_time_is_it_now();
  while (!demo_done) {
    buff_index = (buff_index + 1) % 3;
    if (pthread_create(&fetch_thread, 0, fetch_in_thread, 0))
      error("thread creation failed");
    if (pthread_create(&detect_thread, 0, detect_in_thread, 0))
      error("thread creation failed");
        
    fps = 1. / (what_time_is_it_now() - demo_time);
    demo_time = what_time_is_it_now();
    
    pthread_join(fetch_thread, 0);
    pthread_join(detect_thread, 0);

    ++i;
  }

  // delete pointers
  for(i=0; i<demo_classes; ++i) free(demo_names[i]);
  free(demo_names);

  return 0;
}

