#include "socket_request.h"
#include "game/client/gameclient.h"

void CWebSocket::OnInit()
{
	SocketConnect();
	SocketListen(Client()->PlayerName());
	SetPlayerSkin(g_Config.m_ClPlayerSkin, std::to_string(g_Config.m_ClPlayerColorBody), std::to_string(g_Config.m_ClPlayerColorFeet), g_Config.m_ClPlayerUseCustomColor);
}

void CWebSocket::SocketConnect()
{
    CGameClient *pClient = (CGameClient *)GameClient();

    pClient->m_SocketIO.set_open_listener([this, pClient]() {
    	m_IsConnected = true;
        dbg_msg("socket.io", "Connected to server");
    });

    pClient->m_SocketIO.set_close_listener([this, pClient](sio::client::close_reason const &) {
    	m_IsConnected = false;
        dbg_msg("socket.io", "Disconnected from server");
    });

    pClient->m_SocketIO.set_fail_listener([this, pClient]() {
    	m_IsConnected = false;
        dbg_msg("socket.io", "Connection failed");
    });

    pClient->m_SocketIO.connect(g_Config.m_ClSocketNameserver);

}

void CWebSocket::SocketDisconnect()
{
    CGameClient *pClient = (CGameClient *)GameClient();
    {
        pClient->m_SocketIO.close();
    }
}

void CWebSocket::SocketListen(const std::string &Name)
{
    CGameClient *pClient = (CGameClient *)GameClient();
    std::string player = Name;

    SetupSocketListeners();

    pClient->m_SocketIO.socket()->emit("nickname", sio::string_message::create(player));
}

void CWebSocket::SetupSocketListeners()
{
    CGameClient *pClient = (CGameClient *)GameClient();

    pClient->m_SocketIO.socket()->on("chat_message", [&](sio::event &ev) { HandleChatMessage(ev); });
    pClient->m_SocketIO.socket()->on("online_update", [&](sio::event &ev) { HandleOnlineUpdate(ev); });


	pClient->m_SocketIO.socket()->on("typing_start", [&](sio::event &ev) {
	auto Data = ev.get_message();
	if(!Data || Data->get_flag() != sio::message::flag_object)
		return;
	std::string Nickname = Data->get_map()["nickname"]->get_string();

	std::lock_guard<std::mutex> lock(m_TypingMutex);
	m_TypingUsers.insert(Nickname);
});

	pClient->m_SocketIO.socket()->on("typing_stop", [&](sio::event &ev) {
		auto Data = ev.get_message();
		if(!Data || Data->get_flag() != sio::message::flag_object)
			return;
		std::string Nickname = Data->get_map()["nickname"]->get_string();

		std::lock_guard<std::mutex> lock(m_TypingMutex);
		m_TypingUsers.erase(Nickname);
	});
}

void CWebSocket::HandleChatMessage(sio::event &ev)
{
	auto Data = ev.get_message();
	if(!Data || Data->get_flag() != sio::message::flag_object)
		return;

	auto Map = Data->get_map();
	std::string Nickname = Map["nickname"]->get_string();
	std::string Message = Map["message"]->get_string();

	ColorRGBA color(1.0f, 1.0f, 1.0f, 1.0f);
	if(Map.find("color") != Map.end())
	{
		auto c = Map["color"]->get_map();
		color.r = c["r"] ? (float)c["r"]->get_double() : 1.0f;
		color.g = c["g"] ? (float)c["g"]->get_double() : 1.0f;
		color.b = c["b"] ? (float)c["b"]->get_double() : 1.0f;
		color.a = c["a"] ? (float)c["a"]->get_double() : 1.0f;
	}

	std::string skin_name;
	if(Map.find("skin_name") != Map.end() && Map["skin_name"]->get_flag() == sio::message::flag_string)
		skin_name = Map["skin_name"]->get_string();

	std::string display = "[" + Nickname + "]: " + Message;
	AddMessage(display, color);

	if(g_Config.m_ClCrossChatInGameChat || IClient::STATE_ONLINE)
	{
		char aBuf[512];
		str_format(aBuf, sizeof(aBuf), "[%s]: %s", Nickname.c_str(), Message.c_str());
		Console()->ExecuteLine(aBuf);
	}
}


void CWebSocket::HandleOnlineUpdate(sio::event &ev)
{
    auto Data = ev.get_message();
    if(!Data || Data->get_flag() != sio::message::flag_object) return;

    auto UsersArray = Data->get_map()["users"]->get_vector();
    std::vector<std::string> OnlinePlayers;
    for(auto &userMsg : UsersArray)
        if(userMsg->get_flag() == sio::message::flag_string)
            OnlinePlayers.push_back(userMsg->get_string());

    std::lock_guard<std::mutex> lock(m_OnlinePlayersMutex);
    m_OnlinePlayers = OnlinePlayers;
}

void CWebSocket::SendChatMessage(const std::string &Msg)
{
    CGameClient *pClient = (CGameClient *)GameClient();
	pClient->m_SocketIO.socket()->emit("chat_message", sio::string_message::create(Msg));
}

void CWebSocket::AddMessage(const std::string &Msg, ColorRGBA MsgColor)
{
	std::lock_guard<std::mutex> lock(m_MessageMutex);
	SChatMessage MsgStruct;
	MsgStruct.m_Text = Msg;
	MsgStruct.m_Color = MsgColor;
	m_ChatMessages.push_back(MsgStruct);
	if(m_ChatMessages.size() > 100)
		m_ChatMessages.erase(m_ChatMessages.begin());
}

void CWebSocket::SetPlayerSkin(const std::string &skin_name, const std::string &body_color, const std::string &feet_color, bool IsCustomColor)
{
	CGameClient *pClient = (CGameClient *)GameClient();
	if(!pClient || !pClient->m_SocketIO.socket())
		return;

	sio::object_message::ptr msg = sio::object_message::create();

	msg->get_map()["skin_name"] = sio::string_message::create(skin_name);
	msg->get_map()["body_color"] = sio::string_message::create(body_color);
	msg->get_map()["feet_color"] = sio::string_message::create(feet_color);
	msg->get_map()["use_custom_color"] = sio::bool_message::create(IsCustomColor);


	pClient->m_SocketIO.socket()->emit("set_skin", msg);

	char aBuf[256];
	str_format(aBuf, sizeof(aBuf), "Set skin: %s (Body: %s, Feet: %s)",
		   skin_name.c_str(), body_color.c_str(), feet_color.c_str());
	AddMessage(aBuf, ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f));
}




std::vector<CWebSocket::SChatMessage> CWebSocket::GetMessages()
{
    std::lock_guard<std::mutex> lock(m_MessageMutex);
    return m_ChatMessages;
}

bool CWebSocket::IsConnected() const
{
	return m_IsConnected;
}

void CWebSocket::OnOpen()
{
	AddMessage("Connected successfully.");
}

void CWebSocket::OnClose(int code, const char* reason)
{
	AddMessage("Disconnected from chat.");
}