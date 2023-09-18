# ------------------------------------------------
# 该文件为DML项目的CMakeLists.txt文件
# 作者：bilibili: 随风而息
# ------------------------------------------------
cmake_minimum_required(VERSION 3.18)		
project(DML_Project) 	


# 搜索源文件
file(GLOB_RECURSE DML_HEADERS  ${PROJECT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE DML_CPP  ${PROJECT_SOURCE_DIR}/src/*.cpp)
# 配置vs筛选器
source_group("Header Files" FILES ${DML_HEADERS})
# 设置头文件的搜索路径
include_directories(${HEADPATH})	
# 设置lib库的搜索路径
link_directories(${LIBPATH})
link_directories(${PROJECT_SOURCE_DIR}/lib)	
# 指定动态库生成的路径
set(LIBRARY_OUTPUT_PATH ${LIBPATH})
# 生成动态库
add_library(${DMLIBNAME} SHARED ${DML_CPP} ${DML_HEADERS} ${SPDLOG_RES} ${HEADPATH})
# 公有化
target_include_directories(${DMLIBNAME} PUBLIC src)
target_link_directories(${DMLIBNAME} PUBLIC lib)
# 链接依赖库
target_link_libraries(${DMLIBNAME} ${YOLOLIBNAME})       # target_link_libraries YOLOLIBNAME 后， DMLIBNAME有自身和YOLOLIBNAME的头文件
target_link_libraries(${DMLIBNAME} "d3d11" "dxgi" "onnxruntime" "DirectML")  	