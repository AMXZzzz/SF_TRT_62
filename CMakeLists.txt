# ------------------------------------------------
# 该文件为根项目的CMakeLists.txt文件
# 作者：bilibili: 随风而息
# ------------------------------------------------
cmake_minimum_required(VERSION 3.18)		
project(SF_Project)				

# -- 设置c++属性
set(CMAKE_CXX_STANDARD 17)		
add_definitions(-DUNICODE -D_UNICODE)		
set(CMAKE_BUILD_TYPE Release)			# 设置debug or Release

# -- 定义变量
# 定义opencv的路径，你自己的opecv根路径
set(OpenCV_DIR "C:\\Program Files\\opencv\\build")   
# 定义动态库生成的路径
set(LIBPATH ${PROJECT_SOURCE_DIR}/install/lib)
# 定义exe的存储目录
set(EXECPATH ${PROJECT_SOURCE_DIR}/install/bin)
# 定义头文件变量
set(HEADPATH ${PROJECT_SOURCE_DIR}/include)
# 动态库文件的名字
set(YOLOLIBNAME yoloModule)
set(DMLIBNAME dmlModule)
set(TRTLIBNAME trtModule)
# 执行器的exe, 后续改成DLL
set(ACTUATORLIBNAME actuator)
# -- 引入opencv库
find_package(OpenCV QUIET)				
# -- 引入spdlog日志
file(GLOB_RECURSE SPDLOG_RES ./spdlog/*.*)
source_group(Spdlog FILES ${SPDLOG_RES})
# -- 添加子目录
add_subdirectory(yoloModule)
add_subdirectory(dmlModule)
add_subdirectory(trtModule)
add_subdirectory(actuatorModule)
