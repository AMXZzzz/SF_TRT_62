# ------------------------------------------------
# 该文件为ACTUATOR项目的CMakeLists.txt文件
# 作者：bilibili: 随风而息
# ------------------------------------------------

cmake_minimum_required(VERSION 3.18)		
project(ACTUATOR_Project)				

# 搜索当前目录下的源文件
file(GLOB_RECURSE ACTUATOR_HEADERS  ${PROJECT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE ACTUATOR_CPP  ${PROJECT_SOURCE_DIR}/src/*.cpp)
# 配置vs筛选器
source_group("Header Files" FILES ${ACTUATOR_HEADERS})
# 添加头文件路径
include_directories(${HEADPATH})	
include_directories(${PROJECT_SOURCE_DIR}/include)	
# 添加库文件路径
link_directories(${LIBPATH})
link_directories(${PROJECT_SOURCE_DIR}/lib)
# 设置输出路径
set(EXECUTABLE_OUTPUT_PATH ${EXECPATH})
# 生成二进制文件
add_executable(${ACTUATORLIBNAME} ${ACTUATOR_HEADERS} ${ACTUATOR_CPP} ${HEADPATH})
# 链接库文件
target_link_libraries(${ACTUATORLIBNAME} ${DMLIBNAME})
target_link_libraries(${ACTUATORLIBNAME} ${TRTLIBNAME})
target_link_libraries(${ACTUATORLIBNAME} "dxgilib")