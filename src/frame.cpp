#define API_EXPORT
#include "frame.h"
#include "trt-module.h"
#include "dml-module.h"

SF_API Frame* sf::CreateFrame(FRAMEINFO* info) {
	switch (info->frame_type) {
	case sf::Type::TRT_FRAME: return new ITensorRt(info->yolo, info->logger, info->equipment);
	case sf::Type::DML_FRAME: return new IDML(info->yolo, info->logger, info->equipment);
	default: return nullptr;
	}
}