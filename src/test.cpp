//
//
//#include "yolo.h"
//
//int main() {
//
//	float conf = 0.3;
//	float iou = 0.1;
//	Process process;
//
//
//	YOLOINFO info{};
//	info.type = sf::Type::TYPE_YOLOX;
//	info.conf = &conf;
//	info.iou = &iou;
//	info.process = &process;
//
//	YOLO* yolo = sf::createYoloTable(&info);
//
//	std::cout << *yolo->getInputName() << std::endl;
//	std::cout << *yolo->getOutputName() << std::endl;
//	std::cout << *yolo->getConfidencePtr() << std::endl;
//	std::cout << *yolo->getIOUPtr() << std::endl;
//
//	yolo->Release();
//
//	return 0;
//}