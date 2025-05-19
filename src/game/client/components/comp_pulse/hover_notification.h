#ifndef GAME_CLIENT_COMPONENTS_HOVER_NOTIFICATION_H
#define GAME_CLIENT_COMPONENTS_HOVER_NOTIFICATION_H

#include <engine/shared/config.h>
#include <game/client/component.h>

struct SNotification
{
	char m_aText[256];
	int64_t m_StartTime;
	float m_Duration;
	bool m_Active;
	vec2 m_Position;
	float m_CacheState;
};

class CHoverNotification : public CComponent
{
private:
	SNotification *m_aNotifications;
	SNotification *m_aHistory;
	int m_NumActiveNotifications;
	int m_HistoryCount;
	int m_HistoryIndex;
	int m_MaxNotifications;
	int m_MaxHistory;

	void UpdatePositions();
	void AddToHistory(const char *pText);
	void ResizeArrays();
	void CheckConfigChanges();

public:
	CHoverNotification();
	~CHoverNotification();

	void Start(const char *pText, float Duration = 3.0f);
	void Stop();
	void OnRender() override;
	void OnInit() override;
	void OnReset() override;
	void OnWindowResize() override;
	void OnShutdown() override;

	virtual int Sizeof() const override { return sizeof(*this); }
};

#endif
