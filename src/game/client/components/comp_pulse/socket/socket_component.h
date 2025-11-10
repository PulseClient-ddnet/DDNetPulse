#ifndef GAME_CLIENT_COMPONENTS_COMP_PULSE_SOCKET_COMPONENT_H
#define GAME_CLIENT_COMPONENTS_COMP_PULSE_SOCKET_COMPONENT_H
#include "base/system.h"
#include "sio_client.h"

class CWebSocketComponent
{
protected:
	sio::client *m_Socket = nullptr;
	bool *m_IsConnected = nullptr;
public:
	virtual ~CWebSocketComponent() = default;

	void OnInit(sio::client *Socket, bool *pIsConnected)
	{
		m_Socket = Socket;
		m_IsConnected = pIsConnected;

		dbg_msg("socket_component", "OnInit called for %s", typeid(*this).name());
		ListenerInit();
	}
	virtual void ListenerInit(){}

};

#endif
