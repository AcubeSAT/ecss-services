#ifndef ECSS_SERVICES_MEMMANGSERVICE_HPP
#define ECSS_SERVICES_MEMMANGSERVICE_HPP

#include "Service.hpp"

class MemMangService : public Service {
public:
	MemMangService() {
		serviceType = 6;
	}

	void loadRawMemData(Message &requset);

	void dumpRawMemData(Message &request);

	uint32_t dumpedRawMemData();
};

#endif //ECSS_SERVICES_MEMMANGSERVICE_HPP
