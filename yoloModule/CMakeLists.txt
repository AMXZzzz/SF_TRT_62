# ------------------------------------------------
# 该文件为YOLO项目的CMakeLists.txt文件
# 作者：bilibili: 随风而息
# ------------------------------------------------
cmake_minimum_required(VERSION 3.18)		
project(YOLO_Project) 	

# 搜索源文件
file(GLOB_RECURSE YOLO_HEADERS CONFIGURE_DEPENDS  ${PROJECT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE YOLO_CPP CONFIGURE_DEPENDS  ${PROJECT_SOURCE_DIR}/src/*.cpp)
# 配置vs筛选器
source_group("Header Files" FILES ${YOLO_HEADERS}) 
# 指定头文件搜索路径
include_directories(${HEADPATH})
include_directories(${OpenCV_INCLUDE_DIRS})			
# 指定库文件搜索路径
link_directories(${OpenCV_LIBRARY_DIRS})
# 指定动态库生成的路径
set(LIBRARY_OUTPUT_PATH ${LIBPATH})
# 生成动态库
add_library(${YOLOLIBNAME} SHARED ${YOLO_HEADERS} ${YOLO_CPP} ${HEADPATH})
# 头文件共有化
# target_include_directories(${YOLOLIBNAME} PUBLIC src)
# 链接库文件
target_link_libraries(${YOLOLIBNAME} ${OpenCV_LIBS})