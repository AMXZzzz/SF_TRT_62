cmake_minimum_required(VERSION 3.18)		# cmake 最低版本，3.18具有新特性

project(SF_TRT_Project) 	# 项目名称

# -- 改成自己的路径
set(OpenCV_DIR "D:\\PACH\\opencv\\build")   # 你自己的opecv根路径

# -- 设置标准
add_definitions(-DAPI_EXPORTS)			# 这行解决警告
add_definitions(-DUNICODE -D_UNICODE)		# UNICODE字符集
option(CUDA_USE_STATIC_CUDA_RUNTIME OFF)		# 设置可分离编译，在指定生成cu文件之前
set(CMAKE_CXX_STANDARD 17)			# 设置编译器版本
set(CMAKE_BUILD_TYPE Release)			# 设置debug or Release
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")  
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")  
set(CMAKE_CXX_FLAGS_RELEASE "/MT")  
set(CMAKE_CXX_FLAGS_DEBUG "/MTd")  

# -- 设置多线程
set(THREADS_PREFER_PTHREAD_FLAG ON)		# 设置线程首选标志
find_package(Threads)				# 引入线程库
	
# -- CUDA设置
set(CMAKE_CUDA_ARCHITECTURES  61;70;75;86;89)		# 修改为自己的显卡算力,可以写多个显卡,30系显卡必须要11.1以上的cuda
find_package(CUDA REQUIRED)			# 引入CUDA依赖包
include_directories(${CUDA_INCLUDE_DIRS})		# 指定查找 CUDA 的include路径
enable_language(CUDA)  				# 激活CUDA语言支持，则无需在 vs 中设置 cuda 路径

# -- 设置 opencv
find_package(OpenCV QUIET)				# 引入opecv库
include_directories(${OpenCV_INCLUDE_DIRS})		#  opencv  的 include目录

# -- 项目设置
include_directories(${PROJECT_SOURCE_DIR}/include)	# 指定项目的include目录
include_directories(${PROJECT_SOURCE_DIR})		# 指定项目的include目录
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib)		# 指定项目的lib路径
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL" )	# 增量编译

# -- 设置imgui筛选器
file(GLOB_RECURSE IMGUI_RES ./imgui/*.*)
source_group(Imgui FILES ${IMGUI_RES})

# -- spdlog
file(GLOB_RECURSE SPDLOG_RES ./spdlog/*.*)
source_group(Spdlog FILES ${SPDLOG_RES})

# -- 设置项目头文件筛选器
file(GLOB_RECURSE CURRENT_HEADERS ${PROJECT_SOURCE_DIR}/src/*.h)
source_group("Header Files" FILES ${CURRENT_HEADERS}) 

# -- 设置项目cpp文件筛选器
file(GLOB_RECURSE CPP_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

# -- 设置SF_TRT项目属性
add_executable(SF_TRT ${CPP_FILES} ${CURRENT_HEADERS} ${NETVER_H} ${NETVER_H} ${NETVER_S} ${NETVER_M} ${IMGUI_RES} ${SPDLOG_RES})
SET(SF_TRT "${CMAKE_EXE_LINKER_FLAGS} /level='requireAdministrator' /uiAccess='false'")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT SF_TRT)
target_compile_options(SF_TRT PRIVATE "/MT$<$<CONFIG:Release>:>")


# -- 链接依赖
target_link_libraries(SF_TRT  "nvinfer" "nvinfer_plugin" "nvinfer_plugin" )  	# 链接nvidia库 nvinfer.lib  nvinfer_plugin.lib等依赖
target_link_libraries(SF_TRT  "cublas" "cublasLt" )  		# 链接cudann工具库
target_link_libraries(SF_TRT  "d3d11" "dxgi" )		# 链接d3d11和Toast库
target_link_libraries(SF_TRT ${OpenCV_LIBS})     		# 链接opencv依赖
target_link_libraries(SF_TRT ${CUDA_LIBRARIES})  		# 链接CUDA依赖
target_link_libraries(SF_TRT ${TARGET_NAME} nvonnxparser)	# 链接onnx解析器
target_link_libraries(SF_TRT "SFDXGI")     			# dx截图
target_link_libraries(SF_TRT "onnxruntime" "DirectML")    
target_link_libraries(SF_TRT "ntdll.lib")     			# 系统函数