#include "socket.h"

#include "game/client/gameclient.h"
#include "modules/socket_chat.h"



void CWebSocket::OnInit()
{
	if(g_Config.m_ClCrossChatAutoConnect)
		SocketConnect();
}

void CWebSocket::SocketConnect()
{

	m_Socket.set_open_listener([this]() {
		m_IsConnected = true;
		dbg_msg("socket.io", "Connected to server");
		UserInit();
		SetupSocketListeners();

	});

	m_Socket.set_close_listener([this](sio::client::close_reason const &) {
		m_IsConnected = false;
		dbg_msg("socket.io", "Disconnected from server");
	});

	m_Socket.set_fail_listener([this]() {
		m_IsConnected = false;
		dbg_msg("socket.io", "Connection failed");
	});

	m_Socket.connect(g_Config.m_ClSocketNameserver);

	// UserInit();
	// SetupSocketListeners();
}

void CWebSocket::SocketDisconnect()
{
		m_Socket.close();
}

void CWebSocket::UserInit()
{

	if(!m_Socket.socket())
		return;

	std::string SkinName = g_Config.m_ClPlayerSkin;
	std::string BodyColor = std::to_string(g_Config.m_ClPlayerColorBody);
	std::string FeetColor = std::to_string(g_Config.m_ClPlayerColorFeet);
	bool IsCustomColor = g_Config.m_ClPlayerUseCustomColor;

	sio::object_message::ptr msg = sio::object_message::create();

	msg->get_map()["skin_name"] = sio::string_message::create(SkinName);
	msg->get_map()["body_color"] = sio::string_message::create(BodyColor);
	msg->get_map()["feet_color"] = sio::string_message::create(FeetColor);
	msg->get_map()["use_custom_color"] = sio::bool_message::create(IsCustomColor);


	m_Socket.socket()->emit("nickname", sio::string_message::create(Client()->PlayerName()));
	m_Socket.socket()->emit("set_skin", msg);
	//SetupSocketListeners();
}

void CWebSocket::SetupSocketListeners()
{

	m_vpAll.push_back(&m_WebSocketChat);
	for (auto* pComponent : m_vpAll)
	{

		pComponent->OnInit(&m_Socket, &m_IsConnected);
		//dbg_msg("AAAAAAAAAA", "Initialized component");
	}
}

bool CWebSocket::IsConnected() const
{
	return m_IsConnected;
}

void CWebSocket::OnOpen()
{
	dbg_msg("socket.io", "Connection opened.");
}

void CWebSocket::OnClose()
{
	dbg_msg("socket.io", "Connection closed.");
}
