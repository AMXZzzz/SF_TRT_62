#define API_EXPORT
#include "lock.h"
#include "Functional-Lock.h"
#include "multithread-Lock.h"


SF_API Lock* sf::crateLock(LOCKINFO* info) {
	switch (info->lock_method) {
	case 0:
		return new FunctionalLock(info);
	case 1:
		return new MultithreadLock(info);
	default:
		return nullptr;
	}
}
