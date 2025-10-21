#include "socket_request.h"
#include "game/client/gameclient.h"

#include <sio_client.h>

void CWebSocket::OnInit()
{
	//SocketConnect();
	ChatConnect(Client()->PlayerName());
}

void CWebSocket::SocketConnect()
{
	CGameClient *pClient = (CGameClient *)GameClient();

	// websocket init here
	pClient->m_SocketIOConnected = false;
	pClient->m_SocketIO.set_open_listener([this, pClient]() {
		pClient->m_SocketIOConnected = true;
		dbg_msg("socket.io", "Connected to server");
	});

	pClient->m_SocketIO.set_close_listener([this, pClient](sio::client::close_reason const &reason) {
		pClient->m_SocketIOConnected = false;
		dbg_msg("socket.io", "Disconnected from server");
	});

	pClient->m_SocketIO.set_fail_listener([this, pClient]() {
		pClient->m_SocketIOConnected = false;
		dbg_msg("socket.io", "Connection failed");
	});

	// Connect
	pClient->m_SocketIO.connect("http://0.0.0.0:3000"); //TODO: change to actual API server
}

void CWebSocket::SocketDisconnect()
{
	CGameClient *pClient = (CGameClient *)GameClient();
	if(pClient->m_SocketIOConnected)
	{
		pClient->m_SocketIO.close();
		pClient->m_SocketIOConnected = false;
	}
}

void CWebSocket::SocketMessage(const char *pEvent, const sio::message::list pData)
{
	CGameClient *pClient = (CGameClient *)GameClient();
	pClient->SendSocketMessage(pEvent, pData);
}


void CWebSocket::ChatConnect(const std::string &Name)
{
	std::string player = Name.c_str();
	CGameClient *pClient = (CGameClient *)GameClient();

		pClient->m_SocketIO.set_open_listener([&]() {
		dbg_msg("socket.io", "Connecting to server");
		});

		pClient->m_SocketIO.set_close_listener([&](sio::client::close_reason const &reason) {
		dbg_msg("socket.io", "Disconnected from server");
		});

		pClient->m_SocketIO.socket()->on("chat_message", [&](sio::event &ev) {
		    auto data = ev.get_message();
		    if (data && data->get_flag() == sio::message::flag_string)
		    	dbg_msg("Pulse-Chat:", "%s", data->get_string().c_str());
			char aBuf[256];
			str_format(aBuf, sizeof(aBuf), "echo Pulse-chat: %s", data->get_string().c_str());

			Console()->ExecuteLine(aBuf);
		    });

		pClient->m_SocketIO.connect("http://0.0.0.0:3000");
		pClient->m_SocketIO.socket()->emit("nickname", sio::string_message::create(player));

}
