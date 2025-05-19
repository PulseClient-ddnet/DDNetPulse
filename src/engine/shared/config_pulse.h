/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */

// This file can be included several times.

#ifndef MACRO_CONFIG_INT
#error "The config macros must be defined"
#define MACRO_CONFIG_INT(Name, ScriptName, Def, Min, Max, Save, Desc) ;
#define MACRO_CONFIG_COL(Name, ScriptName, Def, Save, Desc) ;
#define MACRO_CONFIG_STR(Name, ScriptName, Len, Def, Save, Desc) ;
#endif

MACRO_CONFIG_INT(ClDebug, p_debug, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Apply skin in profiles")

MACRO_CONFIG_INT(ClApplyProfileSkin, p_profile_skin, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Apply skin in profiles")
MACRO_CONFIG_INT(ClApplyProfileName, p_profile_name, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Apply name in profiles")
MACRO_CONFIG_INT(ClApplyProfileClan, p_profile_clan, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Apply clan in profiles")
MACRO_CONFIG_INT(ClApplyProfileFlag, p_profile_flag, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Apply flag in profiles")
MACRO_CONFIG_INT(ClApplyProfileColors, p_profile_colors, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Apply colors in profiles")
MACRO_CONFIG_INT(ClApplyProfileEmote, p_profile_emote, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Apply emote in profiles")

MACRO_CONFIG_STR(ClCustomConsoleDefault, p_console_asset_default, 50, "default", CFGFLAG_CLIENT | CFGFLAG_SAVE, "Default console background image")
MACRO_CONFIG_STR(ClCustomConsoleRcon, p_console_asset_rcon, 50, "default", CFGFLAG_CLIENT | CFGFLAG_SAVE, "RCON console background image")

MACRO_CONFIG_INT(ClCustomConsole, p_custom_console, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")
MACRO_CONFIG_STR(ClAssetConsole, p_console_asset, 50, "default", CFGFLAG_SAVE | CFGFLAG_CLIENT, "")
MACRO_CONFIG_INT(ClCustomConsoleFading, p_custom_console_fading, 75, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClCustomConsoleAlpha, p_custom_console_alpha, 100, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClCustomConsoleRconAlpha, p_custom_console_rcon_alpha, 100, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "RCON console background alpha")
MACRO_CONFIG_INT(ClCustomConsoleRconFading, p_custom_console_rcon_fading, 0, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "RCON console background fading")

MACRO_CONFIG_INT(ClFastInp, p_fast_input, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")

MACRO_CONFIG_INT(ClHoverMessages, p_hover_message, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClHoverMessagesHistory, p_hover_messages_history, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClHoverMessagesMaxHistory, p_hover_message_max_history, 15, 1, 40, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClHoverMessagesMaxNotifications, p_hover_message_max_notifications, 10, 1, 40, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")

MACRO_CONFIG_INT(ClTrailStyle, p_toggle_trails, 0, 0, 2, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClPlayerIdleAura, p_idle_aura, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClPlayerIdleAuraTimer, p_idle_aura_timer, 2, 2, 30, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClToggleAura, p_toggle_aura, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")

MACRO_CONFIG_INT(ClLaserGlowIntensity, p_laser_ench, 0, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClBetterLasers, p_better_laser, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")

// All DClient variables are defined here

MACRO_CONFIG_INT(ClShowFlags, dc_show_flags, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Tee stats (Deep/Jetpack/etc)")
MACRO_CONFIG_INT(ClShowFlagsSize, dc_show_flags_size, 30, -50, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Size of tee stat indicators")
MACRO_CONFIG_INT(ClGrenadePath, dc_grenade_path, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Grenade path prediction")
MACRO_CONFIG_INT(ClLaserPath, dc_laser_path, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Laser path prediction")
MACRO_CONFIG_INT(ClShowDJ, dc_show_jumps, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show remaining double jumps of a tee")
MACRO_CONFIG_INT(ClShowJumpsSize, dc_show_jumps_size, 30, -50, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Size of double jump indicators")
MACRO_CONFIG_INT(ClFreeMouse, dc_free_mouse, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Free mouse mode (WIP)")
MACRO_CONFIG_INT(ClUnlockZoom, dc_unlock_zoom, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Disable camera zoom lock")
