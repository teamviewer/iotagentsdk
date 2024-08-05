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
// Modifier masks are adopted from Xorg protocol - X11/X.h: https://gitlab.freedesktop.org/xorg/proto/xorgproto/-/blob/eb28f0378fadd0d143aad7ec16f7b91814faae9a/include/X11/X.h
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

// The following map and implementation below are adopted from https://code.qt.io/cgit/qt/qtbase.git/tree/src/plugins/platforms/xcb/qxcbkeyboard.cpp?h=v5.11.1

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
