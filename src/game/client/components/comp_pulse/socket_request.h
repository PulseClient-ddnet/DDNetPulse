#ifndef GAME_CLIENT_COMPONENTS_COMP_PULSE_SOCKET_REQUEST_H
#define GAME_CLIENT_COMPONENTS_COMP_PULSE_SOCKET_REQUEST_H

#include <game/client/component.h>
#include <sio_client.h>

class CWebSocket : public CComponent
{
private:
	virtual void SocketConnect();

public:
	virtual void SocketDisconnect();
	virtual void SocketMessage(const char *pEvent, const sio::message::list pData);
	virtual void ChatConnect(const std::string &name);

	virtual int Sizeof() const override { return sizeof(*this); }
	virtual void OnInit() override;
};
#endif
