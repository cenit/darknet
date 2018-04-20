cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)
project(darknet)

if(EXISTS "$ENV{WORKSPACE}/sysconfig/cmake/physycom_config.cmake")
   message(STATUS "Enabling physycom settings")
   include("$ENV{WORKSPACE}/sysconfig/cmake/physycom_config.cmake")
else()
   message(STATUS "Unable to find physycom settings file")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/" ${CMAKE_MODULE_PATH})

include(CheckFunctionExists)

if(MSVC)
  add_compile_options(/wd4244)
  add_compile_options(/wd4267)
  add_compile_options(/wd4305)
  add_compile_options(/wd4477)
  add_compile_options(/wd4996)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

if(CMAKE_COMPILER_IS_GNUCC)
  add_compile_options(-Wno-unused-result)
endif()

if(APPLE)
  find_package(OpenCV PATHS /usr/local/opt NO_DEFAULT_PATH)
  if(OpenCV_FOUND)
    message(STATUS "OpenCV found: ${OpenCV_INCLUDE_DIRS}")
  endif()
else()
  find_package(OpenCV)
endif()

if(OpenCV_FOUND)
  add_definitions(-DOPENCV)
  include_directories(${OpenCV_INCLUDE_DIRS})
endif()

find_package(CUDA)
if(CUDA_FOUND)
  add_definitions(-DGPU)
  include_directories(${CUDA_INCLUDE_DIRS})
  list(APPEND CUDA_NVCC_FLAGS "-Xcompiler -fPIC")
  list(APPEND CUDA_NVCC_FLAGS "-O3")
  list(APPEND CUDA_NVCC_FLAGS "-gencode arch=compute_62,code=[sm_62,compute_62]")
else()
  if(MSVC)
    set(CUDA_ADDITIONAL_BUILD_FILE ${CMAKE_SOURCE_DIR}/src/cuda.c)
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
  include_directories(${CMAKE_SOURCE_DIR}/win32)
else()
  add_definitions(-D_DEFAULT_SOURCE)
endif()

find_package(OpenMP)
if(OPENMP_FOUND)
  add_definitions(-DOPENMP)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

list(APPEND DARKNET_SOURCE_FILES
  ${CMAKE_SOURCE_DIR}/examples/art.c
  ${CMAKE_SOURCE_DIR}/examples/attention.c
  ${CMAKE_SOURCE_DIR}/examples/captcha.c
  ${CMAKE_SOURCE_DIR}/examples/cifar.c
  ${CMAKE_SOURCE_DIR}/examples/classifier.c
  ${CMAKE_SOURCE_DIR}/examples/coco.c
  ${CMAKE_SOURCE_DIR}/examples/darknet.c
  ${CMAKE_SOURCE_DIR}/examples/detector.c
#  ${CMAKE_SOURCE_DIR}/examples/dice.c
  ${CMAKE_SOURCE_DIR}/examples/go.c
  ${CMAKE_SOURCE_DIR}/examples/lsd.c
  ${CMAKE_SOURCE_DIR}/examples/nightmare.c
  ${CMAKE_SOURCE_DIR}/examples/regressor.c
  ${CMAKE_SOURCE_DIR}/examples/rnn.c
  ${CMAKE_SOURCE_DIR}/examples/segmenter.c
  ${CMAKE_SOURCE_DIR}/examples/super.c
#  ${CMAKE_SOURCE_DIR}/examples/swag.c
  ${CMAKE_SOURCE_DIR}/examples/tag.c
#  ${CMAKE_SOURCE_DIR}/examples/voxel.c
#  ${CMAKE_SOURCE_DIR}/examples/writing.c
  ${CMAKE_SOURCE_DIR}/examples/yolo.c
  ${CUDA_ADDITIONAL_BUILD_FILE}
)

#file(GLOB YOLO_SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.c)

list(APPEND YOLO_SOURCE_FILES
  ${CMAKE_SOURCE_DIR}/src/activation_layer.c
  ${CMAKE_SOURCE_DIR}/src/activations.c
  ${CMAKE_SOURCE_DIR}/src/avgpool_layer.c
  ${CMAKE_SOURCE_DIR}/src/batchnorm_layer.c
  ${CMAKE_SOURCE_DIR}/src/blas.c
  ${CMAKE_SOURCE_DIR}/src/box.c
  ${CMAKE_SOURCE_DIR}/src/col2im.c
#  ${CMAKE_SOURCE_DIR}/src/compare.c
  ${CMAKE_SOURCE_DIR}/src/connected_layer.c
  ${CMAKE_SOURCE_DIR}/src/convolutional_layer.c
  ${CMAKE_SOURCE_DIR}/src/cost_layer.c
  ${CMAKE_SOURCE_DIR}/src/crnn_layer.c
  ${CMAKE_SOURCE_DIR}/src/crop_layer.c
  ${CMAKE_SOURCE_DIR}/src/cuda.c
  ${CMAKE_SOURCE_DIR}/src/data.c
  ${CMAKE_SOURCE_DIR}/src/deconvolutional_layer.c
  ${CMAKE_SOURCE_DIR}/src/demo.c
  ${CMAKE_SOURCE_DIR}/src/detection_layer.c
  ${CMAKE_SOURCE_DIR}/src/dropout_layer.c
  ${CMAKE_SOURCE_DIR}/src/gemm.c
  ${CMAKE_SOURCE_DIR}/src/gru_layer.c
  ${CMAKE_SOURCE_DIR}/src/im2col.c
  ${CMAKE_SOURCE_DIR}/src/image.c
  ${CMAKE_SOURCE_DIR}/src/l2norm_layer.c
  ${CMAKE_SOURCE_DIR}/src/layer.c
  ${CMAKE_SOURCE_DIR}/src/list.c
  ${CMAKE_SOURCE_DIR}/src/local_layer.c
  ${CMAKE_SOURCE_DIR}/src/logistic_layer.c
  ${CMAKE_SOURCE_DIR}/src/lstm_layer.c
  ${CMAKE_SOURCE_DIR}/src/matrix.c
  ${CMAKE_SOURCE_DIR}/src/maxpool_layer.c
  ${CMAKE_SOURCE_DIR}/src/network.c
  ${CMAKE_SOURCE_DIR}/src/normalization_layer.c
  ${CMAKE_SOURCE_DIR}/src/option_list.c
  ${CMAKE_SOURCE_DIR}/src/parser.c
  ${CMAKE_SOURCE_DIR}/src/region_layer.c
  ${CMAKE_SOURCE_DIR}/src/reorg_layer.c
  ${CMAKE_SOURCE_DIR}/src/rnn_layer.c
  ${CMAKE_SOURCE_DIR}/src/route_layer.c
  ${CMAKE_SOURCE_DIR}/src/shortcut_layer.c
  ${CMAKE_SOURCE_DIR}/src/softmax_layer.c
  ${CMAKE_SOURCE_DIR}/src/tree.c
  ${CMAKE_SOURCE_DIR}/src/upsample_layer.c
  ${CMAKE_SOURCE_DIR}/src/utils.c
  ${CMAKE_SOURCE_DIR}/src/yolo_layer.c
)

#file(GLOB YOLO_HEADER_FILES ${CMAKE_SOURCE_DIR}/src/*.h)

list(APPEND YOLO_HEADER_FILES
  ${CMAKE_SOURCE_DIR}/src/activation_layer.h
  ${CMAKE_SOURCE_DIR}/src/activations.h
  ${CMAKE_SOURCE_DIR}/src/avgpool_layer.h
  ${CMAKE_SOURCE_DIR}/src/batchnorm_layer.h
  ${CMAKE_SOURCE_DIR}/src/blas.h
  ${CMAKE_SOURCE_DIR}/src/box.h
  ${CMAKE_SOURCE_DIR}/src/classifier.h
  ${CMAKE_SOURCE_DIR}/src/col2im.h
  ${CMAKE_SOURCE_DIR}/src/connected_layer.h
  ${CMAKE_SOURCE_DIR}/src/convolutional_layer.h
  ${CMAKE_SOURCE_DIR}/src/cost_layer.h
  ${CMAKE_SOURCE_DIR}/src/crnn_layer.h
  ${CMAKE_SOURCE_DIR}/src/crop_layer.h
  ${CMAKE_SOURCE_DIR}/src/cuda.h
  ${CMAKE_SOURCE_DIR}/src/data.h
  ${CMAKE_SOURCE_DIR}/src/deconvolutional_layer.h
  ${CMAKE_SOURCE_DIR}/src/demo.h
  ${CMAKE_SOURCE_DIR}/src/detection_layer.h
  ${CMAKE_SOURCE_DIR}/src/dropout_layer.h
  ${CMAKE_SOURCE_DIR}/src/gemm.h
  ${CMAKE_SOURCE_DIR}/src/gru_layer.h
  ${CMAKE_SOURCE_DIR}/src/im2col.h
  ${CMAKE_SOURCE_DIR}/src/image.h
  ${CMAKE_SOURCE_DIR}/src/l2norm_layer.h
  ${CMAKE_SOURCE_DIR}/src/layer.h
  ${CMAKE_SOURCE_DIR}/src/list.h
  ${CMAKE_SOURCE_DIR}/src/local_layer.h
  ${CMAKE_SOURCE_DIR}/src/logistic_layer.h
  ${CMAKE_SOURCE_DIR}/src/lstm_layer.h
  ${CMAKE_SOURCE_DIR}/src/matrix.h
  ${CMAKE_SOURCE_DIR}/src/maxpool_layer.h
  ${CMAKE_SOURCE_DIR}/src/network.h
  ${CMAKE_SOURCE_DIR}/src/normalization_layer.h
  ${CMAKE_SOURCE_DIR}/src/option_list.h
  ${CMAKE_SOURCE_DIR}/src/parser.h
  ${CMAKE_SOURCE_DIR}/src/region_layer.h
  ${CMAKE_SOURCE_DIR}/src/reorg_layer.h
  ${CMAKE_SOURCE_DIR}/src/rnn_layer.h
  ${CMAKE_SOURCE_DIR}/src/route_layer.h
  ${CMAKE_SOURCE_DIR}/src/shortcut_layer.h
  ${CMAKE_SOURCE_DIR}/src/softmax_layer.h
  ${CMAKE_SOURCE_DIR}/src/stb_image.h
  ${CMAKE_SOURCE_DIR}/src/stb_image_write.h
  ${CMAKE_SOURCE_DIR}/src/tree.h
  ${CMAKE_SOURCE_DIR}/src/upsample_layer.h
  ${CMAKE_SOURCE_DIR}/src/utils.h
  ${CMAKE_SOURCE_DIR}/src/yolo_layer.h
)

if(MSVC)
  list(APPEND DARKNET_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/win32/gettimeofday.c
    ${CMAKE_SOURCE_DIR}/win32/clock_gettime.c
    ${CMAKE_SOURCE_DIR}/win32/cvround.c
    ${CMAKE_SOURCE_DIR}/win32/getopt.c
  )
  list(APPEND DARKNET_HEADER_FILES
    ${CMAKE_SOURCE_DIR}/include/darknet.h
    ${CMAKE_SOURCE_DIR}/win32/cvround.h
    ${CMAKE_SOURCE_DIR}/win32/getopt.h
    ${CMAKE_SOURCE_DIR}/win32/timeutils.h
    ${CMAKE_SOURCE_DIR}/win32/unistd.h
  )
  list(APPEND YOLO_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/win32/gettimeofday.c
    ${CMAKE_SOURCE_DIR}/win32/clock_gettime.c
    ${CMAKE_SOURCE_DIR}/win32/cvround.c
    ${CMAKE_SOURCE_DIR}/win32/getopt.c
  )
  list(APPEND YOLO_HEADER_FILES
    ${CMAKE_SOURCE_DIR}/win32/cvround.h
    ${CMAKE_SOURCE_DIR}/win32/getopt.h
    ${CMAKE_SOURCE_DIR}/win32/timeutils.h
    ${CMAKE_SOURCE_DIR}/win32/unistd.h
  )
endif()


file(GLOB CUDA_SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.cu)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/include)

#add_library(darknet_lib STATIC ${YOLO_SOURCE_FILES} ${YOLO_HEADER_FILES})
#target_link_libraries(darknet_lib
#  ${CMAKE_THREAD_LIBS_INIT}
#)

add_library(darknet_lib SHARED ${YOLO_SOURCE_FILES} ${YOLO_HEADER_FILES})
target_link_libraries(darknet_lib
  ${CMAKE_THREAD_LIBS_INIT}
)
set_property(TARGET darknet_lib PROPERTY POSITION_INDEPENDENT_CODE ON)

add_executable(darknet ${DARKNET_SOURCE_FILES} ${DARKNET_HEADER_FILES})
target_link_libraries(darknet
  darknet_lib
  ${CMAKE_THREAD_LIBS_INIT}
)

if(CUDA_FOUND)
  CUDA_ADD_LIBRARY(darknet_cuda STATIC ${CUDA_SOURCE_FILES})
  target_link_libraries(darknet_lib
    darknet_cuda
    ${CUDA_LIBRARIES}
    ${CUDA_CUBLAS_LIBRARIES}
    ${CUDA_curand_LIBRARY}
  )
  set_property(TARGET darknet_cuda PROPERTY POSITION_INDEPENDENT_CODE ON)

  target_link_libraries(darknet
    darknet_cuda
    ${CUDA_LIBRARIES}
    ${CUDA_CUBLAS_LIBRARIES}
    ${CUDA_curand_LIBRARY}
  )
endif()

if(CUDNN_FOUND)
  target_link_libraries(darknet_lib
    ${CUDNN_LIBRARY}
  )
  target_link_libraries(darknet
    ${CUDNN_LIBRARY}
  )
  if(CUDA_FOUND)
    target_link_libraries(darknet_cuda
      ${CUDNN_LIBRARY}
    )
  endif()
endif()

if(OpenCV_FOUND)
  target_link_libraries(darknet_lib
    ${OpenCV_LIBRARIES}
  )
  target_link_libraries(darknet
    ${OpenCV_LIBRARIES}
  )
  if(CUDA_FOUND)
    target_link_libraries(darknet_cuda
      ${OpenCV_LIBRARIES}
    )
  endif()
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  target_link_libraries(darknet_lib
    m
  )
  target_link_libraries(darknet
    m
  )
  if(CUDA_FOUND)
    target_link_libraries(darknet_cuda
      m
    )
  endif()
endif()

if(MSVC)
  target_link_libraries(darknet_lib
    ${PTHREADS_LIBRARIES}
    wsock32
    ws2_32
  )
  target_link_libraries(darknet
    ${PTHREADS_LIBRARIES}
    wsock32
    ws2_32
  )
  if(CUDA_FOUND)
    target_link_libraries(darknet_cuda
      ${PTHREADS_LIBRARIES}
      wsock32
      ws2_32
    )
  endif()
endif()
