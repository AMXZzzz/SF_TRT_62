cmake_minimum_required(VERSION 3.18)		# cmake 最低版本，3.18具有新特性

project(SF_TRT_Project) 	# 项目名称

# -- OPENCV路径
set(OpenCV_DIR "C:\\Program Files\\opencv\\build")   # 你自己的opecv根路径

# -- 设置变量
set(ACTUATORLIBNAME actuator)

# -- 设置c++标准
add_definitions(-DAPI_EXPORTS)			# 这行解决警告
add_definitions(-DUNICODE -D_UNICODE)		# UNICODE字符集
set(CMAKE_CXX_STANDARD 17)			# 设置编译器版本

# -- 设置多线程
set(THREADS_PREFER_PTHREAD_FLAG ON)		# 设置线程首选标志
find_package(Threads)				# 引入线程库
	
# -- CUDA设置
option(CUDA_USE_STATIC_CUDA_RUNTIME OFF)
set(CMAKE_CUDA_ARCHITECTURES  61;70;75;86;89)		# 修改为自己的显卡算力,可以写多个显卡,30系显卡必须要11.1以上的cuda
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

# -- 设置项目
file(GLOB_RECURSE HEADERS_FILES ${PROJECT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE CPP_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
source_group("Header Files" FILES ${HEADERS_FILES}) 

# -- 设置spdlog
file(GLOB_RECURSE SPDLOG_RES ./spdlog/*.*)
source_group(Spdlog FILES ${SPDLOG_RES})

# -- 设置第三方
file(GLOB_RECURSE THIRD_PARTY ${PROJECT_SOURCE_DIR}/include/*.h)
source_group(Third_party FILES ${THIRD_PARTY})

# -- 设置SF_TRT项目属性
add_executable(${ACTUATORLIBNAME} ${HEADERS_FILES} ${CPP_FILES} ${SPDLOG_RES} ${THIRD_PARTY})
Set_Target_Properties(${ACTUATORLIBNAME} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${ACTUATORLIBNAME})
SET(${ACTUATORLIBNAME} "${CMAKE_EXE_LINKER_FLAGS} /level='requireAdministrator' /uiAccess='false'")

# -- 链接依赖
target_link_libraries(${ACTUATORLIBNAME} "nvinfer" "nvinfer_plugin" "nvinfer_plugin" "cuda" "cudart" "cublas" "cublasLt")  	# 链接nvidia库, cudann工具库		
target_link_libraries(${ACTUATORLIBNAME} ${OpenCV_LIBS})     		# 链接opencv依赖
target_link_libraries(${ACTUATORLIBNAME} ${CUDA_LIBRARIES})  		# 链接CUDA依赖
target_link_libraries(${ACTUATORLIBNAME}  "d3d11" "dxgi" "runtimeobject")  	# 链接d3d11和Toast库
target_link_libraries(${ACTUATORLIBNAME} ${TARGET_NAME} nvonnxparser)	# 链接onnx解析器
target_link_libraries(${ACTUATORLIBNAME} "msdk")     			# 链接易键鼠
target_link_libraries(${ACTUATORLIBNAME} "ntdll")     			# net驱动
target_link_libraries(${ACTUATORLIBNAME} "dxgilib")     			# dx截图
target_link_libraries(${ACTUATORLIBNAME} "onnxruntime" "DirectML")     	# DML依赖