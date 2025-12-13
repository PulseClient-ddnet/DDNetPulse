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

MACRO_CONFIG_INT(ClCustomConsole, p_custom_console, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")
MACRO_CONFIG_STR(ClAssetConsole, p_console_asset, 50, "default", CFGFLAG_SAVE | CFGFLAG_CLIENT, "")
MACRO_CONFIG_INT(ClCustomConsoleFading, p_custom_console_fading, 75, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClCustomConsoleAlpha, p_custom_console_alpha, 100, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClCustomConsoleRconAlpha, p_custom_console_rcon_alpha, 100, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "RCON console background alpha")
MACRO_CONFIG_INT(ClCustomConsoleRconFading, p_custom_console_rcon_fading, 0, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "RCON console background fading")

MACRO_CONFIG_STR(ClAudioPack, cl_audio_pack, 256, "", CFGFLAG_SAVE | CFGFLAG_CLIENT, "Custom audio pack folder name (empty for default, place packs in $USERDIR/audio_packs/<name>/audio/)")

MACRO_CONFIG_INT(ClFastInp, p_fast_input, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClFastInpOthers, p_fast_input_others, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")

MACRO_CONFIG_INT(ClHoverMessages, p_hover_message, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClHoverMessagesHistory, p_hover_messages_history, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClHoverMessagesMaxHistory, p_hover_message_max_history, 15, 1, 40, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClHoverMessagesMaxNotifications, p_hover_message_max_notifications, 10, 1, 40, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")

MACRO_CONFIG_INT(ClTrailStyle, p_toggle_trails, 0, 0, 3, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Player trail style (0 = off, 1 = sparkle, 2 = ring, 3 = rainbow)")
MACRO_CONFIG_INT(ClPlayerIdleAura, p_idle_aura, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")

MACRO_CONFIG_INT(ClPlayerSquashStretch, p_squash_stretch, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Enable squash and stretch animation for player sprites based on velocity")

MACRO_CONFIG_INT(ClPlayerIdleAuraTimer, p_idle_aura_timer, 2, 2, 30, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClIdleAuraType, p_idle_aura_type, 1, 1, 5, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClToggleAuraParticlesNum, p_toggle_aura_parsicle_num, 12, 0, 300, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClToggleAuraRadius, p_toggle_aura_radius, 30, 0, 600, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")

MACRO_CONFIG_INT(ClLaserGlowIntensity, p_laser_ench, 0, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClBetterLasers, p_better_laser, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")

// All DClient variables are defined here
MACRO_CONFIG_INT(ClShowFlags, cl_show_flags, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Tee stats (Deep/Jetpack/etc)")
MACRO_CONFIG_INT(ClShowFlagsSize, cl_show_flags_size, 30, -50, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Size of tee stat indicators")
MACRO_CONFIG_INT(ClShowDJ, cl_show_jumps, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show remaining double jumps of a tee")
MACRO_CONFIG_INT(ClShowJumpsSize, cl_show_jumps_size, 30, -50, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Size of double jump indicators")
//MACRO_CONFIG_INT(ClFreeMouse, dc_free_mouse, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Free mouse mode (WIP)")
//MACRO_CONFIG_INT(ClUnlockZoom, dc_unlock_zoom, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Disable camera zoom lock")

//Effects controls
MACRO_CONFIG_INT(ClFreezeSnowFlakes, p_effect_freeze_snowflakes, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "toggles snowflakes effect")
MACRO_CONFIG_INT(ClHammerHitEffect, p_effect_hammerhit, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "toggles hammer hit effect")
MACRO_CONFIG_INT(ClHammerHitEffectSound, p_effect_sound_hammerhit, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "toggles hammer hit effect")
MACRO_CONFIG_INT(ClJumpEffect, p_effect_jump, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "toggles hammer hit effect")
MACRO_CONFIG_INT(ClJumpEffectSound, p_effect_sound_jump, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "toggles jump effect sound")

//Scoreboard
MACRO_CONFIG_COL(ClFoeColor, p_blacklist_p_color, 65457, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Blacklist players color")
MACRO_CONFIG_COL(ClFriendColor, p_friend_color, 7995321, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Friends color")

// Focus Mode Settings
MACRO_CONFIG_INT(ClFocusMode, p_focus_mode, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Enable focus mode to minimize visual distractions")
MACRO_CONFIG_INT(ClFocusModeHideNames, p_focus_mode_hide_names, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Hide player names in focus mode")
MACRO_CONFIG_INT(ClFocusModeHideEffects, p_focus_mode_hide_effects, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Hide visual effects in focus mode")
MACRO_CONFIG_INT(ClFocusModeHideUI, p_focus_mode_hide_ui, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Hide unnecessary UI elements in focus mode")
MACRO_CONFIG_INT(ClFocusModeHideChat, p_focus_mode_hide_chat, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Hide chat in focus mode")
MACRO_CONFIG_INT(ClFocusModeHideScoreboard, p_focus_mode_hide_scoreboard, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Hide scoreboard in focus mode")

// deatch phrases
MACRO_CONFIG_INT(ClDeathPhrases, p_death_phrases, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Enable death phrases")

//cursor
MACRO_CONFIG_INT(ClWeaponCursorSize, cl_weapon_cursor_size, 64, 16, 128, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Size of the in-game weapon cursor")
MACRO_CONFIG_INT(ClAntiRQ, cl_anti_rq_mode, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "prevent rege-quitting")

//Chat
MACRO_CONFIG_INT(ClCrossChatAutoConnect, cl_cross_chat_autoconnect, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Automatically connect to CrossChat on startup")
MACRO_CONFIG_INT(ClCrossChatInGameChat, cl_cross_chat_ingame_chat, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Enable CrossChat integration with in-game chat")
MACRO_CONFIG_INT(ClCrossChatDebug, cl_cross_chat_debug, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Enable CrossChat debug messages")

//pulse server
MACRO_CONFIG_STR(ClSocketNameserver, ntwr_nameserver, 256, "http://localhost:3000", CFGFLAG_CLIENT | CFGFLAG_SAVE, "Nameserver for websockets") //TODO: actaual nameserver
