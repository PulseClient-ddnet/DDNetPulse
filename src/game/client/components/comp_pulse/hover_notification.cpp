#include "hover_notification.h"
#include <game/client/gameclient.h>

CHoverNotification::CHoverNotification()
{
	m_NumActiveNotifications = 0;
	m_HistoryCount = 0;
	m_HistoryIndex = 0;
	m_MaxNotifications = 0;
	m_MaxHistory = 0;
	m_aNotifications = nullptr;
	m_aHistory = nullptr;
}

CHoverNotification::~CHoverNotification()
{
	delete[] m_aNotifications;
	delete[] m_aHistory;
}

void CHoverNotification::ResizeArrays()
{
	int NewMaxNotifications = g_Config.m_ClHoverMessagesMaxNotifications;
	int NewMaxHistory = g_Config.m_ClHoverMessagesMaxHistory;

	if(NewMaxNotifications != m_MaxNotifications)
	{
		SNotification *pNewNotifications = new SNotification[NewMaxNotifications];
		int NewNumActive = 0;

		for(int i = 0; i < m_MaxNotifications && NewNumActive < NewMaxNotifications; i++)
		{
			if(m_aNotifications[i].m_Active)
			{
				pNewNotifications[NewNumActive] = m_aNotifications[i];
				NewNumActive++;
			}
		}

		for(int i = NewNumActive; i < NewMaxNotifications; i++)
		{
			pNewNotifications[i].m_Active = false;
			pNewNotifications[i].m_StartTime = 0;
			pNewNotifications[i].m_Duration = 0;
			pNewNotifications[i].m_aText[0] = '\0';
			pNewNotifications[i].m_Position = vec2(0, 0);
		}

		delete[] m_aNotifications;
		m_aNotifications = pNewNotifications;
		m_MaxNotifications = NewMaxNotifications;
		m_NumActiveNotifications = NewNumActive;
	}

	if(NewMaxHistory != m_MaxHistory)
	{
		SNotification *pNewHistory = new SNotification[NewMaxHistory];
		int NewHistoryCount = 0;

		for(int i = 0; i < m_HistoryCount && NewHistoryCount < NewMaxHistory; i++)
		{
			int OldIndex = (m_HistoryIndex - 1 - i + m_MaxHistory) % m_MaxHistory;
			pNewHistory[NewHistoryCount] = m_aHistory[OldIndex];
			NewHistoryCount++;
		}

		for(int i = NewHistoryCount; i < NewMaxHistory; i++)
		{
			pNewHistory[i].m_aText[0] = '\0';
			pNewHistory[i].m_StartTime = 0;
		}

		delete[] m_aHistory;
		m_aHistory = pNewHistory;
		m_MaxHistory = NewMaxHistory;
		m_HistoryCount = NewHistoryCount;
		m_HistoryIndex = NewHistoryCount % NewMaxHistory;
	}
}

void CHoverNotification::OnInit()
{
	ResizeArrays();
}

void CHoverNotification::OnReset()
{
	Stop();
}

void CHoverNotification::OnWindowResize()
{
	UpdatePositions();
}

void CHoverNotification::OnShutdown()
{
	Stop();
}

void CHoverNotification::UpdatePositions()
{
	float CurrentY = g_Config.m_ClShowKillMessages || g_Config.m_ClShowFinishMessages ? 220.0f : 100.0f;
	float Spacing = 35.0f;

	for(int i = 0; i < m_MaxNotifications; i++)
	{
		if(m_aNotifications[i].m_Active)
		{
			m_aNotifications[i].m_Position = vec2(Graphics()->ScreenWidth() - m_aNotifications[i].m_CacheState - 20.0f, CurrentY);
			CurrentY += Spacing;
		}
	}
}

void CHoverNotification::AddToHistory(const char *pText)
{
	if(m_MaxHistory == 0)
		return;

	str_copy(m_aHistory[m_HistoryIndex].m_aText, pText, sizeof(m_aHistory[m_HistoryIndex].m_aText));
	m_aHistory[m_HistoryIndex].m_StartTime = time_get();

	m_HistoryIndex = (m_HistoryIndex + 1) % m_MaxHistory;
	if(m_HistoryCount < m_MaxHistory)
		m_HistoryCount++;
}

void CHoverNotification::Start(const char *pText, float Duration)
{
	if(m_MaxNotifications == 0)
		return;

	int SlotIndex = -1;
	int64_t OldestTime = time_get();

	for(int i = 0; i < m_MaxNotifications; i++)
	{
		if(!m_aNotifications[i].m_Active)
		{
			SlotIndex = i;
			break;
		}
		if(m_aNotifications[i].m_StartTime < OldestTime)
		{
			OldestTime = m_aNotifications[i].m_StartTime;
			SlotIndex = i;
		}
	}

	if(SlotIndex >= 0)
	{
		str_copy(m_aNotifications[SlotIndex].m_aText, pText, sizeof(m_aNotifications[SlotIndex].m_aText));
		m_aNotifications[SlotIndex].m_Duration = Duration;
		m_aNotifications[SlotIndex].m_StartTime = time_get();
		m_aNotifications[SlotIndex].m_Active = true;
		m_aNotifications[SlotIndex].m_CacheState = TextRender()->TextWidth(14.0f, m_aNotifications[SlotIndex].m_aText, -1);
		m_NumActiveNotifications = std::min(m_NumActiveNotifications + 1, m_MaxNotifications);

		AddToHistory(pText);
		UpdatePositions();
	}
}

void CHoverNotification::Stop()
{
	for(int i = 0; i < m_MaxNotifications; i++)
		m_aNotifications[i].m_Active = false;
	m_NumActiveNotifications = 0;
}

void CHoverNotification::CheckConfigChanges()
{
	static int LastMaxNotifications = g_Config.m_ClHoverMessagesMaxNotifications;
	static int LastMaxHistory = g_Config.m_ClHoverMessagesMaxHistory;

	if(LastMaxNotifications != g_Config.m_ClHoverMessagesMaxNotifications ||
		LastMaxHistory != g_Config.m_ClHoverMessagesMaxHistory)
	{
		ResizeArrays();
		LastMaxNotifications = g_Config.m_ClHoverMessagesMaxNotifications;
		LastMaxHistory = g_Config.m_ClHoverMessagesMaxHistory;
	}
}

void CHoverNotification::OnRender()
{
	CheckConfigChanges();

	if(m_NumActiveNotifications == 0 && !m_pClient->m_Chat.IsActive())
		return;

	Graphics()->MapScreen(0, 0, Graphics()->ScreenWidth(), Graphics()->ScreenHeight());

	if(m_pClient->m_Chat.IsActive() && g_Config.m_ClHoverMessagesHistory)
	{
		float CurrentY = 100.0f;
		float Spacing = 35.0f;

		for(int i = 0; i < m_HistoryCount; i++)
		{
			int HistoryIndex = (m_HistoryIndex - 1 - i + m_MaxHistory) % m_MaxHistory;
			if(m_aHistory[HistoryIndex].m_aText[0] == '\0')
				continue;

			float TextWidth = TextRender()->TextWidth(14.0f, m_aHistory[HistoryIndex].m_aText, -1);
			float BoxWidth = TextWidth + 30.0f;
			float BoxHeight = 30.0f;
			vec2 Position = vec2(Graphics()->ScreenWidth() - TextWidth - 20.0f, CurrentY);

			Graphics()->TextureClear();
			Graphics()->QuadsBegin();
			Graphics()->SetColor(0.0f, 0.0f, 0.0f, 0.7f);
			IGraphics::CQuadItem QuadItem(Position.x - 15.0f, Position.y - 5.0f, BoxWidth, BoxHeight);
			Graphics()->QuadsDrawTL(&QuadItem, 1);

			Graphics()->SetColor(0.8f, 0.0f, 0.0f, 0.3f);
			IGraphics::CQuadItem BorderQuad(Position.x - 15.0f, Position.y - 5.0f, 2.0f, BoxHeight);
			Graphics()->QuadsDrawTL(&BorderQuad, 1);
			Graphics()->QuadsEnd();

			TextRender()->TextColor(0.0f, 0.0f, 0.0f, 0.1f);
			TextRender()->Text(Position.x + 1.0f, Position.y + 1.0f, 14.0f, m_aHistory[HistoryIndex].m_aText, -1);

			TextRender()->TextColor(1.0f, 1.0f, 1.0f, 1.0f);
			TextRender()->Text(Position.x, Position.y, 14.0f, m_aHistory[HistoryIndex].m_aText, -1);
			TextRender()->TextColor(TextRender()->DefaultTextColor());

			CurrentY += Spacing;
		}
	}
	else
	{
		for(int i = 0; i < m_MaxNotifications; i++)
		{
			if(!m_aNotifications[i].m_Active)
				continue;

			float TimePassed = (time_get() - m_aNotifications[i].m_StartTime) / (float)time_freq();
			if(TimePassed > m_aNotifications[i].m_Duration)
			{
				m_aNotifications[i].m_Active = false;
				m_NumActiveNotifications--;
				UpdatePositions();
				continue;
			}

			float Progress = 1.0f;
			if(TimePassed < 0.3f)
				Progress = TimePassed / 0.3f;
			else if(TimePassed > m_aNotifications[i].m_Duration - 0.3f)
				Progress = (m_aNotifications[i].m_Duration - TimePassed) / 0.3f;

			float TextWidth = TextRender()->TextWidth(14.0f, m_aNotifications[i].m_aText, -1);
			float BoxWidth = TextWidth + 30.0f;
			float BoxHeight = 30.0f;

			Graphics()->TextureClear();
			Graphics()->QuadsBegin();
			Graphics()->SetColor(0.0f, 0.0f, 0.0f, 0.7f * Progress);
			IGraphics::CQuadItem QuadItem(m_aNotifications[i].m_Position.x - 15.0f,
				m_aNotifications[i].m_Position.y - 5.0f,
				BoxWidth, BoxHeight);
			Graphics()->QuadsDrawTL(&QuadItem, 1);

			Graphics()->SetColor(0.8f, 0.0f, 0.0f, 0.3f * Progress);
			IGraphics::CQuadItem BorderQuad(m_aNotifications[i].m_Position.x - 15.0f,
				m_aNotifications[i].m_Position.y - 5.0f,
				2.0f, BoxHeight);
			Graphics()->QuadsDrawTL(&BorderQuad, 1);
			Graphics()->QuadsEnd();

			TextRender()->TextColor(0.0f, 0.0f, 0.0f, 0.3f * Progress);
			TextRender()->Text(m_aNotifications[i].m_Position.x + 1.0f,
				m_aNotifications[i].m_Position.y + 1.0f,
				14.0f, m_aNotifications[i].m_aText, -1);

			TextRender()->TextColor(1.0f, 1.0f, 1.0f, Progress);
			TextRender()->Text(m_aNotifications[i].m_Position.x,
				m_aNotifications[i].m_Position.y,
				14.0f, m_aNotifications[i].m_aText, -1);
			TextRender()->TextColor(TextRender()->DefaultTextColor());
		}
	}
}
