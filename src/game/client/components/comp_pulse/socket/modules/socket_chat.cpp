#include "socket_chat.h"

#include "../socket.h"
#include "game/client/gameclient.h"


void CWebSocketChat::ListenerInit()
{

	m_Socket->socket()->on("chat_message", [&](sio::event &ev) { HandleChatMessage(ev); });
	m_Socket->socket()->on("online_update", [&](sio::event &ev) { HandleOnlineUpdate(ev); });

	m_Socket->socket()->on("typing_start", [&](sio::event &ev) {HandleTypingStart(ev);});
	m_Socket->socket()->on("typing_stop", [&](sio::event &ev) {HandleTypingStop(ev);});

	dbg_msg("test", "%d", *m_IsConnected);
}

void CWebSocketChat::HandleTypingStart(sio::event &ev)
{
	auto Data = ev.get_message();
	if(!Data || Data->get_flag() != sio::message::flag_object)
		return;
	std::string Nickname = Data->get_map()["nickname"]->get_string();

	std::lock_guard<std::mutex> Lock(m_TypingMutex);
	m_TypingUsers.insert(Nickname);
}

void CWebSocketChat::HandleTypingStop(sio::event &ev)
{
	auto Data = ev.get_message();
	if(!Data || Data->get_flag() != sio::message::flag_object)
		return;
	std::string Nickname = Data->get_map()["nickname"]->get_string();

	std::lock_guard<std::mutex> lock(m_TypingMutex);
	m_TypingUsers.erase(Nickname);
}

void CWebSocketChat::HandleChatMessage(sio::event &ev)
{
	auto Data = ev.get_message();
	if(!Data || Data->get_flag() != sio::message::flag_object)
		return;

	auto Map = Data->get_map();
	std::string Nickname = Map["nickname"]->get_string();
	std::string Message = Map["message"]->get_string();

	ColorRGBA color(1.0f, 1.0f, 1.0f, 1.0f);
	if(Map.contains("color"))
	{
		auto c = Map["color"]->get_map();
		color.r = c["r"] ? (float)c["r"]->get_double() : 1.0f;
		color.g = c["g"] ? (float)c["g"]->get_double() : 1.0f;
		color.b = c["b"] ? (float)c["b"]->get_double() : 1.0f;
		color.a = c["a"] ? (float)c["a"]->get_double() : 1.0f;
	}

	if(Map.contains("skin_name") && Map["skin_name"]->get_flag() == sio::message::flag_string)
		std::string skin_name = Map["skin_name"]->get_string();


	char aBuf[128];
	str_format(aBuf, sizeof(aBuf), "%s: %s", Nickname.c_str(), Message.c_str());
	AddMessage(aBuf, color);

	if(g_Config.m_ClCrossChatDebug)
	{
		str_format(aBuf, sizeof(aBuf), "[Debug]: Got color: r=%.2f g=%.2f b=%.2f a=%.2f", color.r, color.g, color.b, color.a);
		AddMessage(aBuf, ColorRGBA(0.0f, 1.0f, 0.0f, 1.0f));
	}
}

void CWebSocketChat::HandleOnlineUpdate(sio::event &ev)
{
	auto Data = ev.get_message();
	if(!Data || Data->get_flag() != sio::message::flag_object)
		return;

	auto UsersArray = Data->get_map()["users"]->get_vector();
	std::vector<std::string> OnlinePlayers;
	for(auto &userMsg : UsersArray)
		if(userMsg->get_flag() == sio::message::flag_string)
			OnlinePlayers.push_back(userMsg->get_string());

	std::lock_guard<std::mutex> lock(m_OnlinePlayersMutex);
	m_OnlinePlayers = OnlinePlayers;
}

void CWebSocketChat::SendChatMessage(const std::string &Msg) const
{
	if(!m_IsConnected)
		return;
	m_Socket->socket()->emit("chat_message", sio::string_message::create(Msg));
}

void CWebSocketChat::AddMessage(const std::string &Msg, ColorRGBA MsgColor)
{
	std::lock_guard<std::mutex> lock(m_MessageMutex);
	SChatMessage MsgStruct;
	MsgStruct.m_Text = Msg;
	MsgStruct.m_Color = MsgColor;
	m_ChatMessages.push_back(MsgStruct);
	if(m_ChatMessages.size() > 100)
		m_ChatMessages.erase(m_ChatMessages.begin());

	dbg_msg("MSGGGGG", "%s" , Msg.c_str());
}

std::vector<CWebSocketChat::SChatMessage> CWebSocketChat::GetMessages()
{
	std::lock_guard<std::mutex> Lock(m_MessageMutex);
	return m_ChatMessages;
}

void CWebSocketChat::SendTypingState(bool State)
{
	if(!m_IsConnected)
		return;

	if(State)
		m_Socket->socket()->emit("typing_start", sio::string_message::create(g_Config.m_PlayerName));
	else
		m_Socket->socket()->emit("typing_stop", sio::string_message::create(g_Config.m_PlayerName));
}


