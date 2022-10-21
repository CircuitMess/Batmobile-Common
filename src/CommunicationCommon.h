#ifndef BATMOBILE_COMMON_COMMUNICATIONCOMMON_H
#define BATMOBILE_COMMON_COMMUNICATIONCOMMON_H

#include <Loop/LoopListener.h>
#include <AsyncTCP.h>
#include "ComType.h"
#include "DisconnectListener.h"
#include <Buffer/RingBuffer.h>
#include <memory>
#include <unordered_set>
#include <Util/WithListeners.h>

struct ControlPacket {
    ComType type;
    uint8_t data;
};

class CommunicationCommon : private LoopListener, private WithListeners<DisconnectListener>{
public:
	CommunicationCommon();
	bool isConnected();
	void addDcListener(DisconnectListener* listener);
	void removeDcListener(DisconnectListener* listener);

protected:
	virtual void setClient(std::unique_ptr<AsyncClient> client);
	void sendPacket(const ControlPacket& packet);
	virtual bool isWiFiConnected() = 0;
	virtual void processPacket(ControlPacket& packet) = 0;

private:
	void loop(uint micros) override;
	bool isClientConnected();
	std::unique_ptr<AsyncClient> client;
	RingBuffer data;
};

#endif //BATMOBILE_COMMON_COMMUNICATIONCOMMON_H
