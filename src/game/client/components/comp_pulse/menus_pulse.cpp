#include <base/log.h>
#include <base/math.h>
#include <base/system.h>

#include <engine/graphics.h>
#include <engine/shared/config.h>
#include <engine/shared/linereader.h>
#include <engine/shared/localization.h>
#include <engine/shared/protocol7.h>
#include <engine/storage.h>
#include <engine/textrender.h>
#include <engine/updater.h>

#include <game/client/animstate.h>
#include <game/client/components/chat.h>
#include <game/client/components/menu_background.h>
#include <game/client/components/sounds.h>
#include <game/client/gameclient.h>
#include <game/client/render.h>
#include <game/client/skin.h>
#include <game/client/ui.h>
#include <game/client/ui_listbox.h>
#include <game/client/ui_scrollregion.h>
#include <game/localization.h>

#include "../binds.h"
#include "../countryflags.h"
#include "../menus.h"
#include "../skins.h"
#include "skinprofiles.h"

#include <array>
#include <chrono>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

#include <sio_client.h>
using namespace FontIcons;
using namespace std::chrono_literals;

const float FontSize = 14.0f;
const float Margin = 10.0f;
const float HeaderHeight = FontSize + 5.0f + Margin;

const float LineSize = 20.0f;
const float ColorPickerLineSize = 25.0f;
const float HeadlineFontSize = 20.0f;
const float HeadlineHeight = 30.0f;
const float MarginSmall = 5.0f;
const float MarginBetweenViews = 20.0f;

const float ColorPickerLabelSize = 13.0f;
const float ColorPickerLineSpacing = 5.0f;

struct CConsoleImage
{
	char m_aName[64];
	IGraphics::CTextureHandle m_Texture;
	bool m_IsLoaded;
};

static int ListConsoleImagesCallback(const char *pName, int IsDir, int StorageType, void *pUser)
{
	std::vector<CConsoleImage> *pFiles = static_cast<std::vector<CConsoleImage> *>(pUser);

	if(!IsDir && str_endswith(pName, ".png"))
	{
		CConsoleImage Image;
		str_copy(Image.m_aName, pName);
		Image.m_IsLoaded = false;
		pFiles->push_back(Image);
		dbg_msg("console_images", "Found PNG file: %s", pName);
	}

	return 0;
}

void CMenus::RenderConsoleImages(CUIRect MainView)
{
	static std::vector<CConsoleImage> s_vConsoleImages;
	static bool s_ImagesLoaded = false;
	static int s_SelectedImage = -1;
	static int s_SelectedConsoleType = 0; // 0 = default, 1 = RCON

	if(!s_ImagesLoaded)
	{
		const char *apPaths[] = {
			"pulse/assets/console",
			"ddnet/pulse/assets/console"};

		for(const char *pBasePath : apPaths)
		{
			char aPath[IO_MAX_PATH_LENGTH];
			str_format(aPath, sizeof(aPath), "%s", pBasePath);
			dbg_msg("console_images", "Checking path: %s", aPath);

			std::vector<CConsoleImage> vFiles;
			Storage()->ListDirectory(IStorage::TYPE_ALL, aPath, ListConsoleImagesCallback, &vFiles);
			dbg_msg("console_images", "Found %d PNG files in %s", vFiles.size(), aPath);

			for(CConsoleImage &Image : vFiles)
			{
				bool bExists = false;
				for(const CConsoleImage &ExistingImage : s_vConsoleImages)
				{
					if(str_comp(ExistingImage.m_aName, Image.m_aName) == 0)
					{
						bExists = true;
						break;
					}
				}

				if(!bExists)
				{
					CImageInfo ImgInfo;
					char aFullPath[IO_MAX_PATH_LENGTH];
					str_format(aFullPath, sizeof(aFullPath), "%s/%s", aPath, Image.m_aName);
					dbg_msg("console_images", "Loading image: %s", aFullPath);

					if(Graphics()->LoadPng(ImgInfo, aFullPath, IStorage::TYPE_ALL))
					{
						Image.m_Texture = Graphics()->LoadTextureRaw(ImgInfo, 0);
						Image.m_IsLoaded = true;
						s_vConsoleImages.push_back(Image);
						dbg_msg("console_images", "Successfully loaded image: %s", Image.m_aName);
					}
					else
					{
						dbg_msg("console_images", "Failed to load image: %s", aFullPath);
					}
				}
			}
		}
		s_ImagesLoaded = true;
	}

	// Display console type selector
	CUIRect ConsoleTypeSelector;
	MainView.HSplitTop(20.0f, &ConsoleTypeSelector, &MainView);

	static CButtonContainer s_DefaultConsoleBtn;
	static CButtonContainer s_RconConsoleBtn;

	CUIRect DefaultBtn, RconBtn;
	ConsoleTypeSelector.VSplitMid(&DefaultBtn, &RconBtn);

	if(DoButton_CheckBox(&s_DefaultConsoleBtn, Localize("Default Console"), s_SelectedConsoleType == 0, &DefaultBtn))
		s_SelectedConsoleType = 0;
	if(DoButton_CheckBox(&s_RconConsoleBtn, Localize("RCON Console"), s_SelectedConsoleType == 1, &RconBtn))
		s_SelectedConsoleType = 1;

	// Display images in list
	static CListBox s_ListBox;
	s_ListBox.DoHeader(&MainView, Localize("Console Images"), 20.0f);
	s_ListBox.DoStart(20.0f, s_vConsoleImages.size(), 1, 3, s_SelectedImage);

	for(size_t i = 0; i < s_vConsoleImages.size(); i++)
	{
		const CConsoleImage &Image = s_vConsoleImages[i];
		const CListboxItem Item = s_ListBox.DoNextItem(&Image.m_aName, s_SelectedImage == (int)i);

		if(!Item.m_Visible)
			continue;

		CUIRect Icon, Label;
		Item.m_Rect.VSplitLeft(Item.m_Rect.h * 2.0f, &Icon, &Label);

		if(Image.m_IsLoaded && Image.m_Texture.IsValid())
		{
			Icon.VMargin(6.0f, &Icon);
			Icon.HMargin(3.0f, &Icon);
			Graphics()->TextureSet(Image.m_Texture);
			Graphics()->QuadsBegin();
			Graphics()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
			IGraphics::CQuadItem QuadItem(Icon.x, Icon.y, Icon.w, Icon.h);
			Graphics()->QuadsDrawTL(&QuadItem, 1);
			Graphics()->QuadsEnd();
		}

		Ui()->DoLabel(&Label, Image.m_aName, 16.0f * CUi::ms_FontmodHeight, TEXTALIGN_ML);
	}

	const int NewSelected = s_ListBox.DoEnd();
	if(s_SelectedImage != NewSelected)
	{
		s_SelectedImage = NewSelected;
		if(s_SelectedImage >= 0 && s_SelectedImage < (int)s_vConsoleImages.size())
		{
			const CConsoleImage &SelectedImage = s_vConsoleImages[s_SelectedImage];

			char a_cleanName[64];
			str_copy(a_cleanName, SelectedImage.m_aName, sizeof(a_cleanName));
			int len = str_length(a_cleanName);
			if(len > 4 && str_comp(a_cleanName + len - 4, ".png") == 0)
				a_cleanName[len - 4] = '\0';

			char a_buf[128];
			str_format(a_buf, sizeof(a_buf), "p_console_asset %s", a_cleanName);
			Console()->ExecuteLine(a_buf);
			Console()->ExecuteLine("p_console_reload");

			// Set the selected background for the appropriate console type
			if(s_SelectedConsoleType == 0)
				str_copy(g_Config.m_ClCustomConsoleDefault, a_cleanName, sizeof(g_Config.m_ClCustomConsoleDefault));
			else
				str_copy(g_Config.m_ClCustomConsoleRcon, a_cleanName, sizeof(g_Config.m_ClCustomConsoleRcon));

			dbg_msg("console_images", "Selected image: %s for %s console", a_cleanName, s_SelectedConsoleType == 0 ? "default" : "RCON");
		}
	}
}

enum
{
	PULSE_TAB_GLOBAL = 0,
	PULSE_TAB_CONSOLE = 1,
	NUMBER_OF_PULSE_TABS = 2,
};

void CMenus::RenderSettingsPulse(CUIRect MainView)
{
	static int s_CurTab = 0;
	CUIRect TabBar, Button;

	MainView.HSplitTop(20.0f, &TabBar, &MainView);
	const float TabWidth = TabBar.w / NUMBER_OF_PULSE_TABS;
	static CButtonContainer s_aPageTabs[NUMBER_OF_PULSE_TABS] = {};
	const char *apTabNames[NUMBER_OF_PULSE_TABS] = {
		Localize("Pulse"),
		Localize("Console"),
	};

	for(int Tab = PULSE_TAB_GLOBAL; Tab < NUMBER_OF_PULSE_TABS; ++Tab)
	{
		TabBar.VSplitLeft(TabWidth, &Button, &TabBar);
		const int Corners = Tab == PULSE_TAB_GLOBAL ? IGraphics::CORNER_L : Tab == NUMBER_OF_PULSE_TABS - 1 ? IGraphics::CORNER_R : IGraphics::CORNER_NONE;
		if(DoButton_MenuTab(&s_aPageTabs[Tab], apTabNames[Tab], s_CurTab == Tab, &Button, Corners, nullptr, nullptr, nullptr, nullptr, 4.0f))
		{
			s_CurTab = Tab;
		}
	}

	if(s_CurTab == PULSE_TAB_GLOBAL)
	{
		// Add some spacing at the top
		MainView.HSplitTop(10.0f, nullptr, &MainView);

		// Split into two columns
		CUIRect Left, Right;
		MainView.VSplitMid(&Left, &Right, 10.0f);

		// Left Column - Gameplay Settings
		{
			CUIRect Section, Label;
			Left.HSplitTop(40.0f, &Section, &Left);
			Ui()->DoLabel(&Section, Localize("Gameplay Settings"), 20.0f, TEXTALIGN_MC);
			Left.HSplitTop(10.0f, nullptr, &Left);

			// Fast Input
			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_Config.m_ClFastInp, Localize("Fast Input"), g_Config.m_ClFastInp, &Button))
				g_Config.m_ClFastInp ^= 1;

			// Add spacing
			Left.HSplitTop(20.0f, nullptr, &Left);

			// Laser Settings
			Left.HSplitTop(40.0f, &Section, &Left);
			Ui()->DoLabel(&Section, Localize("Laser Settings"), 20.0f, TEXTALIGN_MC);
			Left.HSplitTop(10.0f, nullptr, &Left);

			// RTX Laser
			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_Config.m_ClBetterLasers, Localize("Enhanced Laser Effects"), g_Config.m_ClBetterLasers, &Button))
				g_Config.m_ClBetterLasers ^= 1;

			if(g_Config.m_ClBetterLasers)
			{
				// Laser Glow Intensity
				Left.HSplitTop(20.0f, &Button, &Left);
				Ui()->DoScrollbarOption(&g_Config.m_ClLaserGlowIntensity, &g_Config.m_ClLaserGlowIntensity, &Button, Localize("Laser Glow Intensity"), 30, 100);

				// Laser Preview
				Left.HSplitTop(20.0f, &Label, &Left);
				Ui()->DoLabel(&Label, Localize("Laser Preview"), 16.0f, TEXTALIGN_ML);
				Left.HSplitTop(10.0f, nullptr, &Left);

				const float LaserPreviewHeight = 50.0f;
				CUIRect LaserPreview;
				Left.HSplitTop(LaserPreviewHeight, &LaserPreview, &Left);
				Left.HSplitTop(2 * MarginSmall, nullptr, &Left);
				DoLaserPreview(&LaserPreview, g_Config.m_ClLaserRifleInnerColor, g_Config.m_ClLaserRifleOutlineColor, LASERTYPE_RIFLE);

				Left.HSplitTop(LaserPreviewHeight, &LaserPreview, &Left);
				Left.HSplitTop(2 * MarginSmall, nullptr, &Left);
				DoLaserPreview(&LaserPreview, g_Config.m_ClLaserShotgunInnerColor, g_Config.m_ClLaserShotgunOutlineColor, LASERTYPE_SHOTGUN);
			}
		}

		// Right Column - Visual Effects
		{
			CUIRect Section;
			Right.HSplitTop(40.0f, &Section, &Right);
			Ui()->DoLabel(&Section, Localize("Visual Effects"), 20.0f, TEXTALIGN_MC);
			Right.HSplitTop(10.0f, nullptr, &Right);

			// Player Effects
			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_ClPlayerIdleAura, Localize("Idle Player Aura"), g_Config.m_ClPlayerIdleAura, &Button))
				g_Config.m_ClPlayerIdleAura ^= 1;

			if(g_Config.m_ClPlayerIdleAura)
			{
				Right.HSplitTop(20.0f, &Button, &Right);
				Ui()->DoScrollbarOption(&g_Config.m_ClPlayerIdleAuraTimer, &g_Config.m_ClPlayerIdleAuraTimer, &Button, Localize("Aura Timer"), 2, 30);
			}

			/* Trail Style
			Right.HSplitTop(20.0f, &Button, &Right);
			Ui()->DoScrollbarOption(&g_Config.m_ClTrailStyle, &g_Config.m_ClTrailStyle, &Button, Localize("Trail Style"), 0, 2);
			*/
			// Hover Messages
			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_ClHoverMessages, Localize("Hover Messages"), g_Config.m_ClHoverMessages, &Button))
				g_Config.m_ClHoverMessages ^= 1;

			if(g_Config.m_ClHoverMessages)
			{
				Right.HSplitTop(20.0f, &Button, &Right);
				if(DoButton_CheckBox(&g_Config.m_ClHoverMessagesHistory, Localize("Message History"), g_Config.m_ClHoverMessagesHistory, &Button))
					g_Config.m_ClHoverMessagesHistory ^= 1;

				Right.HSplitTop(20.0f, &Button, &Right);
				Ui()->DoScrollbarOption(&g_Config.m_ClHoverMessagesMaxHistory, &g_Config.m_ClHoverMessagesMaxHistory, &Button, Localize("Max History"), 1, 40);

				Right.HSplitTop(20.0f, &Button, &Right);
				Ui()->DoScrollbarOption(&g_Config.m_ClHoverMessagesMaxNotifications, &g_Config.m_ClHoverMessagesMaxNotifications, &Button, Localize("Max Notifications"), 1, 40);
			}
		}
	}
	else if(s_CurTab == PULSE_TAB_CONSOLE)
	{
		CUIRect Left, Right;
		MainView.VSplitMid(&Left, &Right);
		Left.HSplitTop(20.0f, &Button, &Left);

		if(DoButton_CheckBox(&g_Config.m_ClCustomConsole, Localize("Toggle Custom Console"), g_Config.m_ClCustomConsole, &Button))
			g_Config.m_ClCustomConsole ^= 1;

		if(g_Config.m_ClCustomConsole)
		{
			Right.Draw(ColorRGBA(0.0f, 0.0f, 0.0f, 0.3f), IGraphics::CORNER_ALL, 5.0f);
			RenderConsoleImages(Right);
			CUIRect Title, OpenFolderButton;
			Left.HSplitTop(40.0f, &Title, &Left);
			Ui()->DoLabel(&Title, Localize("Console Settings"), 20.0f, TEXTALIGN_BC);

			Left.HSplitTop(20.0f, &Button, &Left);
			Ui()->DoScrollbarOption(&g_Config.m_ClCustomConsoleAlpha, &g_Config.m_ClCustomConsoleAlpha, &Button, Localize("Console Alpha"), 0, 100);

			Left.HSplitTop(20.0f, &Button, &Left);
			Ui()->DoScrollbarOption(&g_Config.m_ClCustomConsoleFading, &g_Config.m_ClCustomConsoleFading, &Button, Localize("Console Brightness"), 100, 0);

			Left.HSplitTop(20.0f, &Button, &Left);
			Ui()->DoLabel(&Button, Localize("RCON Console"), 14.0f, TEXTALIGN_ML);

			Left.HSplitTop(20.0f, &Button, &Left);
			Ui()->DoScrollbarOption(&g_Config.m_ClCustomConsoleRconAlpha, &g_Config.m_ClCustomConsoleRconAlpha, &Button, Localize("RCON Console Alpha"), 0, 100);

			Left.HSplitTop(20.0f, &Button, &Left);
			Ui()->DoScrollbarOption(&g_Config.m_ClCustomConsoleRconFading, &g_Config.m_ClCustomConsoleRconFading, &Button, Localize("RCON Console Brightness"), 100, 0);

			//** Open Console Assets Folder **//
			Left.HSplitTop(20.0f, &OpenFolderButton, &Left);
			static CButtonContainer s_OpenFolderButton;
			if(DoButton_Menu(&s_OpenFolderButton, Localize("Open Console Assets Folder"), 0, &OpenFolderButton))
			{
				char aPath[IO_MAX_PATH_LENGTH];
				Storage()->GetCompletePath(IStorage::TYPE_SAVE, "pulse/assets/console", aPath, sizeof(aPath));
				Client()->ViewFile(aPath);
			}
		}
	}
}

void CMenus::RenderSettingsProfs(CUIRect MainView)
{
	CUIRect Label, LabelMid, Section, LabelRight;
	static int s_SelectedProfile = -1;

	char *pSkinName = g_Config.m_ClPlayerSkin;
	int *pUseCustomColor = &g_Config.m_ClPlayerUseCustomColor;
	unsigned *pColorBody = &g_Config.m_ClPlayerColorBody;
	unsigned *pColorFeet = &g_Config.m_ClPlayerColorFeet;
	int CurrentFlag = m_Dummy ? g_Config.m_ClDummyCountry : g_Config.m_PlayerCountry;

	if(m_Dummy)
	{
		pSkinName = g_Config.m_ClDummySkin;
		pUseCustomColor = &g_Config.m_ClDummyUseCustomColor;
		pColorBody = &g_Config.m_ClDummyColorBody;
		pColorFeet = &g_Config.m_ClDummyColorFeet;
	}

	// skin info
	CTeeRenderInfo OwnSkinInfo;
	const CSkin *pSkin = m_pClient->m_Skins.Find(pSkinName);
	OwnSkinInfo.m_OriginalRenderSkin = pSkin->m_OriginalSkin;
	OwnSkinInfo.m_ColorableRenderSkin = pSkin->m_ColorableSkin;
	OwnSkinInfo.m_SkinMetrics = pSkin->m_Metrics;
	OwnSkinInfo.m_CustomColoredSkin = *pUseCustomColor;
	if(*pUseCustomColor)
	{
		OwnSkinInfo.m_ColorBody = color_cast<ColorRGBA>(ColorHSLA(*pColorBody).UnclampLighting(ColorHSLA::DARKEST_LGT));
		OwnSkinInfo.m_ColorFeet = color_cast<ColorRGBA>(ColorHSLA(*pColorFeet).UnclampLighting(ColorHSLA::DARKEST_LGT));
	}
	else
	{
		OwnSkinInfo.m_ColorBody = ColorRGBA(1.0f, 1.0f, 1.0f);
		OwnSkinInfo.m_ColorFeet = ColorRGBA(1.0f, 1.0f, 1.0f);
	}
	OwnSkinInfo.m_Size = 50.0f;

	//======YOUR PROFILE======
	char aTempBuf[256];
	str_format(aTempBuf, sizeof(aTempBuf), "%s:", Localize("Your profile"));
	MainView.HSplitTop(LineSize, &Label, &MainView);
	Ui()->DoLabel(&Label, aTempBuf, FontSize, TEXTALIGN_ML);
	MainView.HSplitTop(MarginSmall, nullptr, &MainView);

	MainView.HSplitTop(50.0f, &Label, &MainView);
	Label.VSplitLeft(250.0f, &Label, &LabelMid);
	const CAnimState *pIdleState = CAnimState::GetIdle();
	vec2 OffsetToMid;
	CRenderTools::GetRenderTeeOffsetToRenderedTee(pIdleState, &OwnSkinInfo, OffsetToMid);
	vec2 TeeRenderPos(Label.x + LineSize, Label.y + Label.h / 2.0f + OffsetToMid.y);
	int Emote = m_Dummy ? g_Config.m_ClDummyDefaultEyes : g_Config.m_ClPlayerDefaultEyes;
	RenderTools()->RenderTee(pIdleState, &OwnSkinInfo, Emote, vec2(1.0f, 0.0f), TeeRenderPos);

	char aName[64];
	char aClan[64];
	str_format(aName, sizeof(aName), "%s", m_Dummy ? g_Config.m_ClDummyName : g_Config.m_PlayerName);
	str_format(aClan, sizeof(aClan), "%s", m_Dummy ? g_Config.m_ClDummyClan : g_Config.m_PlayerClan);

	CUIRect FlagRect;
	Label.VSplitLeft(90.0f, &FlagRect, &Label);

	Label.HSplitTop(LineSize, &Section, &Label);
	str_format(aTempBuf, sizeof(aTempBuf), Localize("Name: %s"), aName);
	Ui()->DoLabel(&Section, aTempBuf, FontSize, TEXTALIGN_ML);

	Label.HSplitTop(LineSize, &Section, &Label);
	str_format(aTempBuf, sizeof(aTempBuf), Localize("Clan: %s"), aClan);
	Ui()->DoLabel(&Section, aTempBuf, FontSize, TEXTALIGN_ML);

	Label.HSplitTop(LineSize, &Section, &Label);
	str_format(aTempBuf, sizeof(aTempBuf), Localize("Skin: %s"), pSkinName);
	Ui()->DoLabel(&Section, aTempBuf, FontSize, TEXTALIGN_ML);

	FlagRect.VSplitRight(50.0f, nullptr, &FlagRect);
	FlagRect.HSplitBottom(25.0f, nullptr, &FlagRect);
	FlagRect.y -= 10.0f;
	ColorRGBA Color(1.0f, 1.0f, 1.0f, 1.0f);
	m_pClient->m_CountryFlags.Render(m_Dummy ? g_Config.m_ClDummyCountry : g_Config.m_PlayerCountry, Color, FlagRect.x, FlagRect.y, FlagRect.w, FlagRect.h);

	bool DoSkin = g_Config.m_ClApplyProfileSkin;
	bool DoColors = g_Config.m_ClApplyProfileColors;
	bool DoEmote = g_Config.m_ClApplyProfileEmote;
	bool DoName = g_Config.m_ClApplyProfileName;
	bool DoClan = g_Config.m_ClApplyProfileClan;
	bool DoFlag = g_Config.m_ClApplyProfileFlag;

	//======AFTER LOAD======
	if(s_SelectedProfile != -1 && s_SelectedProfile < (int)GameClient()->m_SkinProfiles.m_Profiles.size())
	{
		CProfile LoadProfile = GameClient()->m_SkinProfiles.m_Profiles[s_SelectedProfile];
		MainView.HSplitTop(LineSize, nullptr, &MainView);
		MainView.HSplitTop(10.0f, &Label, &MainView);
		str_format(aTempBuf, sizeof(aTempBuf), "%s:", Localize("After Load"));
		Ui()->DoLabel(&Label, aTempBuf, FontSize, TEXTALIGN_ML);

		MainView.HSplitTop(50.0f, &Label, &MainView);
		Label.VSplitLeft(250.0f, &Label, nullptr);

		if(DoSkin && strlen(LoadProfile.SkinName) != 0)
		{
			const CSkin *pLoadSkin = m_pClient->m_Skins.Find(LoadProfile.SkinName);
			OwnSkinInfo.m_OriginalRenderSkin = pLoadSkin->m_OriginalSkin;
			OwnSkinInfo.m_ColorableRenderSkin = pLoadSkin->m_ColorableSkin;
			OwnSkinInfo.m_SkinMetrics = pLoadSkin->m_Metrics;
		}
		if(*pUseCustomColor && DoColors && LoadProfile.BodyColor != -1 && LoadProfile.FeetColor != -1)
		{
			OwnSkinInfo.m_ColorBody = color_cast<ColorRGBA>(ColorHSLA(LoadProfile.BodyColor).UnclampLighting(ColorHSLA::DARKEST_LGT));
			OwnSkinInfo.m_ColorFeet = color_cast<ColorRGBA>(ColorHSLA(LoadProfile.FeetColor).UnclampLighting(ColorHSLA::DARKEST_LGT));
		}

		CRenderTools::GetRenderTeeOffsetToRenderedTee(pIdleState, &OwnSkinInfo, OffsetToMid);
		TeeRenderPos = vec2(Label.x + LineSize, Label.y + Label.h / 2.0f + OffsetToMid.y);
		int LoadEmote = Emote;
		if(DoEmote && LoadProfile.Emote != -1)
			LoadEmote = LoadProfile.Emote;
		RenderTools()->RenderTee(pIdleState, &OwnSkinInfo, LoadEmote, vec2(1.0f, 0.0f), TeeRenderPos);

		if(DoName && strlen(LoadProfile.Name) != 0)
			str_format(aName, sizeof(aName), "%s", LoadProfile.Name);
		if(DoClan && strlen(LoadProfile.Clan) != 0)
			str_format(aClan, sizeof(aClan), "%s", LoadProfile.Clan);

		Label.VSplitLeft(90.0f, &FlagRect, &Label);

		Label.HSplitTop(LineSize, &Section, &Label);
		str_format(aTempBuf, sizeof(aTempBuf), Localize("Name: %s"), aName);
		Ui()->DoLabel(&Section, aTempBuf, FontSize, TEXTALIGN_ML);

		Label.HSplitTop(LineSize, &Section, &Label);
		str_format(aTempBuf, sizeof(aTempBuf), Localize("Clan: %s"), aClan);
		Ui()->DoLabel(&Section, aTempBuf, FontSize, TEXTALIGN_ML);

		Label.HSplitTop(LineSize, &Section, &Label);
		str_format(aTempBuf, sizeof(aTempBuf), Localize("Skin: %s"), (DoSkin && strlen(LoadProfile.SkinName) != 0) ? LoadProfile.SkinName : pSkinName);
		Ui()->DoLabel(&Section, aTempBuf, FontSize, TEXTALIGN_ML);

		FlagRect.VSplitRight(50.0f, nullptr, &FlagRect);
		FlagRect.HSplitBottom(25.0f, nullptr, &FlagRect);
		FlagRect.y -= 10.0f;
		int RenderFlag = m_Dummy ? g_Config.m_ClDummyCountry : g_Config.m_PlayerCountry;
		if(DoFlag && LoadProfile.CountryFlag != -2)
			RenderFlag = LoadProfile.CountryFlag;
		m_pClient->m_CountryFlags.Render(RenderFlag, Color, FlagRect.x, FlagRect.y, FlagRect.w, FlagRect.h);

		str_format(aName, sizeof(aName), "%s", m_Dummy ? g_Config.m_ClDummyName : g_Config.m_PlayerName);
		str_format(aClan, sizeof(aClan), "%s", m_Dummy ? g_Config.m_ClDummyClan : g_Config.m_PlayerClan);
	}
	else
	{
		MainView.HSplitTop(80.0f, nullptr, &MainView);
	}

	//===BUTTONS AND CHECK BOX===
	CUIRect DummyCheck, CustomCheck;
	MainView.HSplitTop(30.0f, &DummyCheck, nullptr);
	DummyCheck.HSplitTop(13.0f, nullptr, &DummyCheck);

	DummyCheck.VSplitLeft(100.0f, &DummyCheck, &CustomCheck);
	CustomCheck.VSplitLeft(150.0f, &CustomCheck, nullptr);

	DoButton_CheckBoxAutoVMarginAndSet(&m_Dummy, Localize("Dummy"), (int *)&m_Dummy, &DummyCheck, LineSize);

	static int s_CustomColorID = 0;
	CustomCheck.HSplitTop(LineSize, &CustomCheck, nullptr);

	if(DoButton_CheckBox(&s_CustomColorID, Localize("Custom colors"), *pUseCustomColor, &CustomCheck))
	{
		*pUseCustomColor = *pUseCustomColor ? 0 : 1;
		SetNeedSendInfo();
	}

	LabelMid.VSplitLeft(20.0f, nullptr, &LabelMid);
	LabelMid.VSplitLeft(160.0f, &LabelMid, &LabelRight);

	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClApplyProfileSkin, Localize("Save/Load Skin"), &g_Config.m_ClApplyProfileSkin, &LabelMid, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClApplyProfileColors, Localize("Save/Load Colors"), &g_Config.m_ClApplyProfileColors, &LabelMid, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClApplyProfileEmote, Localize("Save/Load Emote"), &g_Config.m_ClApplyProfileEmote, &LabelMid, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClApplyProfileName, Localize("Save/Load Name"), &g_Config.m_ClApplyProfileName, &LabelMid, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClApplyProfileClan, Localize("Save/Load Clan"), &g_Config.m_ClApplyProfileClan, &LabelMid, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClApplyProfileFlag, Localize("Save/Load Flag"), &g_Config.m_ClApplyProfileFlag, &LabelMid, LineSize);

	CUIRect Button;
	LabelRight.VSplitLeft(150.0f, &LabelRight, nullptr);

	LabelRight.HSplitTop(30.0f, &Button, &LabelRight);
	static CButtonContainer s_LoadButton;

	if(DoButton_Menu(&s_LoadButton, Localize("Load"), 0, &Button))
	{
		if(s_SelectedProfile != -1 && s_SelectedProfile < (int)GameClient()->m_SkinProfiles.m_Profiles.size())
		{
			CProfile LoadProfile = GameClient()->m_SkinProfiles.m_Profiles[s_SelectedProfile];
			if(!m_Dummy)
			{
				if(DoSkin && strlen(LoadProfile.SkinName) != 0)
					str_copy(g_Config.m_ClPlayerSkin, LoadProfile.SkinName, sizeof(g_Config.m_ClPlayerSkin));
				if(DoColors && LoadProfile.BodyColor != -1 && LoadProfile.FeetColor != -1)
				{
					g_Config.m_ClPlayerColorBody = LoadProfile.BodyColor;
					g_Config.m_ClPlayerColorFeet = LoadProfile.FeetColor;
				}
				if(DoEmote && LoadProfile.Emote != -1)
					g_Config.m_ClPlayerDefaultEyes = LoadProfile.Emote;
				if(DoName && strlen(LoadProfile.Name) != 0)
					str_copy(g_Config.m_PlayerName, LoadProfile.Name, sizeof(g_Config.m_PlayerName));
				if(DoClan && strlen(LoadProfile.Clan) != 0)
					str_copy(g_Config.m_PlayerClan, LoadProfile.Clan, sizeof(g_Config.m_PlayerClan));
				if(DoFlag && LoadProfile.CountryFlag != -2)
					g_Config.m_PlayerCountry = LoadProfile.CountryFlag;
			}
			else
			{
				if(DoSkin && strlen(LoadProfile.SkinName) != 0)
					str_copy(g_Config.m_ClDummySkin, LoadProfile.SkinName, sizeof(g_Config.m_ClDummySkin));
				if(DoColors && LoadProfile.BodyColor != -1 && LoadProfile.FeetColor != -1)
				{
					g_Config.m_ClDummyColorBody = LoadProfile.BodyColor;
					g_Config.m_ClDummyColorFeet = LoadProfile.FeetColor;
				}
				if(DoEmote && LoadProfile.Emote != -1)
					g_Config.m_ClDummyDefaultEyes = LoadProfile.Emote;
				if(DoName && strlen(LoadProfile.Name) != 0)
					str_copy(g_Config.m_ClDummyName, LoadProfile.Name, sizeof(g_Config.m_ClDummyName));
				if(DoClan && strlen(LoadProfile.Clan) != 0)
					str_copy(g_Config.m_ClDummyClan, LoadProfile.Clan, sizeof(g_Config.m_ClDummyClan));
				if(DoFlag && LoadProfile.CountryFlag != -2)
					g_Config.m_ClDummyCountry = LoadProfile.CountryFlag;
			}
		}
		SetNeedSendInfo();
	}
	LabelRight.HSplitTop(5.0f, nullptr, &LabelRight);

	LabelRight.HSplitTop(30.0f, &Button, &LabelRight);
	static CButtonContainer s_SaveButton;
	if(DoButton_Menu(&s_SaveButton, Localize("Save"), 0, &Button))
	{
		GameClient()->m_SkinProfiles.AddProfile(
			DoColors ? *pColorBody : -1,
			DoColors ? *pColorFeet : -1,
			DoFlag ? CurrentFlag : -2,
			DoEmote ? Emote : -1,
			DoSkin ? pSkinName : "",
			DoName ? aName : "",
			DoClan ? aClan : "");
		GameClient()->m_SkinProfiles.SaveProfiles();
	}
	LabelRight.HSplitTop(5.0f, nullptr, &LabelRight);

	static int s_AllowDelete;
	DoButton_CheckBoxAutoVMarginAndSet(&s_AllowDelete, Localizable("Enable Deleting"), &s_AllowDelete, &LabelRight, LineSize);
	LabelRight.HSplitTop(5.0f, nullptr, &LabelRight);

	if(s_AllowDelete)
	{
		LabelRight.HSplitTop(28.0f, &Button, &LabelRight);
		static CButtonContainer s_DeleteButton;
		if(DoButton_Menu(&s_DeleteButton, Localize("Delete"), 0, &Button))
		{
			if(s_SelectedProfile != -1 && s_SelectedProfile < (int)GameClient()->m_SkinProfiles.m_Profiles.size())
			{
				GameClient()->m_SkinProfiles.m_Profiles.erase(GameClient()->m_SkinProfiles.m_Profiles.begin() + s_SelectedProfile);
				GameClient()->m_SkinProfiles.SaveProfiles();
			}
		}
		LabelRight.HSplitTop(5.0f, nullptr, &LabelRight);

		LabelRight.HSplitTop(28.0f, &Button, &LabelRight);
		static CButtonContainer s_OverrideButton;
		if(DoButton_Menu(&s_OverrideButton, Localize("Override"), 0, &Button))
		{
			if(s_SelectedProfile != -1 && s_SelectedProfile < (int)GameClient()->m_SkinProfiles.m_Profiles.size())
			{
				GameClient()->m_SkinProfiles.m_Profiles[s_SelectedProfile] = CProfile(
					DoColors ? *pColorBody : -1,
					DoColors ? *pColorFeet : -1,
					DoFlag ? CurrentFlag : -2,
					DoEmote ? Emote : -1,
					DoSkin ? pSkinName : "",
					DoName ? aName : "",
					DoClan ? aClan : "");
				GameClient()->m_SkinProfiles.SaveProfiles();
			}
		}
	}

	//---RENDER THE SELECTOR---
	CUIRect FileButton;
	CUIRect SelectorRect;
	MainView.HSplitTop(50.0f, nullptr, &SelectorRect);
	SelectorRect.HSplitBottom(LineSize, &SelectorRect, &FileButton);
	SelectorRect.HSplitBottom(MarginSmall, &SelectorRect, nullptr);
	std::vector<CProfile> *pProfileList = &GameClient()->m_SkinProfiles.m_Profiles;

	static CListBox s_ListBox;
	s_ListBox.DoStart(50.0f, pProfileList->size(), 4, 3, s_SelectedProfile, &SelectorRect, true);

	static bool s_Indexs[1024];

	for(size_t i = 0; i < pProfileList->size(); ++i)
	{
		CProfile CurrentProfile = GameClient()->m_SkinProfiles.m_Profiles[i];

		char RenderSkin[24];
		if(strlen(CurrentProfile.SkinName) == 0)
			str_copy(RenderSkin, pSkinName, sizeof(RenderSkin));
		else
			str_copy(RenderSkin, CurrentProfile.SkinName, sizeof(RenderSkin));

		const CSkin *pSkinToBeDraw = m_pClient->m_Skins.Find(RenderSkin);

		CListboxItem Item = s_ListBox.DoNextItem(&s_Indexs[i], s_SelectedProfile >= 0 && (size_t)s_SelectedProfile == i);

		if(!Item.m_Visible)
			continue;

		if(Item.m_Visible)
		{
			CTeeRenderInfo Info;
			Info.m_ColorBody = color_cast<ColorRGBA>(ColorHSLA(CurrentProfile.BodyColor).UnclampLighting(ColorHSLA::DARKEST_LGT));
			Info.m_ColorFeet = color_cast<ColorRGBA>(ColorHSLA(CurrentProfile.FeetColor).UnclampLighting(ColorHSLA::DARKEST_LGT));
			Info.m_CustomColoredSkin = true;
			Info.m_OriginalRenderSkin = pSkinToBeDraw->m_OriginalSkin;
			Info.m_ColorableRenderSkin = pSkinToBeDraw->m_ColorableSkin;
			Info.m_SkinMetrics = pSkinToBeDraw->m_Metrics;
			Info.m_Size = 50.0f;
			if(CurrentProfile.BodyColor == -1 && CurrentProfile.FeetColor == -1)
			{
				Info.m_CustomColoredSkin = m_Dummy ? g_Config.m_ClDummyUseCustomColor : g_Config.m_ClPlayerUseCustomColor;
				Info.m_ColorBody = ColorRGBA(1.0f, 1.0f, 1.0f);
				Info.m_ColorFeet = ColorRGBA(1.0f, 1.0f, 1.0f);
			}

			CRenderTools::GetRenderTeeOffsetToRenderedTee(pIdleState, &Info, OffsetToMid);

			int RenderEmote = CurrentProfile.Emote == -1 ? Emote : CurrentProfile.Emote;
			TeeRenderPos = vec2(Item.m_Rect.x + 30.0f, Item.m_Rect.y + Item.m_Rect.h / 2.0f + OffsetToMid.y);

			Item.m_Rect.VSplitLeft(60.0f, nullptr, &Item.m_Rect);
			CUIRect PlayerRect, ClanRect, FeetColorSquare, BodyColorSquare;

			Item.m_Rect.VSplitLeft(60.0f, nullptr, &BodyColorSquare); // Delete this maybe

			Item.m_Rect.VSplitRight(60.0f, &BodyColorSquare, &FlagRect);
			BodyColorSquare.x -= 11.0f;
			BodyColorSquare.VSplitLeft(10.0f, &BodyColorSquare, nullptr);
			BodyColorSquare.HSplitMid(&BodyColorSquare, &FeetColorSquare);
			BodyColorSquare.HSplitMid(nullptr, &BodyColorSquare);
			FeetColorSquare.HSplitMid(&FeetColorSquare, nullptr);
			FlagRect.HSplitBottom(10.0f, &FlagRect, nullptr);
			FlagRect.HSplitTop(10.0f, nullptr, &FlagRect);

			Item.m_Rect.HSplitMid(&PlayerRect, &ClanRect);

			SLabelProperties Props;
			Props.m_MaxWidth = Item.m_Rect.w;
			if(CurrentProfile.CountryFlag != -2)
				m_pClient->m_CountryFlags.Render(CurrentProfile.CountryFlag, Color, FlagRect.x, FlagRect.y, FlagRect.w, FlagRect.h);

			if(CurrentProfile.BodyColor != -1 && CurrentProfile.FeetColor != -1)
			{
				ColorRGBA BodyColor = color_cast<ColorRGBA>(ColorHSLA(CurrentProfile.BodyColor).UnclampLighting(ColorHSLA::DARKEST_LGT));
				ColorRGBA FeetColor = color_cast<ColorRGBA>(ColorHSLA(CurrentProfile.FeetColor).UnclampLighting(ColorHSLA::DARKEST_LGT));

				Graphics()->TextureClear();
				Graphics()->QuadsBegin();
				Graphics()->SetColor(BodyColor.r, BodyColor.g, BodyColor.b, 1.0f);
				IGraphics::CQuadItem Quads[2];
				Quads[0] = IGraphics::CQuadItem(BodyColorSquare.x, BodyColorSquare.y, BodyColorSquare.w, BodyColorSquare.h);
				Graphics()->QuadsDrawTL(&Quads[0], 1);
				Graphics()->SetColor(FeetColor.r, FeetColor.g, FeetColor.b, 1.0f);
				Quads[1] = IGraphics::CQuadItem(FeetColorSquare.x, FeetColorSquare.y, FeetColorSquare.w, FeetColorSquare.h);
				Graphics()->QuadsDrawTL(&Quads[1], 1);
				Graphics()->QuadsEnd();
			}
			RenderTools()->RenderTee(pIdleState, &Info, RenderEmote, vec2(1.0f, 0.0f), TeeRenderPos);

			if(strlen(CurrentProfile.Name) == 0 && strlen(CurrentProfile.Clan) == 0)
			{
				PlayerRect = Item.m_Rect;
				PlayerRect.y += MarginSmall;
				Ui()->DoLabel(&PlayerRect, CurrentProfile.SkinName, FontSize, TEXTALIGN_ML, Props);
			}
			else
			{
				Ui()->DoLabel(&PlayerRect, CurrentProfile.Name, FontSize, TEXTALIGN_ML, Props);
				Item.m_Rect.HSplitTop(LineSize, nullptr, &Item.m_Rect);
				Props.m_MaxWidth = Item.m_Rect.w;
				Ui()->DoLabel(&ClanRect, CurrentProfile.Clan, FontSize, TEXTALIGN_ML, Props);
			}
		}
	}

	const int NewSelected = s_ListBox.DoEnd();
	if(s_SelectedProfile != NewSelected)
	{
		s_SelectedProfile = NewSelected;
	}
	static CButtonContainer s_ProfilesFile;
	FileButton.VSplitLeft(130.0f, &FileButton, nullptr);
	if(DoButton_Menu(&s_ProfilesFile, Localize("Profiles file"), 0, &FileButton))
	{
		Storage()->GetCompletePath(IStorage::TYPE_SAVE, PROFILES_FILE, aTempBuf, sizeof(aTempBuf));
		Client()->ViewFile(aTempBuf);
	}
}

//TODO: Add everything to here
