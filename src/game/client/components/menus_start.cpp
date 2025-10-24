/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include "menus_start.h"

#include <engine/client/updater.h>
#include <engine/graphics.h>
#include <engine/keys.h>
#include <engine/serverbrowser.h>
#include <engine/shared/config.h>
#include <engine/textrender.h>

#include <generated/client_data.h>

#include <game/client/gameclient.h>
#include <game/client/ui.h>
#include <game/localization.h>
#include <game/version.h>

#if defined(CONF_PLATFORM_ANDROID)
#include <android/android_main.h>
#endif

using namespace FontIcons;

static const char *s_apSplashTexts[] = {
	"Hook. Fail. Repeat.",
	"Made with love and checkpoints!",
	"Cendren was here.",
	"n9 did it first.",
	"Now with 100% more ragequits.",
	"Teleport abuse not included.",
	"Save your hammerhits, they matter.",
	"This map is easy... said no one ever.",
	"Who needs friends when you have dummy...? Right?...",
	"Your dummy is judging you.",
	"Speedrunners hate this one trick!",
	"Unfreeze me, I swear I won’t troll.",
	"Map rated 2★, ego rated 5★.",
	"This phrase was placed here by Tee gods.",
	"Welcome to hook school – you’re late.",
	"Lag is just a feature.",
	"Reset timer. Reset hope.",
	"Hold F to pay respects to your dummy.",
	"You didn’t triple hook, you triple failed.",
	"n3xt plz!",
	"You can't hammer your way out of this one.",
	"That wasn’t lag, that was skill delay.",
	"Now featuring invisible fails!",
	"Your dummy abandoned you.",
	"Made by players. Cursed by players.",
	"Finish the map, they said.",
	"Rank farming intensifies.",
	"100% skill, 0% sanity.",
	"Spectate the pros. Copy. Fail.",
	"Mapmaker’s revenge starts here.",
	"One more try. One more life.",
	"What’s a solo part between friends?",
	"Manual edgehook required. Good luck.",
	"In tees we trust.",
	"Tee lost. Please return to DDNet.",
	"Checkpoint roulette: Try again.",
	"Trust your dummy... or don’t.",
	"Legend says Aoe beat it blindfolded.",
	"Press F1 to uninstall.",
	"You vs. the dummy she told you not to worry about.",
	"This phrase is harder than the map.",
	"Aoe finished before you even spawned.",
	"You're not Aoe. Accept it.",
	"Cendren would have edgehooked that...",
	"n9 finished this map blindfolded.",
	"Cendren wrote a book on this part.",
	"Aoe says hi. Also, get good.",
	"StormA already reviewed your demo.",
	"n9 could finish this with a spoon.",
	"This map once feared Aoe.",
	"You just failed where Cendren flexed.",
	"+200 lines of code",
	"absolutely no bot",
	"also try teeworlds",
	"didirice notwork",
	"soon will crash",
	"♿",
	"noby is the biggest troll",
	"Spüli is love, Spüli is life",
	"a chatroom disguised as a game",
	"welcome to hell",
	"We love edging!",
	"fng best mod",
	"fng sux",
	"send twinbop pics",
	"blyaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
	"getting hammered",
	"whis best skin",
	"please twin my bop",
	"professional hookers",
	"Stepfunn: want swap?",
	"free zombietoad",
	"10 0",
	"hi - always ",
	"pipotam",
	"try volleyball!",
	"no u",
	"connect dummy? more like skill issue"
};

static const char *GetRandomSplashText() { return s_apSplashTexts[secure_rand() % std::size(s_apSplashTexts)]; }



void CMenusStart::RenderStartMenu(CUIRect MainView)
{
	GameClient()->m_MenuBackground.ChangePosition(CMenuBackground::POS_START);

// render logo

	Graphics()->TextureSet(g_pData->m_aImages[IMAGE_NULL].m_Id);
	Graphics()->QuadsBegin();
	Graphics()->SetColor(0, 0, 0, 0.3f);
	IGraphics::CQuadItem QuadItem2(-MainView.w + 550, 0, 1000, 1000);
	Graphics()->QuadsSetRotation(50.0f);
	Graphics()->QuadsDrawTL(&QuadItem2, 1);
	Graphics()->QuadsEnd();




		Graphics()->TextureSet(g_pData->m_aImages[IMAGE_BANNER].m_Id);
		Graphics()->QuadsBegin();
		Graphics()->SetColor(1, 1, 1, 1);
		IGraphics::CQuadItem QuadItem(MainView.x, MainView.h / 5, 360, 103);
		Graphics()->QuadsDrawTL(&QuadItem, 1);
		Graphics()->QuadsEnd();
	/*
		static const char *s_pCurrentSplashText = nullptr;
		static float s_SplashTextTime = 0.0f;

		if(s_SplashTextTime == 0.0f || Client()->LocalTime() - s_SplashTextTime > 10.0f)
		{
			s_pCurrentSplashText = GetRandomSplashText();
			s_SplashTextTime = Client()->LocalTime();
		}

		// Render splash text
		if(s_pCurrentSplashText)
		{


			float x = MainView.w / 2 + 190.0f;
			float y = 120.0f;

			float BaseSize = 18.0f;
			float PulseSpeed = 0.000000003f; //eeeeh, vibecoded
			float PulseAmount = 2.0f;
			float CurrentTime = time_get_nanoseconds().count() * PulseSpeed;
			float PulseSize = BaseSize + sinf(CurrentTime) * PulseAmount;

			CUIRect SplashButton;
			SplashButton.x = MainView.w / 2 - 150;
			SplashButton.y = 60.0f;
			SplashButton.w = 350.0f;
			SplashButton.h = 100.0f;
			//SplashButton.Draw(ColorRGBA(0.0f, 0.0f, 0.0f, 0.25f), IGraphics::CORNER_ALL, 10.0f);
			static CButtonContainer s_SplashTextButton;

			if(GameClient()->m_Menus.DoButton_Menu(&s_SplashTextButton, "", 0, &SplashButton, BUTTONFLAG_LEFT, nullptr, IGraphics::CORNER_NONE, 0.0f, 0.0f, ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f)))
			{
				s_pCurrentSplashText = GetRandomSplashText();
				s_SplashTextTime = Client()->LocalTime();
			}

			TextRender()->TextColor(1.0f, 0.8f, 0.0f, 1.0f);
			TextRender()->SetFontPreset(EFontPreset::MINECRAFT_FONT);

			TextRender()->TextRotated(x, y - 30, PulseSize, pi / 7.0f, s_pCurrentSplashText);

			TextRender()->SetFontPreset(EFontPreset::DEFAULT_FONT);
			TextRender()->TextColor(TextRender()->DefaultTextColor());
		}
	*/

	// render slash quad


	const float Rounding = 10.0f;
	const float VMargin = MainView.w / 2 - 190.0f;

	CUIRect Button;
	int NewPage = -1;


	CUIRect Menu, Logo;
	MainView.VSplitLeft(MainView.w / 2.8 , &Menu, &MainView);
	Menu.VSplitLeft(15.0f, nullptr, &Menu);
	Menu.HSplitTop(Menu.h / 3, &Logo, &Menu);
	Menu.HSplitBottom(25.0f, &Menu, nullptr);
	//Menu.Draw(ColorRGBA (0.0f, 0.0f, 0.0f, 0.15f), IGraphics::CORNER_ALL, Rounding);


	Menu.HSplitTop(80.0f, nullptr, &Menu);
	Menu.VSplitLeft(7.5f, nullptr, &Menu);
	Menu.HSplitTop(45.0f, &Button, &Menu);
	//Button.VMargin(5.0f, &Button);
	static CButtonContainer s_PlayButton;
	if(GameClient()->m_Menus.DoButton_Menu(&s_PlayButton, Localize("Play", "Start menu"), 0, &Button, BUTTONFLAG_LEFT, g_Config.m_ClShowStartMenuImages ? "play_game" : nullptr, IGraphics::CORNER_ALL, Rounding, 0.5f, ColorRGBA(0.0f, 0.0f, 0.0f, 0.25f)) || Ui()->ConsumeHotkey(CUi::HOTKEY_ENTER) || CheckHotKey(KEY_P)) { NewPage = g_Config.m_UiPage >= CMenus::PAGE_INTERNET && g_Config.m_UiPage <= CMenus::PAGE_FAVORITE_COMMUNITY_5 ? g_Config.m_UiPage : CMenus::PAGE_INTERNET; }


	Menu.HSplitTop(7.50f, nullptr, &Menu);
	Menu.HSplitTop(45.0f, &Button, &Menu);
	//Button.VMargin(5.0f, &Button);

	static CButtonContainer s_MapEditorButton;
	if(GameClient()->m_Menus.DoButton_Menu(&s_MapEditorButton, Localize("Editor"), 0, &Button, BUTTONFLAG_LEFT, g_Config.m_ClShowStartMenuImages ? "editor" : nullptr, IGraphics::CORNER_ALL, Rounding, 0.5f, GameClient()->Editor()->HasUnsavedData() ? ColorRGBA(0.0f, 1.0f, 0.0f, 0.25f) : ColorRGBA(0.0f, 0.0f, 0.0f, 0.25f)) || CheckHotKey(KEY_E))
	{
		g_Config.m_ClEditor = 1;
		Input()->MouseModeRelative();
	}

	Menu.HSplitTop(7.5f, nullptr, &Menu);
	Menu.HSplitTop(45.0f, &Button, &Menu);
	static CButtonContainer s_SettingsButton;
	if(GameClient()->m_Menus.DoButton_Menu(&s_SettingsButton, Localize("Settings"), 0, &Button, BUTTONFLAG_LEFT, g_Config.m_ClShowStartMenuImages ? "settings" : nullptr, IGraphics::CORNER_ALL, Rounding, 0.5f, ColorRGBA(0.0f, 0.0f, 0.0f, 0.25f)) || CheckHotKey(KEY_S))
		NewPage = CMenus::PAGE_SETTINGS;

	CUIRect QuickBar;
	Menu.HSplitTop(7.5f, nullptr, &Menu);
	Menu.HSplitTop(30.0f, &QuickBar, &Menu);
	QuickBar.VSplitLeft(QuickBar.w / 5, &Button, &QuickBar);
	static CButtonContainer s_ChatButton;



	ColorRGBA DefaultColor = TextRender()->DefaultTextColor();
	char aBuf[64];
	str_format(aBuf, sizeof(aBuf), "Welcome back,");
	TextRender()->TextColor(ColorRGBA(0.8f, 0.8f, 0.8f, 1.0f));
	TextRender()->Text(QuickBar.x + 5.f, QuickBar.y, 11.0f, aBuf, -1);
	TextRender()->TextColor(DefaultColor);
	str_format(aBuf, sizeof(aBuf), "%s", Client()->PlayerName());
	TextRender()->TextColor(ColorRGBA(g_Config.m_ClMessageSystemColor));
	TextRender()->Text(QuickBar.x + 5.f, QuickBar.y + 13, 15.0f, aBuf, -1);
	TextRender()->TextColor(DefaultColor);


	TextRender()->SetFontPreset(EFontPreset::ICON_FONT);
	TextRender()->SetRenderFlags(ETextRenderFlags::TEXT_RENDER_FLAG_ONLY_ADVANCE_WIDTH | ETextRenderFlags::TEXT_RENDER_FLAG_NO_X_BEARING | ETextRenderFlags::TEXT_RENDER_FLAG_NO_Y_BEARING | ETextRenderFlags::TEXT_RENDER_FLAG_NO_PIXEL_ALIGNMENT | ETextRenderFlags::TEXT_RENDER_FLAG_NO_OVERSIZE);
	if(GameClient()->m_Menus.DoButton_MenuTab(&s_ChatButton, FONT_ICON_PAPER_PLANE, 0, &Button, IGraphics::CORNER_ALL, nullptr, nullptr, nullptr, nullptr, 5.0f))
	{
		NewPage = CMenus::PAGE_CROSSCHAT;
	}

	CUIRect RightView, Line;
	MainView.VSplitRight(MainView.w / 2.0f, &MainView, &RightView);
	//RightView.Draw(ColorRGBA(0.0f, 0.0f, 0.0f, 0.25f), IGraphics::CORNER_ALL, Rounding);

	RightView.HSplitTop(33.0f, &Line, &RightView);
	Line.VSplitRight(33.0f, nullptr, &Button);
	static CButtonContainer s_QuitButton;
	ColorRGBA QuitColor(1, 0, 0, 0.5f);
	if(GameClient()->m_Menus.DoButton_MenuTab(&s_QuitButton, FONT_ICON_EXITGAME, 0, &Button, IGraphics::CORNER_ALL, nullptr, nullptr, nullptr, &QuitColor, 5.0f))
	{
		Client()->Quit();
	}
	GameClient()->m_Tooltips.DoToolTip(&s_QuitButton, &Button, Localize("Quit"));




	TextRender()->SetRenderFlags(0);
	TextRender()->SetFontPreset(EFontPreset::DEFAULT_FONT);

	/*
	static CButtonContainer s_SettingsButton;
	if(GameClient()->m_Menus.DoButton_Menu(&s_SettingsButton, Localize("Settings"), 0, &Button, BUTTONFLAG_LEFT, g_Config.m_ClShowStartMenuImages ? "settings" : nullptr, IGraphics::CORNER_ALL, Rounding, 0.5f, ColorRGBA(0.0f, 0.0f, 0.0f, 0.25f)) || CheckHotKey(KEY_S))
		NewPage = CMenus::PAGE_SETTINGS;

	Menu.HSplitBottom(5.0f, &Menu, nullptr); // little space
	Menu.HSplitBottom(40.0f, &Menu, &Button);
	static CButtonContainer s_LocalServerButton;

	const bool LocalServerRunning = GameClient()->m_LocalServer.IsServerRunning();
	if(GameClient()->m_Menus.DoButton_Menu(&s_LocalServerButton, LocalServerRunning ? Localize("Stop server") : Localize("Run server"), 0, &Button, BUTTONFLAG_LEFT, g_Config.m_ClShowStartMenuImages ? "local_server" : nullptr, IGraphics::CORNER_ALL, Rounding, 0.5f, LocalServerRunning ? ColorRGBA(0.0f, 1.0f, 0.0f, 0.25f) : ColorRGBA(0.0f, 0.0f, 0.0f, 0.25f)) || (CheckHotKey(KEY_R) && Input()->KeyPress(KEY_R)))
	{
		if(LocalServerRunning) { GameClient()->m_LocalServer.KillServer(); }
		else { GameClient()->m_LocalServer.RunServer({}); }
	}

	Menu.HSplitBottom(5.0f, &Menu, nullptr); // little space
	Menu.HSplitBottom(40.0f, &Menu, &Button);
	static CButtonContainer s_MapEditorButton;
	if(GameClient()->m_Menus.DoButton_Menu(&s_MapEditorButton, Localize("Editor"), 0, &Button, BUTTONFLAG_LEFT, g_Config.m_ClShowStartMenuImages ? "editor" : nullptr, IGraphics::CORNER_ALL, Rounding, 0.5f, GameClient()->Editor()->HasUnsavedData() ? ColorRGBA(0.0f, 1.0f, 0.0f, 0.25f) : ColorRGBA(0.0f, 0.0f, 0.0f, 0.25f)) || CheckHotKey(KEY_E))
	{
		g_Config.m_ClEditor = 1;
		Input()->MouseModeRelative();
	}
	Menu.HSplitBottom(5.0f, &Menu, nullptr); // little space
	Menu.HSplitBottom(40.0f, &Menu, &Button);
	static CButtonContainer s_PlayButton;
	if(GameClient()->m_Menus.DoButton_Menu(&s_PlayButton, Localize("Play", "Start menu"), 0, &Button, BUTTONFLAG_LEFT, g_Config.m_ClShowStartMenuImages ? "play_game" : nullptr, IGraphics::CORNER_ALL, Rounding, 0.5f, ColorRGBA(0.0f, 0.0f, 0.0f, 0.25f)) || Ui()->ConsumeHotkey(CUi::HOTKEY_ENTER) || CheckHotKey(KEY_P)) { NewPage = g_Config.m_UiPage >= CMenus::PAGE_INTERNET && g_Config.m_UiPage <= CMenus::PAGE_FAVORITE_COMMUNITY_5 ? g_Config.m_UiPage : CMenus::PAGE_INTERNET; }

	// render version
	CUIRect CurVersion, ConsoleButton;
	MainView.HSplitBottom(45.0f, nullptr, &CurVersion);
	CurVersion.VSplitRight(40.0f, &CurVersion, nullptr);
	CurVersion.HSplitTop(20.0f, &ConsoleButton, &CurVersion);
	CurVersion.HSplitTop(5.0f, nullptr, &CurVersion);
	ConsoleButton.VSplitRight(40.0f, nullptr, &ConsoleButton);
	Ui()->DoLabel(&CurVersion, GAME_RELEASE_VERSION, 14.0f, TEXTALIGN_MR);

	static CButtonContainer s_ConsoleButton;
	TextRender()->SetFontPreset(EFontPreset::ICON_FONT);
	TextRender()->SetRenderFlags(ETextRenderFlags::TEXT_RENDER_FLAG_ONLY_ADVANCE_WIDTH | ETextRenderFlags::TEXT_RENDER_FLAG_NO_X_BEARING | ETextRenderFlags::TEXT_RENDER_FLAG_NO_Y_BEARING | ETextRenderFlags::TEXT_RENDER_FLAG_NO_PIXEL_ALIGNMENT | ETextRenderFlags::TEXT_RENDER_FLAG_NO_OVERSIZE);
	if(GameClient()->m_Menus.DoButton_Menu(&s_ConsoleButton, FONT_ICON_TERMINAL, 0, &ConsoleButton, BUTTONFLAG_LEFT, nullptr, IGraphics::CORNER_ALL, 5.0f, 0.0f, ColorRGBA(0.0f, 0.0f, 0.0f, 0.1f))) { GameClient()->m_GameConsole.Toggle(CGameConsole::CONSOLETYPE_LOCAL); }
	TextRender()->SetRenderFlags(0);
	TextRender()->SetFontPreset(EFontPreset::DEFAULT_FONT);
*/
	CUIRect VersionUpdate;
	MainView.HSplitBottom(20.0f, nullptr, &VersionUpdate);
	VersionUpdate.VMargin(VMargin, &VersionUpdate);
#if defined(CONF_AUTOUPDATE)
	CUIRect UpdateButton;
	VersionUpdate.VSplitRight(100.0f, &VersionUpdate, &UpdateButton);
	VersionUpdate.VSplitRight(10.0f, &VersionUpdate, nullptr);

	char aBuf[128];
	const IUpdater::EUpdaterState State = Updater()->GetCurrentState();
	const bool NeedUpdate = str_comp(Client()->LatestVersion(), "0");

	if(State == IUpdater::CLEAN && NeedUpdate)
	{
		static CButtonContainer s_VersionUpdate;
		if(GameClient()->m_Menus.DoButton_Menu(&s_VersionUpdate, Localize("Update now"), 0, &UpdateButton, BUTTONFLAG_LEFT, 0, IGraphics::CORNER_ALL, 5.0f, 0.0f, ColorRGBA(0.0f, 0.0f, 0.0f, 0.25f)))
		{
			Updater()->InitiateUpdate();
		}
	}
	else if(State == IUpdater::NEED_RESTART)
	{
		static CButtonContainer s_VersionUpdate;
		if(GameClient()->m_Menus.DoButton_Menu(&s_VersionUpdate, Localize("Restart"), 0, &UpdateButton, BUTTONFLAG_LEFT, 0, IGraphics::CORNER_ALL, 5.0f, 0.0f, ColorRGBA(0.0f, 0.0f, 0.0f, 0.25f)))
		{
			Client()->Restart();
		}
	}
	else if(State >= IUpdater::GETTING_MANIFEST && State < IUpdater::NEED_RESTART)
	{
		Ui()->RenderProgressBar(UpdateButton, Updater()->GetCurrentPercent() / 100.0f);
	}

	if(State == IUpdater::CLEAN && NeedUpdate)
	{
		str_format(aBuf, sizeof(aBuf), Localize("DDNet %s is out!"), Client()->LatestVersion());
		TextRender()->TextColor(1.0f, 0.4f, 0.4f, 1.0f);
	}
	else if(State == IUpdater::CLEAN)
	{
		aBuf[0] = '\0';
	}
	else if(State >= IUpdater::GETTING_MANIFEST && State < IUpdater::NEED_RESTART)
	{
		char aCurrentFile[64];
		Updater()->GetCurrentFile(aCurrentFile, sizeof(aCurrentFile));
		str_format(aBuf, sizeof(aBuf), Localize("Downloading %s:"), aCurrentFile);
	}
	else if(State == IUpdater::FAIL)
	{
		str_copy(aBuf, Localize("Update failed! Check log…"));
		TextRender()->TextColor(1.0f, 0.4f, 0.4f, 1.0f);
	}
	else if(State == IUpdater::NEED_RESTART)
	{
		str_copy(aBuf, Localize("DDNet Client updated!"));
		TextRender()->TextColor(1.0f, 0.4f, 0.4f, 1.0f);
	}
	Ui()->DoLabel(&VersionUpdate, aBuf, 14.0f, TEXTALIGN_ML);
	TextRender()->TextColor(TextRender()->DefaultTextColor());
#elif defined(CONF_INFORM_UPDATE)
	if(str_comp(Client()->LatestVersion(), "0") != 0)
	{
		CUIRect DownloadButton;
		VersionUpdate.VSplitRight(100.0f, &VersionUpdate, &DownloadButton);
		VersionUpdate.VSplitRight(10.0f, &VersionUpdate, nullptr);

		static CButtonContainer s_DownloadButton;
		if(GameClient()->m_Menus.DoButton_Menu(&s_DownloadButton, Localize("Download"), 0, &DownloadButton, BUTTONFLAG_LEFT, 0, IGraphics::CORNER_ALL, 5.0f, 0.0f, ColorRGBA(0.0f, 0.0f, 0.0f, 0.25f)))
		{
			Client()->ViewLink("https://ddnet.org/downloads/");
		}

		char aBuf[64];
		str_format(aBuf, sizeof(aBuf), Localize("DDNet %s is out!"), Client()->LatestVersion());
		SLabelProperties UpdateLabelProps;
		UpdateLabelProps.SetColor(ColorRGBA(1.0f, 0.4f, 0.4f, 1.0f));
		Ui()->DoLabel(&VersionUpdate, aBuf, 14.0f, TEXTALIGN_ML, UpdateLabelProps);
	}
#endif

	if(NewPage != -1)
	{
		GameClient()->m_Menus.SetShowStart(false);
		GameClient()->m_Menus.SetMenuPage(NewPage);
	}
}

bool CMenusStart::CheckHotKey(int Key) const
{
	return !Input()->ShiftIsPressed() && !Input()->ModifierIsPressed() && !Input()->AltIsPressed() && // no modifier
	       Input()->KeyPress(Key) &&
	       !GameClient()->m_GameConsole.IsActive();
}