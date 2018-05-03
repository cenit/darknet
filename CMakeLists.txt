cmake_minimum_required(VERSION 3.1)
project(darknet)

enable_language(C)
set (CMAKE_C_STANDARD 11)

# C++ is required for CUDA activation, building and linking, if CUDA was not necessary C++ could be completely removed
enable_language(CXX)
set (CMAKE_CXX_STANDARD 14)


if(EXISTS "$ENV{WORKSPACE}/sysconfig/cmake/physycom_config.cmake")
   message(STATUS "Enabling physycom settings")
   include("$ENV{WORKSPACE}/sysconfig/cmake/physycom_config.cmake")
else()
   message(STATUS "Unable to find physycom settings file")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules/" ${CMAKE_MODULE_PATH})

set(CMAKE_SKIP_BUILD_RPATH  FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_SOURCE_DIR}")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if(MSVC)
  add_compile_options(/wd4244)
  add_compile_options(/wd4267)
  add_compile_options(/wd4305)
  add_compile_options(/wd4477)
  add_compile_options(/wd4996)
  #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRTD")
  #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT")
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

if(CMAKE_COMPILER_IS_GNUCC)
  add_compile_options(-Wno-unused-result)
  string(REGEX REPLACE "-O3" "-Ofast" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
endif()

if(APPLE)
  find_package(OpenCV PATHS /usr/local/opt NO_DEFAULT_PATH)
else()
  find_package(OpenCV)
endif()

if(OpenCV_FOUND)
  message(STATUS "OpenCV found: ${OpenCV_INCLUDE_DIRS}")
  add_definitions(-DOPENCV)
  include_directories(${OpenCV_INCLUDE_DIRS})
endif()

find_package(CUDA)
if(CUDA_FOUND)
  add_definitions(-DGPU)
  include_directories(${CUDA_INCLUDE_DIRS})
  if (MSVC)
    set(CUDA_NVCC_FLAGS "-gencode arch=compute_62,code=[sm_62,compute_62] --compiler-options /DGPU ")
  else()
    set(CUDA_NVCC_FLAGS "-gencode arch=compute_62,code=[sm_62,compute_62] --compiler-options \"-Wall -Wno-unused-result -Wno-unknown-pragmas -Wfatal-errors -fPIC -fopenmp -Ofast -DOPENCV -DGPU -DCUDNN\"")
  endif()
else()
  if(MSVC)
    set(CUDA_ADDITIONAL_BUILD_FILE ${CMAKE_CURRENT_LIST_DIR}/src/cuda.c)
  endif()
endif()

find_package(CUDNN)
if(CUDNN_FOUND)
  add_definitions(-DCUDNN)
  include_directories(${CUDNN_INCLUDE})
endif()

set(CMAKE_THREAD_PREFER_PTHREAD ON)
find_package(Threads REQUIRED)
if(MSVC)
  find_package(pthreads_windows REQUIRED)
  include_directories(${PTHREADS_INCLUDE_DIR})
  include_directories(${CMAKE_CURRENT_LIST_DIR}/win32)
else()
  add_definitions(-D_DEFAULT_SOURCE)
endif()

find_package(OpenMP)
if(OPENMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
endif()

list(APPEND DARKNET_SOURCE_FILES
  ${CMAKE_CURRENT_LIST_DIR}/examples/art.c
  ${CMAKE_CURRENT_LIST_DIR}/examples/attention.c
  ${CMAKE_CURRENT_LIST_DIR}/examples/captcha.c
  ${CMAKE_CURRENT_LIST_DIR}/examples/cifar.c
  ${CMAKE_CURRENT_LIST_DIR}/examples/classifier.c
  ${CMAKE_CURRENT_LIST_DIR}/examples/coco.c
  ${CMAKE_CURRENT_LIST_DIR}/examples/darknet.c
  ${CMAKE_CURRENT_LIST_DIR}/examples/detector.c
#  ${CMAKE_CURRENT_LIST_DIR}/examples/dice.c
  ${CMAKE_CURRENT_LIST_DIR}/examples/go.c
  ${CMAKE_CURRENT_LIST_DIR}/examples/lsd.c
  ${CMAKE_CURRENT_LIST_DIR}/examples/nightmare.c
  ${CMAKE_CURRENT_LIST_DIR}/examples/regressor.c
  ${CMAKE_CURRENT_LIST_DIR}/examples/rnn.c
  ${CMAKE_CURRENT_LIST_DIR}/examples/segmenter.c
  ${CMAKE_CURRENT_LIST_DIR}/examples/super.c
#  ${CMAKE_CURRENT_LIST_DIR}/examples/swag.c
  ${CMAKE_CURRENT_LIST_DIR}/examples/tag.c
#  ${CMAKE_CURRENT_LIST_DIR}/examples/voxel.c
#  ${CMAKE_CURRENT_LIST_DIR}/examples/writing.c
  ${CMAKE_CURRENT_LIST_DIR}/examples/yolo.c
  ${CUDA_ADDITIONAL_BUILD_FILE}
)

#file(GLOB YOLO_SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.c)
list(APPEND YOLO_SOURCE_FILES
  ${CMAKE_CURRENT_LIST_DIR}/src/activation_layer.c
  ${CMAKE_CURRENT_LIST_DIR}/src/activations.c
  ${CMAKE_CURRENT_LIST_DIR}/src/avgpool_layer.c
  ${CMAKE_CURRENT_LIST_DIR}/src/batchnorm_layer.c
  ${CMAKE_CURRENT_LIST_DIR}/src/blas.c
  ${CMAKE_CURRENT_LIST_DIR}/src/box.c
  ${CMAKE_CURRENT_LIST_DIR}/src/col2im.c
#  ${CMAKE_CURRENT_LIST_DIR}/src/compare.c
  ${CMAKE_CURRENT_LIST_DIR}/src/connected_layer.c
  ${CMAKE_CURRENT_LIST_DIR}/src/convolutional_layer.c
  ${CMAKE_CURRENT_LIST_DIR}/src/cost_layer.c
  ${CMAKE_CURRENT_LIST_DIR}/src/crnn_layer.c
  ${CMAKE_CURRENT_LIST_DIR}/src/crop_layer.c
  ${CMAKE_CURRENT_LIST_DIR}/src/cuda.c
  ${CMAKE_CURRENT_LIST_DIR}/src/data.c
  ${CMAKE_CURRENT_LIST_DIR}/src/deconvolutional_layer.c
  ${CMAKE_CURRENT_LIST_DIR}/src/demo.c
  ${CMAKE_CURRENT_LIST_DIR}/src/detection_layer.c
  ${CMAKE_CURRENT_LIST_DIR}/src/dropout_layer.c
  ${CMAKE_CURRENT_LIST_DIR}/src/gemm.c
  ${CMAKE_CURRENT_LIST_DIR}/src/gru_layer.c
  ${CMAKE_CURRENT_LIST_DIR}/src/im2col.c
  ${CMAKE_CURRENT_LIST_DIR}/src/image.c
  ${CMAKE_CURRENT_LIST_DIR}/src/l2norm_layer.c
  ${CMAKE_CURRENT_LIST_DIR}/src/layer.c
  ${CMAKE_CURRENT_LIST_DIR}/src/list.c
  ${CMAKE_CURRENT_LIST_DIR}/src/local_layer.c
  ${CMAKE_CURRENT_LIST_DIR}/src/logistic_layer.c
  ${CMAKE_CURRENT_LIST_DIR}/src/lstm_layer.c
  ${CMAKE_CURRENT_LIST_DIR}/src/matrix.c
  ${CMAKE_CURRENT_LIST_DIR}/src/maxpool_layer.c
  ${CMAKE_CURRENT_LIST_DIR}/src/network.c
  ${CMAKE_CURRENT_LIST_DIR}/src/normalization_layer.c
  ${CMAKE_CURRENT_LIST_DIR}/src/option_list.c
  ${CMAKE_CURRENT_LIST_DIR}/src/parser.c
  ${CMAKE_CURRENT_LIST_DIR}/src/region_layer.c
  ${CMAKE_CURRENT_LIST_DIR}/src/reorg_layer.c
  ${CMAKE_CURRENT_LIST_DIR}/src/rnn_layer.c
  ${CMAKE_CURRENT_LIST_DIR}/src/route_layer.c
  ${CMAKE_CURRENT_LIST_DIR}/src/shortcut_layer.c
  ${CMAKE_CURRENT_LIST_DIR}/src/softmax_layer.c
  ${CMAKE_CURRENT_LIST_DIR}/src/tree.c
  ${CMAKE_CURRENT_LIST_DIR}/src/upsample_layer.c
  ${CMAKE_CURRENT_LIST_DIR}/src/utils.c
  ${CMAKE_CURRENT_LIST_DIR}/src/yolo_layer.c
)

#file(GLOB YOLO_HEADER_FILES ${CMAKE_SOURCE_DIR}/src/*.h)
list(APPEND YOLO_HEADER_FILES
  ${CMAKE_CURRENT_LIST_DIR}/src/activation_layer.h
  ${CMAKE_CURRENT_LIST_DIR}/src/activations.h
  ${CMAKE_CURRENT_LIST_DIR}/src/avgpool_layer.h
  ${CMAKE_CURRENT_LIST_DIR}/src/batchnorm_layer.h
  ${CMAKE_CURRENT_LIST_DIR}/src/blas.h
  ${CMAKE_CURRENT_LIST_DIR}/src/box.h
  ${CMAKE_CURRENT_LIST_DIR}/src/classifier.h
  ${CMAKE_CURRENT_LIST_DIR}/src/col2im.h
  ${CMAKE_CURRENT_LIST_DIR}/src/connected_layer.h
  ${CMAKE_CURRENT_LIST_DIR}/src/convolutional_layer.h
  ${CMAKE_CURRENT_LIST_DIR}/src/cost_layer.h
  ${CMAKE_CURRENT_LIST_DIR}/src/crnn_layer.h
  ${CMAKE_CURRENT_LIST_DIR}/src/crop_layer.h
  ${CMAKE_CURRENT_LIST_DIR}/src/cuda.h
  ${CMAKE_CURRENT_LIST_DIR}/src/data.h
  ${CMAKE_CURRENT_LIST_DIR}/src/deconvolutional_layer.h
  ${CMAKE_CURRENT_LIST_DIR}/src/demo.h
  ${CMAKE_CURRENT_LIST_DIR}/src/detection_layer.h
  ${CMAKE_CURRENT_LIST_DIR}/src/dropout_layer.h
  ${CMAKE_CURRENT_LIST_DIR}/src/gemm.h
  ${CMAKE_CURRENT_LIST_DIR}/src/gru_layer.h
  ${CMAKE_CURRENT_LIST_DIR}/src/im2col.h
  ${CMAKE_CURRENT_LIST_DIR}/src/image.h
  ${CMAKE_CURRENT_LIST_DIR}/src/l2norm_layer.h
  ${CMAKE_CURRENT_LIST_DIR}/src/layer.h
  ${CMAKE_CURRENT_LIST_DIR}/src/list.h
  ${CMAKE_CURRENT_LIST_DIR}/src/local_layer.h
  ${CMAKE_CURRENT_LIST_DIR}/src/logistic_layer.h
  ${CMAKE_CURRENT_LIST_DIR}/src/lstm_layer.h
  ${CMAKE_CURRENT_LIST_DIR}/src/matrix.h
  ${CMAKE_CURRENT_LIST_DIR}/src/maxpool_layer.h
  ${CMAKE_CURRENT_LIST_DIR}/src/network.h
  ${CMAKE_CURRENT_LIST_DIR}/src/normalization_layer.h
  ${CMAKE_CURRENT_LIST_DIR}/src/option_list.h
  ${CMAKE_CURRENT_LIST_DIR}/src/parser.h
  ${CMAKE_CURRENT_LIST_DIR}/src/region_layer.h
  ${CMAKE_CURRENT_LIST_DIR}/src/reorg_layer.h
  ${CMAKE_CURRENT_LIST_DIR}/src/rnn_layer.h
  ${CMAKE_CURRENT_LIST_DIR}/src/route_layer.h
  ${CMAKE_CURRENT_LIST_DIR}/src/shortcut_layer.h
  ${CMAKE_CURRENT_LIST_DIR}/src/softmax_layer.h
  ${CMAKE_CURRENT_LIST_DIR}/src/stb_image.h
  ${CMAKE_CURRENT_LIST_DIR}/src/stb_image_write.h
  ${CMAKE_CURRENT_LIST_DIR}/src/tree.h
  ${CMAKE_CURRENT_LIST_DIR}/src/upsample_layer.h
  ${CMAKE_CURRENT_LIST_DIR}/src/utils.h
  ${CMAKE_CURRENT_LIST_DIR}/src/yolo_layer.h
)

if(MSVC)
  list(APPEND DARKNET_SOURCE_FILES
    ${YOLO_SOURCE_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/win32/gettimeofday.c
    ${CMAKE_CURRENT_LIST_DIR}/win32/clock_gettime.c
    ${CMAKE_CURRENT_LIST_DIR}/win32/getopt.c
  )
  list(APPEND DARKNET_HEADER_FILES
    ${YOLO_HEADER_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/include/darknet.h
    ${CMAKE_CURRENT_LIST_DIR}/win32/getopt.h
    ${CMAKE_CURRENT_LIST_DIR}/win32/timeutils.h
    ${CMAKE_CURRENT_LIST_DIR}/win32/unistd.h
  )
  list(APPEND YOLO_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/win32/gettimeofday.c
    ${CMAKE_CURRENT_LIST_DIR}/win32/clock_gettime.c
    ${CMAKE_CURRENT_LIST_DIR}/win32/getopt.c
  )
  list(APPEND YOLO_HEADER_FILES
    ${CMAKE_CURRENT_LIST_DIR}/win32/getopt.h
    ${CMAKE_CURRENT_LIST_DIR}/win32/timeutils.h
    ${CMAKE_CURRENT_LIST_DIR}/win32/unistd.h
  )
endif()

file(GLOB CUDA_SOURCE_FILES ${CMAKE_CURRENT_LIST_DIR}/src/*.cu)

include_directories(${CMAKE_CURRENT_LIST_DIR}/src)
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

add_library(darknet_lib SHARED ${YOLO_SOURCE_FILES} ${YOLO_HEADER_FILES})
set_property(TARGET darknet_lib PROPERTY POSITION_INDEPENDENT_CODE ON)
add_executable(darknet ${DARKNET_SOURCE_FILES} ${DARKNET_HEADER_FILES})

list(APPEND linked_libs ${CMAKE_THREAD_LIBS_INIT})
if(CUDA_FOUND)
  CUDA_ADD_LIBRARY(darknet_cuda STATIC ${CUDA_SOURCE_FILES})
  list(APPEND linked_libs ${CUDA_LIBRARIES} ${CUDA_CUBLAS_LIBRARIES} ${CUDA_curand_LIBRARY})
  list(APPEND additional_linked_libs darknet_cuda)
  set_property(TARGET darknet_cuda PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()

if(CUDNN_FOUND)
  list(APPEND linked_libs ${CUDNN_LIBRARY})
endif()

if(OpenCV_FOUND)
  list(APPEND linked_libs ${OpenCV_LIBRARIES})
endif()

if(OPENMP_FOUND AND APPLE)
  list(APPEND linked_libs OpenMP::OpenMP_C)
endif()

if(CMAKE_COMPILER_IS_GNUCC)
  list(APPEND linked_libs m)
endif()

if(MSVC)
  list(APPEND linked_libs ${PTHREADS_LIBRARIES} wsock32 ws2_32)
endif()

if(CUDA_FOUND)
  target_link_libraries(darknet_cuda ${linked_libs})
endif()

target_link_libraries(darknet_lib ${linked_libs} ${additional_linked_libs})
if(MSVC)
  target_link_libraries(darknet ${linked_libs} ${additional_linked_libs})
else()
  target_link_libraries(darknet darknet_lib ${linked_libs} ${additional_linked_libs})
endif()

install(TARGETS darknet_lib DESTINATION ${CMAKE_CURRENT_LIST_DIR})
install(TARGETS darknet_lib DESTINATION ${CMAKE_SOURCE_DIR})
install(TARGETS darknet DESTINATION ${CMAKE_CURRENT_LIST_DIR})
if(CUDA_FOUND)
  install(TARGETS darknet_cuda DESTINATION ${CMAKE_SOURCE_DIR})
endif()
