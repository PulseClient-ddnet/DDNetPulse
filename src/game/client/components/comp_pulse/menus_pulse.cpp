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
#include <game/client/components/comp_pulse/menus_macro.h>
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

typedef struct
{
	const char *m_pName;
	const char *m_pCommand;
	int m_KeyId;
	int m_ModifierCombination;
} CKeyInfo;


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
					else { dbg_msg("console_images", "Failed to load image: %s", aFullPath); }
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

			str_copy(g_Config.m_ClAssetConsole, a_cleanName, sizeof(g_Config.m_ClAssetConsole));

			//dbg_msg("console_images", "Selected image: %s for %s console", a_cleanName, s_SelectedConsoleType == 0 ? "default" : "RCON");
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
		if(DoButton_MenuTab(&s_aPageTabs[Tab], apTabNames[Tab], s_CurTab == Tab, &Button, Corners, nullptr, nullptr, nullptr, nullptr, 4.0f)) { s_CurTab = Tab; }
	}

	if(s_CurTab == PULSE_TAB_GLOBAL)
	{
		// scrollable controls
		static CScrollRegion s_ScrollRegion;
		vec2 ScrollOffset(0.0f, 0.0f);
		CScrollRegionParams ScrollParams;
		ScrollParams.m_ScrollUnit = 120.0f;
		s_ScrollRegion.Begin(&MainView, &ScrollOffset, &ScrollParams);
		//s_ScrollRegion.AddRect(MainView);
		MainView.y += ScrollOffset.y;

		// Add some spacing at the top
		MainView.HSplitTop(10.0f, nullptr, &MainView);

		// Create two sections side by side
		CUIRect LeftSection, RightSection;
		MainView.VSplitMid(&LeftSection, &RightSection, 10.0f);

		ColorRGBA defaultCol = ColorRGBA(0, 0, 0, 0.45f);

		// Left Section - Gameplay Settings
		{
			CUIRect Section, Label;

			CUIRect GameplayRect;
			DRAW_BOX(LeftSection, GameplayRect, 100.0f, defaultCol, 10.0f);
			s_ScrollRegion.AddRect(GameplayRect);
			BOX_LABEL(GameplayRect, Section, "Input & Predict", 40.0f, 10.0f);
			BUTTON(GameplayRect, Button, 20.0f, &g_Config.m_ClFastInp, "Fast Input", g_Config.m_ClFastInp);
			BUTTON(GameplayRect, Button, 20.0f, &g_Config.m_ClFastInpOthers, "Fast Input Others", g_Config.m_ClFastInpOthers);
			END_LABEL(LeftSection);

			// Laser Settings
			CUIRect LaserRect;
			DRAW_BOX(LeftSection, LaserRect, 260.0f, defaultCol, 10.0f);
			s_ScrollRegion.AddRect(LaserRect);
			BOX_LABEL(LaserRect, Section, "Laser Settings", 40.0f, 10.0f);

			// RTX Laser
			BUTTON(LaserRect, Button, 20.0f, &g_Config.m_ClBetterLasers, "Enhanced Laser Effects", g_Config.m_ClBetterLasers);

			if(g_Config.m_ClBetterLasers)
			{
				// Laser Glow Intensity
				LaserRect.HSplitTop(20.0f, &Button, &LaserRect);
				Ui()->DoScrollbarOption(&g_Config.m_ClLaserGlowIntensity, &g_Config.m_ClLaserGlowIntensity, &Button, Localize("Laser Glow Intensity"), 30, 100);

				LaserRect.HSplitTop(20.0f, &Button, &LaserRect);
				// Laser Preview
				BOX_LABEL(LaserRect, Label, "Laser Preview", 20.0f, 10.0f);
				s_ScrollRegion.AddRect(LaserRect);

				const float LaserPreviewHeight = 50.0f;
				CUIRect LaserPreview;
				LASER_PREVIEW(LaserRect, LaserPreview, LaserPreviewHeight, g_Config.m_ClLaserRifleInnerColor, g_Config.m_ClLaserRifleOutlineColor, LASERTYPE_RIFLE);
				LASER_PREVIEW(LaserRect, LaserPreview, LaserPreviewHeight, g_Config.m_ClLaserShotgunInnerColor, g_Config.m_ClLaserShotgunOutlineColor, LASERTYPE_SHOTGUN);
			}
			END_LABEL(LeftSection);

			CUIRect EffectsRect;
			// Effects Settings Box
			DRAW_BOX(LeftSection, EffectsRect, 160.0f, defaultCol, 10.0f);
			s_ScrollRegion.AddRect(EffectsRect);
			BOX_LABEL(EffectsRect, Section, "Effects Settings", 40.0f, 10.0f);
			BUTTON(EffectsRect, Button, 20.0f, &g_Config.m_ClFreezeSnowFlakes, "Show Freeze Snowflakes", g_Config.m_ClFreezeSnowFlakes);
			BUTTON(EffectsRect, Button, 20.0f, &g_Config.m_ClHammerHitEffect, "Hammer Hit Effects", g_Config.m_ClHammerHitEffect);
			BUTTON(EffectsRect, Button, 20.0f, &g_Config.m_ClHammerHitEffectSound, "Hammer Hit Sound Effects", g_Config.m_ClHammerHitEffectSound);
			BUTTON(EffectsRect, Button, 20.0f, &g_Config.m_ClJumpEffect, "Jump Effects", g_Config.m_ClJumpEffect);
			BUTTON(EffectsRect, Button, 20.0f, &g_Config.m_ClJumpEffectSound, "Jump Sound Effects", g_Config.m_ClJumpEffectSound);
			END_LABEL(LeftSection);

			// Player Aura Settings
			CUIRect PlayerAuraRect;
			DRAW_BOX(LeftSection, PlayerAuraRect, 160.0f, defaultCol, 10.0f);
			s_ScrollRegion.AddRect(PlayerAuraRect);
			BOX_LABEL(PlayerAuraRect, Section, "Player Aura Settings", 40.0f, 10.0f);
			BUTTON(PlayerAuraRect, Button, 20.0f, &g_Config.m_ClPlayerIdleAura, "Idle Player Aura", g_Config.m_ClPlayerIdleAura);
			if(g_Config.m_ClPlayerIdleAura)
			{
				SCROLLBAR(PlayerAuraRect, Button, 20.0f, &g_Config.m_ClPlayerIdleAuraTimer, 2, 30, "Aura Timer (seconds)");
				SCROLLBAR(PlayerAuraRect, Button, 20.0f, &g_Config.m_ClToggleAuraParticlesNum, 0, 300, "Particles Number");
				SCROLLBAR(PlayerAuraRect, Button, 20.0f, &g_Config.m_ClToggleAuraRadius, 0, 600, "Aura Radius");
			}

			if(DoLine_RadioMenu(PlayerAuraRect, Localize("Idle Aura Pattern"),
				   MVButtonContainersIdleAura,
				   {Localize("Starfall"), Localize("Spiral"), Localize("Clock"), Localize("Peace"), Localize("Circles")},
				   {1, 2, 3, 4, 5},
				   g_Config.m_ClIdleAuraType)){}

			END_LABEL(LeftSection);
			CUIRect FunRect;
			DRAW_BOX(LeftSection, FunRect, 80.0f, defaultCol, 10.0f);
			s_ScrollRegion.AddRect(FunRect);
			BOX_LABEL(FunRect, Section, "Fun Settings", 40.0f, 10.0f);
			BUTTON(FunRect, Button, 20.0f, &g_Config.m_ClDeathPhrases, "Funny death phrases on death", g_Config.m_ClDeathPhrases);
			END_LABEL(LeftSection);
		}

		// Right Section - Visual Effects
		{
			CUIRect Section, Label;
			CUIRect PreferencesRect;


			// Preferences
			DRAW_BOX(RightSection, PreferencesRect, 80.0f, defaultCol, 10.0f);
			s_ScrollRegion.AddRect(PreferencesRect);
			BOX_LABEL(PreferencesRect, Section, "Preferences", 40.0f, 10.0f);
			BUTTON(PreferencesRect, Button, 20.0f, &g_Config.m_ClAntiRQ, "Prevent alt+f4 quit", g_Config.m_ClAntiRQ);
			END_LABEL(RightSection);

			// Player Effects Box
			CUIRect VisualRect;
			DRAW_BOX(RightSection, VisualRect, 80.0f, defaultCol, 10.0f);
			s_ScrollRegion.AddRect(VisualRect);
			BOX_LABEL(VisualRect, Section, "Player Effects", 40.0f, 10.0f);
			BUTTON(VisualRect, Button, 20.0f, &g_Config.m_ClPlayerSquashStretch, "Squash & Stretch Animation (beta)", g_Config.m_ClPlayerSquashStretch);
			END_LABEL(RightSection);

			// Hover Messages Box
			CUIRect HoverRect;
			DRAW_BOX(RightSection, HoverRect, 140.0f, defaultCol, 10.0f);
			s_ScrollRegion.AddRect(HoverRect);
			BOX_LABEL(HoverRect, Section, "Hover Messages", 40.0f, 10.0f);
			BUTTON(HoverRect, Button, 20.0f, &g_Config.m_ClHoverMessages, "Hover Messages", g_Config.m_ClHoverMessages);

			if(g_Config.m_ClHoverMessages)
			{
				BUTTON(HoverRect, Button, 20.0f, &g_Config.m_ClHoverMessagesHistory, "Message History", g_Config.m_ClHoverMessagesHistory);
				SCROLLBAR(HoverRect, Button, 20.0f, &g_Config.m_ClHoverMessagesMaxHistory, 1, 40, "Max History");
				SCROLLBAR(HoverRect, Button, 20.0f, &g_Config.m_ClHoverMessagesMaxNotifications, 1, 40, "Max Notifications");
			}
			END_LABEL(RightSection);

			CUIRect DuckRect;
			// Duck Settings Box
			DRAW_BOX(RightSection, DuckRect, 140.0f, defaultCol, 10.0f);
			s_ScrollRegion.AddRect(DuckRect);
			BOX_LABEL(DuckRect, Section, "Duck Settings", 40.0f, 10.0f);
			BUTTON(DuckRect, Button, 20.0f, &g_Config.m_ClShowFlags, "Show Nameplates Flags (Deep/Jetpack/etc)", g_Config.m_ClShowFlags);
			if(g_Config.m_ClShowFlags) { SCROLLBAR(DuckRect, Button, 20.0f, &g_Config.m_ClShowFlagsSize, -50, 100, "Nameplates Flags Size"); }
			BUTTON(DuckRect, Button, 20.0f, &g_Config.m_ClShowDJ, "Show Double Jumps", g_Config.m_ClShowDJ);
			if(g_Config.m_ClShowDJ) { SCROLLBAR(DuckRect, Button, 20.0f, &g_Config.m_ClShowJumpsSize, -50, 100, "Double Jumps Size"); }
			END_LABEL(RightSection);

			CUIRect ScoreboardRect;
			// Scoreboard Settings Box
			DRAW_BOX(RightSection, ScoreboardRect, 140.0f, defaultCol, 10.0f);
			s_ScrollRegion.AddRect(ScoreboardRect);
			BOX_LABEL(ScoreboardRect, Section, "Scoreboard Settings", 40.0f, 10.0f);
			static CButtonContainer s_FoeColor, s_FriendColor;
			ColorRGBA DefaultCol = ColorRGBA(1.0f, 1.0f, 1.0f, 1.f);
			COLORPICKER(s_FoeColor, ScoreboardRect, "Foe Color In Scoreboard", &g_Config.m_ClFoeColor, DefaultCol);
			COLORPICKER(s_FriendColor, ScoreboardRect, "Friend Color In Scoreboard", &g_Config.m_ClFriendColor, DefaultCol);

			CKeyInfo Key;
			FIND_KEY_BINDING(Key, "toggle_scoreboard_menu");

			CUIRect KeyLabel;
			ScoreboardRect.HSplitTop(20.0f, &Button, &ScoreboardRect);
			Button.VSplitLeft(120.0f, &KeyLabel, &Button);
			Button.VSplitLeft(100.0f, &Button, nullptr);
			char aBuf[64];
			str_format(aBuf, sizeof(aBuf), "%s:", Localize((const char *)Key.m_pName));

			Ui()->DoLabel(&KeyLabel, aBuf, FontSize, TEXTALIGN_ML);
			int OldId = Key.m_KeyId, OldModifierCombination = Key.m_ModifierCombination, NewModifierCombination;
			int NewId = DoKeyReader((void *)&Key.m_pName, &Button, OldId, OldModifierCombination, &NewModifierCombination);
			if(NewId != OldId || NewModifierCombination != OldModifierCombination)
			{
				if(OldId != 0 || NewId == 0)
					GameClient()->m_Binds.Bind(OldId, "", false, OldModifierCombination);
				if(NewId != 0)
					GameClient()->m_Binds.Bind(NewId, Key.m_pCommand, false, NewModifierCombination);
			}
			ScoreboardRect.HSplitTop(5.0f, nullptr, &ScoreboardRect);
			END_LABEL(RightSection);

			// Focus Mode Settings Box
			CUIRect FocusModeRect;

			if(g_Config.m_ClFocusMode) { DRAW_BOX(RightSection, FocusModeRect, 180.0f, defaultCol, 10.0f); }
			else { DRAW_BOX(RightSection, FocusModeRect, 80.0f, defaultCol, 10.0f); }

			s_ScrollRegion.AddRect(FocusModeRect);
			BOX_LABEL(FocusModeRect, Section, "Focus Mode Settings", 40.0f, 10.0f);
			BUTTON(FocusModeRect, Button, 20.0f, &g_Config.m_ClFocusMode, "Enable Focus Mode", g_Config.m_ClFocusMode);

			if(g_Config.m_ClFocusMode)
			{
				BUTTON(FocusModeRect, Button, 20.0f, &g_Config.m_ClFocusModeHideNames, "Hide Player Names", g_Config.m_ClFocusModeHideNames);
				BUTTON(FocusModeRect, Button, 20.0f, &g_Config.m_ClFocusModeHideEffects, "Hide Visual Effects", g_Config.m_ClFocusModeHideEffects);
				BUTTON(FocusModeRect, Button, 20.0f, &g_Config.m_ClFocusModeHideUI, "Hide Unnecessary UI", g_Config.m_ClFocusModeHideUI);
				BUTTON(FocusModeRect, Button, 20.0f, &g_Config.m_ClFocusModeHideChat, "Hide Chat", g_Config.m_ClFocusModeHideChat);
				BUTTON(FocusModeRect, Button, 20.0f, &g_Config.m_ClFocusModeHideScoreboard, "Hide Scoreboard", g_Config.m_ClFocusModeHideScoreboard);
			}
			END_LABEL(RightSection);

			CUIRect CursorModeRect;
			DRAW_BOX(RightSection, CursorModeRect, 140.0f, defaultCol, 10.0f);
			s_ScrollRegion.AddRect(CursorModeRect);
			BOX_LABEL(CursorModeRect, Section, "Cursor Settings", 40.0f, 10.0f);
			SCROLLBAR(CursorModeRect, Button, 20.0f, &g_Config.m_ClPlayerIdleAuraTimer, 2, 30, "Aura Timer (seconds)"); //#define SCROLLBAR(section, button, height, value, min, max, text) \


			float multiplier = g_Config.m_ClWeaponCursorSize / 64.0f;
			char aMultiplierText[16];
			str_format(aMultiplierText, sizeof(aMultiplierText), "  (%.1fx)", multiplier);
			CursorModeRect.HSplitTop(20.0f, &Button, &CursorModeRect);
			Ui()->DoScrollbarOption(&g_Config.m_ClWeaponCursorSize, &g_Config.m_ClWeaponCursorSize, &Button, Localize("Cursor scaling"), 16, 128, &CUi::ms_LinearScrollbarScale, 0, aMultiplierText);

			END_LABEL(RightSection);

			s_ScrollRegion.End();
		}
	}

	else if(s_CurTab == PULSE_TAB_CONSOLE)
	{
		CUIRect Left, Right;
		MainView.VSplitMid(&Left, &Right);
		CUIRect ConsoleRect;
		DRAW_BOX(Left, ConsoleRect, 200.0f, ColorRGBA(0.0f, 0.0f, 0.0f, 0.3f), 10.0f);

		CUIRect Section;
		BOX_LABEL(ConsoleRect, Section, "Console Settings", 40.0f, 10.0f);
		BUTTON(ConsoleRect, Button, 20.0f, &g_Config.m_ClCustomConsole, "Toggle Custom Console", g_Config.m_ClCustomConsole);
		if(g_Config.m_ClCustomConsole)
		{
			Right.Draw(ColorRGBA(0.0f, 0.0f, 0.0f, 0.3f), IGraphics::CORNER_ALL, 5.0f);
			RenderConsoleImages(Right);

			SCROLLBAR(ConsoleRect, Button, 20.0f, &g_Config.m_ClCustomConsoleAlpha, 0, 100, "Console Alpha");
			SCROLLBAR(ConsoleRect, Button, 20.0f, &g_Config.m_ClCustomConsoleFading, 100, 0, "Console Brightness");

			CUIRect OpenFolderButton;
			ConsoleRect.HSplitTop(20.0f, &OpenFolderButton, &ConsoleRect);
			static CButtonContainer s_OpenFolderButton;
			if(DoButton_Menu(&s_OpenFolderButton, Localize("Open Console Assets Folder"), 0, &OpenFolderButton))
			{
				char aPath[IO_MAX_PATH_LENGTH];
				Storage()->GetCompletePath(IStorage::TYPE_SAVE, "pulse/assets/console", aPath, sizeof(aPath));
				Client()->ViewFile(aPath);
			}
		}
		END_LABEL(Left);
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
	const CSkin *pSkin = GameClient()->m_Skins.Find(pSkinName);
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
	GameClient()->m_CountryFlags.Render(m_Dummy ? g_Config.m_ClDummyCountry : g_Config.m_PlayerCountry, Color, FlagRect.x, FlagRect.y, FlagRect.w, FlagRect.h);

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
			const CSkin *pLoadSkin = GameClient()->m_Skins.Find(LoadProfile.SkinName);
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
		GameClient()->m_CountryFlags.Render(RenderFlag, Color, FlagRect.x, FlagRect.y, FlagRect.w, FlagRect.h);

		str_format(aName, sizeof(aName), "%s", m_Dummy ? g_Config.m_ClDummyName : g_Config.m_PlayerName);
		str_format(aClan, sizeof(aClan), "%s", m_Dummy ? g_Config.m_ClDummyClan : g_Config.m_PlayerClan);
	}
	else { MainView.HSplitTop(80.0f, nullptr, &MainView); }

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

		const CSkin *pSkinToBeDraw = GameClient()->m_Skins.Find(RenderSkin);

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
				GameClient()->m_CountryFlags.Render(CurrentProfile.CountryFlag, Color, FlagRect.x, FlagRect.y, FlagRect.w, FlagRect.h);

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
	if(s_SelectedProfile != NewSelected) { s_SelectedProfile = NewSelected; }
	static CButtonContainer s_ProfilesFile;
	FileButton.VSplitLeft(130.0f, &FileButton, nullptr);
	if(DoButton_Menu(&s_ProfilesFile, Localize("Profiles file"), 0, &FileButton))
	{
		Storage()->GetCompletePath(IStorage::TYPE_SAVE, PROFILES_FILE, aTempBuf, sizeof(aTempBuf));
		Client()->ViewFile(aTempBuf);
	}
}




void CMenus::RenderCrossChat(CUIRect MainView)
{
    if(!m_CrossChatInitialized)
    {
        m_CrossChatInitialized = true;
        m_CrossChatInput.Clear();
        m_CrossChatMessages.clear();
        // m_CrossChatMessages.emplace_back("[Server]: Добро пожаловать в CrossChat!");

    	char aBuf[64];
    	str_format(aBuf, sizeof(aBuf), "[Server]: Welcome to chat, %s", g_Config.m_PlayerName);
    	GameClient()->m_WebSocket.AddMessage(aBuf);
    }

    MainView.Draw(ColorRGBA(0, 0, 0, 0.5f), IGraphics::CORNER_ALL, 5.0f);

    CUIRect ChatView, InputBar;
    MainView.HSplitBottom(40.0f, &ChatView, &InputBar);
    ChatView.VMargin(10.0f, &ChatView);
    ChatView.HMargin(10.0f, &ChatView);
    InputBar.VSplitLeft(InputBar.w / 5, nullptr, &InputBar); // TODO: Add a button or something here later
    InputBar.VMargin(10.0f, &InputBar);
    InputBar.HMargin(10.0f, &InputBar);

    CUIRect LeftBar;
    ChatView.VSplitLeft(ChatView.w / 5, &LeftBar, &ChatView);
    LeftBar.Draw(ColorRGBA(0, 0, 0, 0.3f), IGraphics::CORNER_ALL, 5.0f);

    static CLineInputBuffered<64> s_ChatInput;
    s_ChatInput.SetEmptyText("Send Message");

    if(Ui()->DoEditBox(&s_ChatInput, &InputBar, FontSize + 2.0f))
        Ui()->SetActiveItem(&s_ChatInput);

    if(Input()->KeyPress(KEY_RETURN) || Input()->KeyPress(KEY_KP_ENTER))
    {
        const char *pText = s_ChatInput.GetString();
        if(pText && pText[0])
        {
            GameClient()->m_WebSocket.SendChatMessage(pText);
            s_ChatInput.Clear();
        }
    }

    std::lock_guard<std::mutex> lock(GameClient()->m_WebSocket.m_OnlinePlayersMutex);
    float yOnline = LeftBar.y + 10.0f;
    for(const auto &player : GameClient()->m_WebSocket.m_OnlinePlayers)
    {
        TextRender()->Text(LeftBar.x + 5.0f, yOnline, 12.0f, player.c_str(), -1);
        yOnline += 15.0f;
    }

    std::vector<std::string> Messages = GameClient()->m_WebSocket.GetMessages();
    const float LineHeight = 12.0f;
    float y = ChatView.y + ChatView.h - LineHeight;

    for(auto it = Messages.rbegin(); it != Messages.rend(); ++it)
    {
        std::stringstream ss(*it);
        std::string line;
        while(std::getline(ss, line, '\n'))
        {
            TextRender()->TextColor(1, 1, 1, 1);
            TextRender()->Text(ChatView.x + 10, y, 12.0f, line.c_str(), -1);
            y -= LineHeight + 2;

            if(y < ChatView.y)
                break;
        }

        if(y < ChatView.y)
            break; 
    }
}


//TODO: Add everything to here