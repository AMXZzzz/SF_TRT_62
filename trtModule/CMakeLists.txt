# ------------------------------------------------
# 该文件为TRT项目的CMakeLists.txt文件
# 作者：bilibili: 随风而息
# ------------------------------------------------

cmake_minimum_required(VERSION 3.18)		
project(TRT_Project)

# 配置CUDA
option(CUDA_USE_STATIC_CUDA_RUNTIME OFF)	
# 引入线程库
set(THREADS_PREFER_PTHREAD_FLAG ON)		
find_package(Threads)			
# 修改为自己的显卡算力,可以写多个显卡,30系显卡必须要11.1以上的cuda
set(CMAKE_CUDA_ARCHITECTURES  61;70;75;86;89)		
enable_language(CUDA)  		
include_directories(${CUDA_INCLUDE_DIRS})		
# 搜索源文件
file(GLOB_RECURSE TRT_HEADERS  ${PROJECT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE TRT_CPP  ${PROJECT_SOURCE_DIR}/src/*.cpp)
# 配置vs筛选器
source_group("Header Files" FILES ${TRT_HEADERS})
# 引入TRT所需的头文件
include_directories(${HEADPATH})
include_directories(${OpenCV_INCLUDE_DIRS})			
# 指定动态库生成的路径
set(LIBRARY_OUTPUT_PATH ${LIBPATH})
#生成动态库
add_library(${TRTLIBNAME} SHARED ${TRT_HEADERS} ${TRT_CPP} ${SPDLOG_RES} ${HEADPATH})
# trt-module.h公有化
target_include_directories(${TRTLIBNAME} PUBLIC src)
# 链接依赖
target_link_libraries(${TRTLIBNAME} "nvinfer" "nvinfer_plugin" "nvinfer_plugin" "cuda" "cudart" "cublas" "cublasLt")  	# 链接nvidia库, cudann工具库		
target_link_libraries(${TRTLIBNAME} ${OpenCV_LIBS})     		# 链接opencv依赖
target_link_libraries(${TRTLIBNAME} ${CUDA_LIBRARIES})  		# 链接CUDA依赖
target_link_libraries(${TRTLIBNAME} ${YOLOLIBNAME})     
