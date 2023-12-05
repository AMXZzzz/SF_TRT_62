/*
* ����������
* ������ǿ�Դ��������������ʹ�á��޸ĺͷ�������ʹ�� GNU ͨ�ù������֤�汾 2��GPLv2�������
* ��ע�⣬�ڸ���GPLv2���������֮ǰ�������������������
* 1. �κ��޸ĺ�������Ʒ����ͬ������GPLv2����������������Ʒ���ṩGPLv2�������ı���
* 2. �����뱣��ԭʼ����İ�Ȩͨ�棬����������Ŀ����ȷָ��ԭʼ���ߺ��������Դ��
* 3. ������ַ���������������ṩ������Դ���룬������GPLv2��Ҫ������������޸ĺ�������Ʒ��Դ���롣
* 4. ������ǰ�"ԭ��"�ṩ�ģ��������κ���ʾ��ʾ�ı�֤�����߶���ʹ�ñ������ɵ��κ�ֱ�ӡ���ӡ�ż�������⡢���ͻ�ͽ����𺦲��е����Ρ�
* ʹ�ô˴��뼴��ʾ��ͬ������GPLv2������Ҫ�������������������
* ����ϸ�Ķ�GNUͨ�ù������֤�汾2���˽������ϸ��Ϣ���������� http ://www.gnu.org/licenses/gpl-2.0.html ���ҵ�GPLv2�������ı���
* [��������]: bilibili������Ϣ
* [��ַ]:https://github.com/AMXZzzz/SF_TRT_62.git
* [����]: 2023/10/26
*/
#pragma once
#include <vector>
#include <opencv2/core/types.hpp>

struct IPoint {
	int origin_x = 0;	//! ��ͼԭ��x
	int origin_y = 0;	//! ��ͼԭ��y
	int center_x = 0;	//! ��Ļ���ĵ�x
	int center_y = 0;	//! ��Ļ���ĵ�y
	int CapWidth = 0;	//! ��ͼ��Χ-��
	int CapHeight = 0;	//! ��ͼ��Χ-��
	int WinWidth = 0;	//! ��Ļ��
	int WinHeight = 0;	//! ��Ļ��
};

// !�м�����ṹ��
struct Process {
	std::vector<cv::Rect> boxes;	//! ����Ŀ�������
	std::vector<int> indices;		//! ����Ŀ�������
	std::vector<int> classes;		//! ����Ŀ�����
	std::vector<float> confidences;	//! ����Ŀ������Ŷ�

	int getClassName(int idx) {
		return classes[indices[idx]];
	}
};

