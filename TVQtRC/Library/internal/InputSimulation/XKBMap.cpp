//********************************************************************************//
// MIT License                                                                    //
//                                                                                //
// Copyright (c) 2021 TeamViewer Germany GmbH                                     //
//                                                                                //
// Permission is hereby granted, free of charge, to any person obtaining a copy   //
// of this software and associated documentation files (the "Software"), to deal  //
// in the Software without restriction, including without limitation the rights   //
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      //
// copies of the Software, and to permit persons to whom the Software is          //
// furnished to do so, subject to the following conditions:                       //
//                                                                                //
// The above copyright notice and this permission notice shall be included in all //
// copies or substantial portions of the Software.                                //
//                                                                                //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    //
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         //
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  //
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  //
// SOFTWARE.                                                                      //
//********************************************************************************//
#include "XKBMap.h"

#include "xkbcommon/xkbcommon-keysyms.h"

#include <QtCore/QString>

#include <cstdio>

namespace tvqtsdk
{
// Modifier masks are adopted from Xorg protocol - X11/X.h: https://gitlab.freedesktop.org/xorg/proto/xorgproto/blob/master/include/X11/X.h
namespace XkbModMask
{
constexpr uint32_t Shift   = (1<<0);
constexpr uint32_t Lock    = (1<<1);
constexpr uint32_t Control = (1<<2);
constexpr uint32_t Mod1    = (1<<3);
constexpr uint32_t Mod2    = (1<<4);
constexpr uint32_t Mod3    = (1<<5);
constexpr uint32_t Mod4    = (1<<6);
constexpr uint32_t Mod5    = (1<<7);
}

// The implementation is adapted from http://code.qt.io/cgit/qt/qtbase.git/tree/src/plugins/platforms/xcb/qxcbkeyboard.cpp
// The applicable version hash is: 3733995a0d8aeae06e151c13ec4e2a6da8d88256

static const unsigned int KeyTbl[] = {
    // misc keys

    XKB_KEY_Escape,                  Qt::Key_Escape,
    XKB_KEY_Tab,                     Qt::Key_Tab,
    XKB_KEY_ISO_Left_Tab,            Qt::Key_Backtab,
    XKB_KEY_BackSpace,               Qt::Key_Backspace,
    XKB_KEY_Return,                  Qt::Key_Return,
    XKB_KEY_Insert,                  Qt::Key_Insert,
    XKB_KEY_Delete,                  Qt::Key_Delete,
    XKB_KEY_Clear,                   Qt::Key_Delete,
    XKB_KEY_Pause,                   Qt::Key_Pause,
    XKB_KEY_Print,                   Qt::Key_Print,
    0x1005FF60,                 Qt::Key_SysReq,         // hardcoded Sun SysReq
    0x1007ff00,                 Qt::Key_SysReq,         // hardcoded X386 SysReq

    // cursor movement

    XKB_KEY_Home,                    Qt::Key_Home,
    XKB_KEY_End,                     Qt::Key_End,
    XKB_KEY_Left,                    Qt::Key_Left,
    XKB_KEY_Up,                      Qt::Key_Up,
    XKB_KEY_Right,                   Qt::Key_Right,
    XKB_KEY_Down,                    Qt::Key_Down,
    XKB_KEY_Prior,                   Qt::Key_PageUp,
    XKB_KEY_Next,                    Qt::Key_PageDown,

    // modifiers

    XKB_KEY_Shift_L,                 Qt::Key_Shift,
    XKB_KEY_Shift_R,                 Qt::Key_Shift,
    XKB_KEY_Shift_Lock,              Qt::Key_Shift,
    XKB_KEY_Control_L,               Qt::Key_Control,
    XKB_KEY_Control_R,               Qt::Key_Control,
    XKB_KEY_Meta_L,                  Qt::Key_Meta,
    XKB_KEY_Meta_R,                  Qt::Key_Meta,
    XKB_KEY_Alt_L,                   Qt::Key_Alt,
    XKB_KEY_Alt_R,                   Qt::Key_Alt,
    XKB_KEY_Caps_Lock,               Qt::Key_CapsLock,
    XKB_KEY_Num_Lock,                Qt::Key_NumLock,
    XKB_KEY_Scroll_Lock,             Qt::Key_ScrollLock,
    XKB_KEY_Super_L,                 Qt::Key_Super_L,
    XKB_KEY_Super_R,                 Qt::Key_Super_R,
    XKB_KEY_Menu,                    Qt::Key_Menu,
    XKB_KEY_Hyper_L,                 Qt::Key_Hyper_L,
    XKB_KEY_Hyper_R,                 Qt::Key_Hyper_R,
    XKB_KEY_Help,                    Qt::Key_Help,
    0x1000FF74,                 Qt::Key_Backtab,        // hardcoded HP backtab
    0x1005FF10,                 Qt::Key_F11,            // hardcoded Sun F36 (labeled F11)
    0x1005FF11,                 Qt::Key_F12,            // hardcoded Sun F37 (labeled F12)

    // numeric and function keypad keys

    XKB_KEY_KP_Space,                Qt::Key_Space,
    XKB_KEY_KP_Tab,                  Qt::Key_Tab,
    XKB_KEY_KP_Enter,                Qt::Key_Enter,
    //XKB_KEY_KP_F1,                 Qt::Key_F1,
    //XKB_KEY_KP_F2,                 Qt::Key_F2,
    //XKB_KEY_KP_F3,                 Qt::Key_F3,
    //XKB_KEY_KP_F4,                 Qt::Key_F4,
    XKB_KEY_KP_Home,                 Qt::Key_Home,
    XKB_KEY_KP_Left,                 Qt::Key_Left,
    XKB_KEY_KP_Up,                   Qt::Key_Up,
    XKB_KEY_KP_Right,                Qt::Key_Right,
    XKB_KEY_KP_Down,                 Qt::Key_Down,
    XKB_KEY_KP_Prior,                Qt::Key_PageUp,
    XKB_KEY_KP_Next,                 Qt::Key_PageDown,
    XKB_KEY_KP_End,                  Qt::Key_End,
    XKB_KEY_KP_Begin,                Qt::Key_Clear,
    XKB_KEY_KP_Insert,               Qt::Key_Insert,
    XKB_KEY_KP_Delete,               Qt::Key_Delete,
    XKB_KEY_KP_Equal,                Qt::Key_Equal,
    XKB_KEY_KP_Multiply,             Qt::Key_Asterisk,
    XKB_KEY_KP_Add,                  Qt::Key_Plus,
    XKB_KEY_KP_Separator,            Qt::Key_Comma,
    XKB_KEY_KP_Subtract,             Qt::Key_Minus,
    XKB_KEY_KP_Decimal,              Qt::Key_Period,
    XKB_KEY_KP_Divide,               Qt::Key_Slash,

    // special non-XF86 function keys

    XKB_KEY_Undo,                    Qt::Key_Undo,
    XKB_KEY_Redo,                    Qt::Key_Redo,
    XKB_KEY_Find,                    Qt::Key_Find,
    XKB_KEY_Cancel,                  Qt::Key_Cancel,

    // International input method support keys

    // International & multi-key character composition
    XKB_KEY_ISO_Level3_Shift,        Qt::Key_AltGr,
    XKB_KEY_Multi_key,               Qt::Key_Multi_key,
    XKB_KEY_Codeinput,               Qt::Key_Codeinput,
    XKB_KEY_SingleCandidate,         Qt::Key_SingleCandidate,
    XKB_KEY_MultipleCandidate,       Qt::Key_MultipleCandidate,
    XKB_KEY_PreviousCandidate,       Qt::Key_PreviousCandidate,

    // Misc Functions
    XKB_KEY_Mode_switch,             Qt::Key_Mode_switch,
    XKB_KEY_script_switch,           Qt::Key_Mode_switch,

    // Japanese keyboard support
    XKB_KEY_Kanji,                   Qt::Key_Kanji,
    XKB_KEY_Muhenkan,                Qt::Key_Muhenkan,
    //XKB_KEY_Henkan_Mode,           Qt::Key_Henkan_Mode,
    XKB_KEY_Henkan_Mode,             Qt::Key_Henkan,
    XKB_KEY_Henkan,                  Qt::Key_Henkan,
    XKB_KEY_Romaji,                  Qt::Key_Romaji,
    XKB_KEY_Hiragana,                Qt::Key_Hiragana,
    XKB_KEY_Katakana,                Qt::Key_Katakana,
    XKB_KEY_Hiragana_Katakana,       Qt::Key_Hiragana_Katakana,
    XKB_KEY_Zenkaku,                 Qt::Key_Zenkaku,
    XKB_KEY_Hankaku,                 Qt::Key_Hankaku,
    XKB_KEY_Zenkaku_Hankaku,         Qt::Key_Zenkaku_Hankaku,
    XKB_KEY_Touroku,                 Qt::Key_Touroku,
    XKB_KEY_Massyo,                  Qt::Key_Massyo,
    XKB_KEY_Kana_Lock,               Qt::Key_Kana_Lock,
    XKB_KEY_Kana_Shift,              Qt::Key_Kana_Shift,
    XKB_KEY_Eisu_Shift,              Qt::Key_Eisu_Shift,
    XKB_KEY_Eisu_toggle,             Qt::Key_Eisu_toggle,
    //XKB_KEY_Kanji_Bangou,          Qt::Key_Kanji_Bangou,
    //XKB_KEY_Zen_Koho,              Qt::Key_Zen_Koho,
    //XKB_KEY_Mae_Koho,              Qt::Key_Mae_Koho,
    XKB_KEY_Kanji_Bangou,            Qt::Key_Codeinput,
    XKB_KEY_Zen_Koho,                Qt::Key_MultipleCandidate,
    XKB_KEY_Mae_Koho,                Qt::Key_PreviousCandidate,

    // Korean keyboard support
    XKB_KEY_Hangul,                  Qt::Key_Hangul,
    XKB_KEY_Hangul_Start,            Qt::Key_Hangul_Start,
    XKB_KEY_Hangul_End,              Qt::Key_Hangul_End,
    XKB_KEY_Hangul_Hanja,            Qt::Key_Hangul_Hanja,
    XKB_KEY_Hangul_Jamo,             Qt::Key_Hangul_Jamo,
    XKB_KEY_Hangul_Romaja,           Qt::Key_Hangul_Romaja,
    //XKB_KEY_Hangul_Codeinput,      Qt::Key_Hangul_Codeinput,
    XKB_KEY_Hangul_Codeinput,        Qt::Key_Codeinput,
    XKB_KEY_Hangul_Jeonja,           Qt::Key_Hangul_Jeonja,
    XKB_KEY_Hangul_Banja,            Qt::Key_Hangul_Banja,
    XKB_KEY_Hangul_PreHanja,         Qt::Key_Hangul_PreHanja,
    XKB_KEY_Hangul_PostHanja,        Qt::Key_Hangul_PostHanja,
    //XKB_KEY_Hangul_SingleCandidate,Qt::Key_Hangul_SingleCandidate,
    //XKB_KEY_Hangul_MultipleCandidate,Qt::Key_Hangul_MultipleCandidate,
    //XKB_KEY_Hangul_PreviousCandidate,Qt::Key_Hangul_PreviousCandidate,
    XKB_KEY_Hangul_SingleCandidate,  Qt::Key_SingleCandidate,
    XKB_KEY_Hangul_MultipleCandidate,Qt::Key_MultipleCandidate,
    XKB_KEY_Hangul_PreviousCandidate,Qt::Key_PreviousCandidate,
    XKB_KEY_Hangul_Special,          Qt::Key_Hangul_Special,
    //XKB_KEY_Hangul_switch,         Qt::Key_Hangul_switch,
    XKB_KEY_Hangul_switch,           Qt::Key_Mode_switch,

    // dead keys
    XKB_KEY_dead_grave,              Qt::Key_Dead_Grave,
    XKB_KEY_dead_acute,              Qt::Key_Dead_Acute,
    XKB_KEY_dead_circumflex,         Qt::Key_Dead_Circumflex,
    XKB_KEY_dead_tilde,              Qt::Key_Dead_Tilde,
    XKB_KEY_dead_macron,             Qt::Key_Dead_Macron,
    XKB_KEY_dead_breve,              Qt::Key_Dead_Breve,
    XKB_KEY_dead_abovedot,           Qt::Key_Dead_Abovedot,
    XKB_KEY_dead_diaeresis,          Qt::Key_Dead_Diaeresis,
    XKB_KEY_dead_abovering,          Qt::Key_Dead_Abovering,
    XKB_KEY_dead_doubleacute,        Qt::Key_Dead_Doubleacute,
    XKB_KEY_dead_caron,              Qt::Key_Dead_Caron,
    XKB_KEY_dead_cedilla,            Qt::Key_Dead_Cedilla,
    XKB_KEY_dead_ogonek,             Qt::Key_Dead_Ogonek,
    XKB_KEY_dead_iota,               Qt::Key_Dead_Iota,
    XKB_KEY_dead_voiced_sound,       Qt::Key_Dead_Voiced_Sound,
    XKB_KEY_dead_semivoiced_sound,   Qt::Key_Dead_Semivoiced_Sound,
    XKB_KEY_dead_belowdot,           Qt::Key_Dead_Belowdot,
    XKB_KEY_dead_hook,               Qt::Key_Dead_Hook,
    XKB_KEY_dead_horn,               Qt::Key_Dead_Horn,
    XKB_KEY_dead_stroke,             Qt::Key_Dead_Stroke,
    XKB_KEY_dead_abovecomma,         Qt::Key_Dead_Abovecomma,
    XKB_KEY_dead_abovereversedcomma, Qt::Key_Dead_Abovereversedcomma,
    XKB_KEY_dead_doublegrave,        Qt::Key_Dead_Doublegrave,
    XKB_KEY_dead_belowring,          Qt::Key_Dead_Belowring,
    XKB_KEY_dead_belowmacron,        Qt::Key_Dead_Belowmacron,
    XKB_KEY_dead_belowcircumflex,    Qt::Key_Dead_Belowcircumflex,
    XKB_KEY_dead_belowtilde,         Qt::Key_Dead_Belowtilde,
    XKB_KEY_dead_belowbreve,         Qt::Key_Dead_Belowbreve,
    XKB_KEY_dead_belowdiaeresis,     Qt::Key_Dead_Belowdiaeresis,
    XKB_KEY_dead_invertedbreve,      Qt::Key_Dead_Invertedbreve,
    XKB_KEY_dead_belowcomma,         Qt::Key_Dead_Belowcomma,
    XKB_KEY_dead_currency,           Qt::Key_Dead_Currency,
    XKB_KEY_dead_a,                  Qt::Key_Dead_a,
    XKB_KEY_dead_A,                  Qt::Key_Dead_A,
    XKB_KEY_dead_e,                  Qt::Key_Dead_e,
    XKB_KEY_dead_E,                  Qt::Key_Dead_E,
    XKB_KEY_dead_i,                  Qt::Key_Dead_i,
    XKB_KEY_dead_I,                  Qt::Key_Dead_I,
    XKB_KEY_dead_o,                  Qt::Key_Dead_o,
    XKB_KEY_dead_O,                  Qt::Key_Dead_O,
    XKB_KEY_dead_u,                  Qt::Key_Dead_u,
    XKB_KEY_dead_U,                  Qt::Key_Dead_U,
    XKB_KEY_dead_small_schwa,        Qt::Key_Dead_Small_Schwa,
    XKB_KEY_dead_capital_schwa,      Qt::Key_Dead_Capital_Schwa,
    XKB_KEY_dead_greek,              Qt::Key_Dead_Greek,
    XKB_KEY_dead_lowline,            Qt::Key_Dead_Lowline,
    XKB_KEY_dead_aboveverticalline,  Qt::Key_Dead_Aboveverticalline,
    XKB_KEY_dead_belowverticalline,  Qt::Key_Dead_Belowverticalline,
    XKB_KEY_dead_longsolidusoverlay, Qt::Key_Dead_Longsolidusoverlay,

    // Special keys from X.org - This include multimedia keys,
    // wireless/bluetooth/uwb keys, special launcher keys, etc.
    XKB_KEY_XF86Back,                Qt::Key_Back,
    XKB_KEY_XF86Forward,             Qt::Key_Forward,
    XKB_KEY_XF86Stop,                Qt::Key_Stop,
    XKB_KEY_XF86Refresh,             Qt::Key_Refresh,
    XKB_KEY_XF86Favorites,           Qt::Key_Favorites,
    XKB_KEY_XF86AudioMedia,          Qt::Key_LaunchMedia,
    XKB_KEY_XF86OpenURL,             Qt::Key_OpenUrl,
    XKB_KEY_XF86HomePage,            Qt::Key_HomePage,
    XKB_KEY_XF86Search,              Qt::Key_Search,
    XKB_KEY_XF86AudioLowerVolume,    Qt::Key_VolumeDown,
    XKB_KEY_XF86AudioMute,           Qt::Key_VolumeMute,
    XKB_KEY_XF86AudioRaiseVolume,    Qt::Key_VolumeUp,
    XKB_KEY_XF86AudioPlay,           Qt::Key_MediaPlay,
    XKB_KEY_XF86AudioStop,           Qt::Key_MediaStop,
    XKB_KEY_XF86AudioPrev,           Qt::Key_MediaPrevious,
    XKB_KEY_XF86AudioNext,           Qt::Key_MediaNext,
    XKB_KEY_XF86AudioRecord,         Qt::Key_MediaRecord,
    XKB_KEY_XF86AudioPause,          Qt::Key_MediaPause,
    XKB_KEY_XF86Mail,                Qt::Key_LaunchMail,
    XKB_KEY_XF86MyComputer,          Qt::Key_Launch0,  // ### Qt 6: remap properly
    XKB_KEY_XF86Calculator,          Qt::Key_Launch1,
    XKB_KEY_XF86Memo,                Qt::Key_Memo,
    XKB_KEY_XF86ToDoList,            Qt::Key_ToDoList,
    XKB_KEY_XF86Calendar,            Qt::Key_Calendar,
    XKB_KEY_XF86PowerDown,           Qt::Key_PowerDown,
    XKB_KEY_XF86ContrastAdjust,      Qt::Key_ContrastAdjust,
    XKB_KEY_XF86Standby,             Qt::Key_Standby,
    XKB_KEY_XF86MonBrightnessUp,     Qt::Key_MonBrightnessUp,
    XKB_KEY_XF86MonBrightnessDown,   Qt::Key_MonBrightnessDown,
    XKB_KEY_XF86KbdLightOnOff,       Qt::Key_KeyboardLightOnOff,
    XKB_KEY_XF86KbdBrightnessUp,     Qt::Key_KeyboardBrightnessUp,
    XKB_KEY_XF86KbdBrightnessDown,   Qt::Key_KeyboardBrightnessDown,
    XKB_KEY_XF86PowerOff,            Qt::Key_PowerOff,
    XKB_KEY_XF86WakeUp,              Qt::Key_WakeUp,
    XKB_KEY_XF86Eject,               Qt::Key_Eject,
    XKB_KEY_XF86ScreenSaver,         Qt::Key_ScreenSaver,
    XKB_KEY_XF86WWW,                 Qt::Key_WWW,
    XKB_KEY_XF86Sleep,               Qt::Key_Sleep,
    XKB_KEY_XF86LightBulb,           Qt::Key_LightBulb,
    XKB_KEY_XF86Shop,                Qt::Key_Shop,
    XKB_KEY_XF86History,             Qt::Key_History,
    XKB_KEY_XF86AddFavorite,         Qt::Key_AddFavorite,
    XKB_KEY_XF86HotLinks,            Qt::Key_HotLinks,
    XKB_KEY_XF86BrightnessAdjust,    Qt::Key_BrightnessAdjust,
    XKB_KEY_XF86Finance,             Qt::Key_Finance,
    XKB_KEY_XF86Community,           Qt::Key_Community,
    XKB_KEY_XF86AudioRewind,         Qt::Key_AudioRewind,
    XKB_KEY_XF86BackForward,         Qt::Key_BackForward,
    XKB_KEY_XF86ApplicationLeft,     Qt::Key_ApplicationLeft,
    XKB_KEY_XF86ApplicationRight,    Qt::Key_ApplicationRight,
    XKB_KEY_XF86Book,                Qt::Key_Book,
    XKB_KEY_XF86CD,                  Qt::Key_CD,
    XKB_KEY_XF86Calculater,          Qt::Key_Calculator,
    XKB_KEY_XF86Clear,               Qt::Key_Clear,
    XKB_KEY_XF86ClearGrab,           Qt::Key_ClearGrab,
    XKB_KEY_XF86Close,               Qt::Key_Close,
    XKB_KEY_XF86Copy,                Qt::Key_Copy,
    XKB_KEY_XF86Cut,                 Qt::Key_Cut,
    XKB_KEY_XF86Display,             Qt::Key_Display,
    XKB_KEY_XF86DOS,                 Qt::Key_DOS,
    XKB_KEY_XF86Documents,           Qt::Key_Documents,
    XKB_KEY_XF86Excel,               Qt::Key_Excel,
    XKB_KEY_XF86Explorer,            Qt::Key_Explorer,
    XKB_KEY_XF86Game,                Qt::Key_Game,
    XKB_KEY_XF86Go,                  Qt::Key_Go,
    XKB_KEY_XF86iTouch,              Qt::Key_iTouch,
    XKB_KEY_XF86LogOff,              Qt::Key_LogOff,
    XKB_KEY_XF86Market,              Qt::Key_Market,
    XKB_KEY_XF86Meeting,             Qt::Key_Meeting,
    XKB_KEY_XF86MenuKB,              Qt::Key_MenuKB,
    XKB_KEY_XF86MenuPB,              Qt::Key_MenuPB,
    XKB_KEY_XF86MySites,             Qt::Key_MySites,
    XKB_KEY_XF86New,                 Qt::Key_New,
    XKB_KEY_XF86News,                Qt::Key_News,
    XKB_KEY_XF86OfficeHome,          Qt::Key_OfficeHome,
    XKB_KEY_XF86Open,                Qt::Key_Open,
    XKB_KEY_XF86Option,              Qt::Key_Option,
    XKB_KEY_XF86Paste,               Qt::Key_Paste,
    XKB_KEY_XF86Phone,               Qt::Key_Phone,
    XKB_KEY_XF86Reply,               Qt::Key_Reply,
    XKB_KEY_XF86Reload,              Qt::Key_Reload,
    XKB_KEY_XF86RotateWindows,       Qt::Key_RotateWindows,
    XKB_KEY_XF86RotationPB,          Qt::Key_RotationPB,
    XKB_KEY_XF86RotationKB,          Qt::Key_RotationKB,
    XKB_KEY_XF86Save,                Qt::Key_Save,
    XKB_KEY_XF86Send,                Qt::Key_Send,
    XKB_KEY_XF86Spell,               Qt::Key_Spell,
    XKB_KEY_XF86SplitScreen,         Qt::Key_SplitScreen,
    XKB_KEY_XF86Support,             Qt::Key_Support,
    XKB_KEY_XF86TaskPane,            Qt::Key_TaskPane,
    XKB_KEY_XF86Terminal,            Qt::Key_Terminal,
    XKB_KEY_XF86Tools,               Qt::Key_Tools,
    XKB_KEY_XF86Travel,              Qt::Key_Travel,
    XKB_KEY_XF86Video,               Qt::Key_Video,
    XKB_KEY_XF86Word,                Qt::Key_Word,
    XKB_KEY_XF86Xfer,                Qt::Key_Xfer,
    XKB_KEY_XF86ZoomIn,              Qt::Key_ZoomIn,
    XKB_KEY_XF86ZoomOut,             Qt::Key_ZoomOut,
    XKB_KEY_XF86Away,                Qt::Key_Away,
    XKB_KEY_XF86Messenger,           Qt::Key_Messenger,
    XKB_KEY_XF86WebCam,              Qt::Key_WebCam,
    XKB_KEY_XF86MailForward,         Qt::Key_MailForward,
    XKB_KEY_XF86Pictures,            Qt::Key_Pictures,
    XKB_KEY_XF86Music,               Qt::Key_Music,
    XKB_KEY_XF86Battery,             Qt::Key_Battery,
    XKB_KEY_XF86Bluetooth,           Qt::Key_Bluetooth,
    XKB_KEY_XF86WLAN,                Qt::Key_WLAN,
    XKB_KEY_XF86UWB,                 Qt::Key_UWB,
    XKB_KEY_XF86AudioForward,        Qt::Key_AudioForward,
    XKB_KEY_XF86AudioRepeat,         Qt::Key_AudioRepeat,
    XKB_KEY_XF86AudioRandomPlay,     Qt::Key_AudioRandomPlay,
    XKB_KEY_XF86Subtitle,            Qt::Key_Subtitle,
    XKB_KEY_XF86AudioCycleTrack,     Qt::Key_AudioCycleTrack,
    XKB_KEY_XF86Time,                Qt::Key_Time,
    XKB_KEY_XF86Select,              Qt::Key_Select,
    XKB_KEY_XF86View,                Qt::Key_View,
    XKB_KEY_XF86TopMenu,             Qt::Key_TopMenu,
    XKB_KEY_XF86Red,                 Qt::Key_Red,
    XKB_KEY_XF86Green,               Qt::Key_Green,
    XKB_KEY_XF86Yellow,              Qt::Key_Yellow,
    XKB_KEY_XF86Blue,                Qt::Key_Blue,
    XKB_KEY_XF86Bluetooth,           Qt::Key_Bluetooth,
    XKB_KEY_XF86Suspend,             Qt::Key_Suspend,
    XKB_KEY_XF86Hibernate,           Qt::Key_Hibernate,
    XKB_KEY_XF86TouchpadToggle,      Qt::Key_TouchpadToggle,
    XKB_KEY_XF86TouchpadOn,          Qt::Key_TouchpadOn,
    XKB_KEY_XF86TouchpadOff,         Qt::Key_TouchpadOff,
    XKB_KEY_XF86AudioMicMute,        Qt::Key_MicMute,
    XKB_KEY_XF86Launch0,             Qt::Key_Launch2, // ### Qt 6: remap properly
    XKB_KEY_XF86Launch1,             Qt::Key_Launch3,
    XKB_KEY_XF86Launch2,             Qt::Key_Launch4,
    XKB_KEY_XF86Launch3,             Qt::Key_Launch5,
    XKB_KEY_XF86Launch4,             Qt::Key_Launch6,
    XKB_KEY_XF86Launch5,             Qt::Key_Launch7,
    XKB_KEY_XF86Launch6,             Qt::Key_Launch8,
    XKB_KEY_XF86Launch7,             Qt::Key_Launch9,
    XKB_KEY_XF86Launch8,             Qt::Key_LaunchA,
    XKB_KEY_XF86Launch9,             Qt::Key_LaunchB,
    XKB_KEY_XF86LaunchA,             Qt::Key_LaunchC,
    XKB_KEY_XF86LaunchB,             Qt::Key_LaunchD,
    XKB_KEY_XF86LaunchC,             Qt::Key_LaunchE,
    XKB_KEY_XF86LaunchD,             Qt::Key_LaunchF,
    XKB_KEY_XF86LaunchE,             Qt::Key_LaunchG,
    XKB_KEY_XF86LaunchF,             Qt::Key_LaunchH,
	0,                          0
};

int keysymToQtKey(uint32_t key)
{
	int code = 0;
	int i = 0;
	while (KeyTbl[i])
	{
		if (key == KeyTbl[i])
		{
			code = (int)KeyTbl[i+1];
			break;
		}
		i += 2;
	}

	return code;
}

bool xkbToQtKey(uint32_t xkbKeySymbol, uint32_t unicodeCharacter, int& qtKey, Qt::KeyboardModifiers& modifiers)
{
	// Commentary in X11/keysymdef says that X codes match ASCII, so it
	// is safe to use the locale functions to process X codes in ISO8859-1.
	// This is mainly for compatibility - applications should not use the
	// Qt keycodes between 128 and 255 (extended ACSII codes), but should
	// rather use the QKeyEvent::text().
	if (xkbKeySymbol < 256)
	{
		// upper-case key, if known
		qtKey = isprint((int)xkbKeySymbol) ? toupper((int)xkbKeySymbol) : 0;
	}
	else if (xkbKeySymbol >= XKB_KEY_F1 && xkbKeySymbol <= XKB_KEY_F35)
	{
		// function keys
		qtKey = Qt::Key_F1 + ((int)xkbKeySymbol - XKB_KEY_F1);
	}
	else if (xkbKeySymbol >= XKB_KEY_KP_Space && xkbKeySymbol <= XKB_KEY_KP_9)
	{
		if (xkbKeySymbol >= XKB_KEY_KP_0)
		{
			// numeric keypad keys
			qtKey = Qt::Key_0 + ((int)xkbKeySymbol - XKB_KEY_KP_0);
		}
		else
		{
			qtKey = keysymToQtKey(xkbKeySymbol);
		}
		modifiers |= Qt::KeypadModifier;
	}
	else if (unicodeCharacter
		&& unicodeCharacter > 0x1f
		&& unicodeCharacter != 0x7f
		&& !(xkbKeySymbol >= XKB_KEY_dead_grave && xkbKeySymbol <= XKB_KEY_dead_currency))
	{
		QString text = QChar(unicodeCharacter);
		qtKey = text.unicode()->toUpper().unicode();
	}
	else
	{
		// any other keys
		qtKey = keysymToQtKey(xkbKeySymbol);
	}

	return qtKey != 0;
}

Qt::KeyboardModifiers xkbToQtModifers(uint32_t xkbModifiers)
{
	/*
	Alt        = Mod1,
	NumLock    = Mod2,
	ModeSwitch = Mod3,
	Meta       = Mod4,
	AltGr      = Mod5,
	*/

	Qt::KeyboardModifiers ret{};
	if (xkbModifiers & XkbModMask::Shift)
		ret |= Qt::ShiftModifier;
	if (xkbModifiers & XkbModMask::Control)
		ret |= Qt::ControlModifier;
	if (xkbModifiers & XkbModMask::Mod1)
		ret |= Qt::AltModifier;
	if (xkbModifiers & XkbModMask::Mod2)
		ret |= Qt::KeypadModifier;
	if (xkbModifiers & XkbModMask::Mod3)
		ret |= Qt::GroupSwitchModifier;
	if (xkbModifiers & XkbModMask::Mod4)
		ret |= Qt::MetaModifier;
	if (xkbModifiers & XkbModMask::Mod5)
		ret |= Qt::GroupSwitchModifier;
	if (xkbModifiers & XkbModMask::Lock)
		ret |= Qt::KeypadModifier;

	return ret;
}
} // namespace tvqtsdk
