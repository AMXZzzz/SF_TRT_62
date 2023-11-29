# SF_TRT_62


## 该分支为备份分支 (已弃用)


## ->编译文档

### 1. 安装Visual Studio 2022
- vs2022负载选择 **使用c++的桌面开发**
- 安装完成后才能进入下一步骤

### 2. 安装CUDA,cudann,TensorRt, [安装视频](https://www.bilibili.com/video/BV1pG411h743/?spm_id_from=333.999.0.0&vd_source=48769c3445e4933d438612c7cb69d59c)

### 3. 安装[opencv](https://github.com/opencv/opencv/releases)
- 双击安装后,在exe同级目录下解压出一个opencv文件夹,记住他
  
### 4. 安装[cmake](https://github.com/Kitware/CMake/releases)
 - 版本要求>3.18

### 5.修改SF_TRT_62源码下的CMakeLists.txt
- 打开CMakeLists.txt
- 修改opencv的路径,就是上面记住的路径
- 路径定位到build,即能看到include文件夹
- 检查分隔符是不是2个  **\\\\**
- ctrl + s 保存

### 6. 打开cmake,按照[视频](https://www.bilibili.com/video/BV1pG411h743?p=3&vd_source=48769c3445e4933d438612c7cb69d59c)编译
- 记得将Debug改为Release
- 运行库选择MT

## [作者: Bilibili:随风而息](https://space.bilibili.com/120366874)
## 如果你觉得很酷,请给个赞+投币!
### [其他]
- [python-dll教程](https://www.bilibili.com/video/BV1Pe4y1p7Ds/?share_source=copy_web&vd_source=1ab4c859f1ebd918903f472636409e44)
- [依赖yolov5项目写一个自己的项目](https://www.bilibili.com/video/BV1Da4y1G7B2/?share_source=copy_web&vd_source=1ab4c859f1ebd918903f472636409e44)
