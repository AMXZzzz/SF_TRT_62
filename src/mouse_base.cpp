#include "mouse_base.h"
#include "mouse_sendinput.h"
#include "mousec_ghub.h"

IMouse* sf::createMouse(MouseInfo* info) {
	switch (info->mousec_manner) {
	case sf::Type::MousecType::SendInput: return new ISendInput();
	case sf::Type::MousecType::EasyKeyMouse: return nullptr;
	case sf::Type::MousecType::GHUB: return new IGHUB();
	}
	//! Î´ÃüÖĞ¾ÍÎª¿Õ
    return nullptr;
}
