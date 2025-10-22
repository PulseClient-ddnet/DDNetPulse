#include "socket_request.h"

#include "game/client/gameclient.h"

#include <sio_client.h>

void CWebSocket::OnInit()
{
	// SocketConnect();
	if(g_Config.m_ClCrossChatAutoConnect)
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
	pClient->m_SocketIO.connect("http://0.0.0.0:3000"); // TODO: change to actual API server
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
		auto Data = ev.get_message();
		if(Data && Data->get_flag() == sio::message::flag_string)
		{
			dbg_msg("socket.io", "%s", Data->get_string().c_str());
			if(g_Config.m_ClCrossChatInGameChat)
			{
				char aBuf[256];
				str_format(aBuf, sizeof(aBuf), "echo ->: %s", Data->get_string().c_str());
				Console()->ExecuteLine(aBuf);
			}
			std::string Msg = Data->get_string();
			AddMessage(Msg);
		}
		else if(Data->get_flag() == sio::message::flag_object)
		{
			auto Nickname = Data->get_map()["nickname"]->get_string();
			auto Message = Data->get_map()["message"]->get_string();
			dbg_msg("socket.io", "[%s]: %s", Nickname.c_str(), Message.c_str());
			if(g_Config.m_ClCrossChatInGameChat)
			{
				char aBuf[256];
				str_format(aBuf, sizeof(aBuf), "echo -> [%s]: %s", Nickname.c_str(), Message.c_str());
				Console()->ExecuteLine(aBuf);
			}
			char aBuf[256];
			str_format(aBuf, sizeof(aBuf), "[%s]: %s", Nickname.c_str(), Message.c_str());
			std::string Msg = aBuf;
			AddMessage(Msg);
		}
	});

	pClient->m_SocketIO.socket()->on("online_update", [&](sio::event &ev) {
		auto Data = ev.get_message();
		if(Data->get_flag() == sio::message::flag_object)
		{
			auto UsersArray = Data->get_map()["users"]->get_vector();
			std::vector<std::string> OnlinePlayers;
			for(auto &userMsg : UsersArray)
			{
				if(userMsg->get_flag() == sio::message::flag_string)
				{
					OnlinePlayers.push_back(userMsg->get_string());
				}
			}

			std::lock_guard<std::mutex> lock(m_OnlinePlayersMutex);
			m_OnlinePlayers = OnlinePlayers;

			// dbg_msg("WebSocket", "Online players received: %d", (int)m_OnlinePlayers.size());
		}
	});

	pClient->m_SocketIO.connect("http://0.0.0.0:3000");
	pClient->m_SocketIO.socket()->emit("nickname", sio::string_message::create(player));
}

void CWebSocket::SendChatMessage(const std::string &Msg)
{
	CGameClient *pClient = (CGameClient *)GameClient();
	if(pClient->m_SocketIOConnected)
		pClient->m_SocketIO.socket()->emit("chat_message", sio::string_message::create(Msg));
}

void CWebSocket::AddMessage(const std::string &Msg)
{
	std::lock_guard<std::mutex> Alock(m_MessageMutex);
	m_ChatMessages.push_back(Msg);
	if(m_ChatMessages.size() > 100) // ограничим историю
		m_ChatMessages.erase(m_ChatMessages.begin());
}

std::vector<std::string> CWebSocket::GetMessages()
{
	std::lock_guard<std::mutex> Alock(m_MessageMutex);
	return m_ChatMessages;
}
