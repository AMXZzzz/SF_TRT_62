# SF_TRT_62


## ->编译文档

### 1. 安装Visual Studio 2022
- vs2022负载选择 **使用c++的桌面开发**
- 安装完成后才能进入下一步骤

### 2. 安装CUDA,cudann,TensorRt, [安装视频](https://www.bilibili.com/video/BV1pG411h743/?spm_id_from=333.999.0.0&vd_source=48769c3445e4933d438612c7cb69d59c)

### 3. 安装[opencv](https://objects.githubusercontent.com/github-production-release-asset-2e65be/5108051/909c7495-778f-473e-986f-15771fd5aba4?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAIWNJYAX4CSVEH53A%2F20230803%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20230803T140937Z&X-Amz-Expires=300&X-Amz-Signature=7463dfac83b13831d77673fb4046b4a58659bd03d409531c466d94335d224767&X-Amz-SignedHeaders=host&actor_id=96624481&key_id=0&repo_id=5108051&response-content-disposition=attachment%3B%20filename%3Dopencv-4.8.0-windows.exe&response-content-type=application%2Foctet-stream)
- 双击安装后,在exe同级目录下解压出一个opencv文件夹,记住他
  
 ### 4. 安装[cmake](https://objects.githubusercontent.com/github-production-release-asset-2e65be/537699/80b4d510-c3d7-421c-b5ee-8f3890af8c7a?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAIWNJYAX4CSVEH53A%2F20230803%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20230803T141816Z&X-Amz-Expires=300&X-Amz-Signature=0e9430f1448f6c5893715104c6f7e78f21e6e5fea404dd5d4384cd22b3411e09&X-Amz-SignedHeaders=host&actor_id=96624481&key_id=0&repo_id=537699&response-content-disposition=attachment%3B%20filename%3Dcmake-3.27.1-windows-x86_64.msi&response-content-type=application%2Foctet-stream), 版本要求>3.18

### 5.修改SF_TRT_62源码下的CMakeLists.txt
- 打开CMakeLists.txt
- 修改opencv的路径,就是上面记住的路径
- 路径定位到build,即能看到include文件夹
- 检查分隔符是不是2个  **\\\\**
- ctrl + s 保存

### 6. 打开cmake,按照[视频](https://www.bilibili.com/video/BV1pG411h743?p=3&vd_source=48769c3445e4933d438612c7cb69d59c)编译
- 记得将Debug改为Release
- 运行库选择MD

## [作者: Bilibili:随风而息](https://space.bilibili.com/120366874)
## 如果你觉得很酷,请给个赞+投币!
### [其他]
- [python-dll教程](https://www.bilibili.com/video/BV1Pe4y1p7Ds/?share_source=copy_web&vd_source=1ab4c859f1ebd918903f472636409e44)
- [依赖yolov5项目写一个自己的项目](https://www.bilibili.com/video/BV1Da4y1G7B2/?share_source=copy_web&vd_source=1ab4c859f1ebd918903f472636409e44)
