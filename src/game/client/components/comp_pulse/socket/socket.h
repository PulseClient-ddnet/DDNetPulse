#ifndef GAME_CLIENT_COMPONENTS_COMP_PULSE_SOCKET_H
#define GAME_CLIENT_COMPONENTS_COMP_PULSE_SOCKET_H

#include "base/color.h"
#include "engine/console.h"
#include "modules/socket_chat.h"

#include <game/client/component.h>

#include <sio_client.h>

#include <mutex>
#include <string>


class CWebSocket : public CComponent
{
public:
	CWebSocketChat m_WebSocketChat;
	std::vector<CWebSocketComponent*> m_vpAll;

	void SetupSocketListeners();
	std::mutex m_SkinMutex;

	sio::client m_Socket;


	bool m_IsConnected;


	void SocketConnect();
	void SocketDisconnect();
	bool IsConnected() const;
	void OnOpen();
	void OnClose();

	void UserInit();

	void OnInit() override;
	int Sizeof() const override { return sizeof(*this); }
};

#endif
