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

find_package(OpenCV)
if(OpenCV_FOUND)
  add_definitions(-DOPENCV)
  include_directories(${OpenCV_INCLUDE_DIRS})
endif()

find_package(CUDA)
if(CUDA_FOUND)
  add_definitions(-DGPU)
  include_directories(${CUDA_INCLUDE_DIRS})
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
  ${CMAKE_SOURCE_DIR}/src/gemm.c
  ${CMAKE_SOURCE_DIR}/src/utils.c
  ${CMAKE_SOURCE_DIR}/src/cuda.c
  ${CMAKE_SOURCE_DIR}/src/convolutional_layer.c
  ${CMAKE_SOURCE_DIR}/src/list.c
  ${CMAKE_SOURCE_DIR}/src/image.c
  ${CMAKE_SOURCE_DIR}/src/activations.c
  ${CMAKE_SOURCE_DIR}/src/im2col.c
  ${CMAKE_SOURCE_DIR}/src/col2im.c
  ${CMAKE_SOURCE_DIR}/src/blas.c
  ${CMAKE_SOURCE_DIR}/src/crop_layer.c
  ${CMAKE_SOURCE_DIR}/src/dropout_layer.c
  ${CMAKE_SOURCE_DIR}/src/maxpool_layer.c
  ${CMAKE_SOURCE_DIR}/src/softmax_layer.c
  ${CMAKE_SOURCE_DIR}/src/data.c
  ${CMAKE_SOURCE_DIR}/src/matrix.c
  ${CMAKE_SOURCE_DIR}/src/network.c
  ${CMAKE_SOURCE_DIR}/src/connected_layer.c
  ${CMAKE_SOURCE_DIR}/src/cost_layer.c
  ${CMAKE_SOURCE_DIR}/src/parser.c
  ${CMAKE_SOURCE_DIR}/src/option_list.c
  ${CMAKE_SOURCE_DIR}/src/darknet.c
  ${CMAKE_SOURCE_DIR}/src/detection_layer.c
  ${CMAKE_SOURCE_DIR}/src/captcha.c
  ${CMAKE_SOURCE_DIR}/src/route_layer.c
  ${CMAKE_SOURCE_DIR}/src/writing.c
  ${CMAKE_SOURCE_DIR}/src/box.c
  ${CMAKE_SOURCE_DIR}/src/nightmare.c
  ${CMAKE_SOURCE_DIR}/src/normalization_layer.c
  ${CMAKE_SOURCE_DIR}/src/avgpool_layer.c
  ${CMAKE_SOURCE_DIR}/src/coco.c
  ${CMAKE_SOURCE_DIR}/src/dice.c
  ${CMAKE_SOURCE_DIR}/src/yolo.c
  ${CMAKE_SOURCE_DIR}/src/detector.c
  ${CMAKE_SOURCE_DIR}/src/layer.c
  ${CMAKE_SOURCE_DIR}/src/compare.c
  ${CMAKE_SOURCE_DIR}/src/classifier.c
  ${CMAKE_SOURCE_DIR}/src/local_layer.c
  ${CMAKE_SOURCE_DIR}/src/swag.c
  ${CMAKE_SOURCE_DIR}/src/shortcut_layer.c
  ${CMAKE_SOURCE_DIR}/src/activation_layer.c
  ${CMAKE_SOURCE_DIR}/src/rnn_layer.c
  ${CMAKE_SOURCE_DIR}/src/gru_layer.c
  ${CMAKE_SOURCE_DIR}/src/rnn.c
  ${CMAKE_SOURCE_DIR}/src/rnn_vid.c
  ${CMAKE_SOURCE_DIR}/src/crnn_layer.c
  ${CMAKE_SOURCE_DIR}/src/demo.c
  ${CMAKE_SOURCE_DIR}/src/tag.c
  ${CMAKE_SOURCE_DIR}/src/cifar.c
  ${CMAKE_SOURCE_DIR}/src/go.c
  ${CMAKE_SOURCE_DIR}/src/batchnorm_layer.c
  ${CMAKE_SOURCE_DIR}/src/art.c
  ${CMAKE_SOURCE_DIR}/src/region_layer.c
  ${CMAKE_SOURCE_DIR}/src/reorg_layer.c
  ${CMAKE_SOURCE_DIR}/src/super.c
  ${CMAKE_SOURCE_DIR}/src/voxel.c
  ${CMAKE_SOURCE_DIR}/src/tree.c
)

if(MSVC)
  list(APPEND DARKNET_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/win32/gettimeofday.c
    ${CMAKE_SOURCE_DIR}/win32/clock_gettime.c
    ${CMAKE_SOURCE_DIR}/win32/cvround.c
    ${CMAKE_SOURCE_DIR}/win32/getopt.c
  )
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(darknet ${DARKNET_SOURCE_FILES} ${DARKNET_HEADER_FILES})
target_link_libraries(darknet
  ${CMAKE_THREAD_LIBS_INIT}
)

if(CUDNN_FOUND)
  target_link_libraries(darknet
    ${CUDNN_LIBRARY}
  )
endif()

if(OpenCV_FOUND)
  target_link_libraries(darknet
    ${OpenCV_LIBRARIES}
  )
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  target_link_libraries(darknet
    m
  )
endif()

if(MSVC)
  target_link_libraries(darknet
    ${PTHREADS_LIBRARIES}
    wsock32
    ws2_32
  )
endif()
