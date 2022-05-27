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

#include <QtCore/QString>

#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <X11/X.h>

#include <cstdio>

// The implementation is adapted from http://code.qt.io/cgit/qt/qtbase.git/tree/src/plugins/platforms/xcb/qxcbkeyboard.cpp
// The applicable version hash is: c5dc7b21ad846ec9d439367f04537b41d35af81d

namespace tvqtsdk
{
static const unsigned int KeyTbl[] = {
	// misc keys

	XK_Escape,                  Qt::Key_Escape,
	XK_Tab,                     Qt::Key_Tab,
	XK_ISO_Left_Tab,            Qt::Key_Backtab,
	XK_BackSpace,               Qt::Key_Backspace,
	XK_Return,                  Qt::Key_Return,
	XK_Insert,                  Qt::Key_Insert,
	XK_Delete,                  Qt::Key_Delete,
	XK_Clear,                   Qt::Key_Delete,
	XK_Pause,                   Qt::Key_Pause,
	XK_Print,                   Qt::Key_Print,
	0x1005FF60,                 Qt::Key_SysReq,         // hardcoded Sun SysReq
	0x1007ff00,                 Qt::Key_SysReq,         // hardcoded X386 SysReq

	// cursor movement

	XK_Home,                    Qt::Key_Home,
	XK_End,                     Qt::Key_End,
	XK_Left,                    Qt::Key_Left,
	XK_Up,                      Qt::Key_Up,
	XK_Right,                   Qt::Key_Right,
	XK_Down,                    Qt::Key_Down,
	XK_Prior,                   Qt::Key_PageUp,
	XK_Next,                    Qt::Key_PageDown,

	// modifiers

	XK_Shift_L,                 Qt::Key_Shift,
	XK_Shift_R,                 Qt::Key_Shift,
	XK_Shift_Lock,              Qt::Key_Shift,
	XK_Control_L,               Qt::Key_Control,
	XK_Control_R,               Qt::Key_Control,
	XK_Meta_L,                  Qt::Key_Meta,
	XK_Meta_R,                  Qt::Key_Meta,
	XK_Alt_L,                   Qt::Key_Alt,
	XK_Alt_R,                   Qt::Key_Alt,
	XK_Caps_Lock,               Qt::Key_CapsLock,
	XK_Num_Lock,                Qt::Key_NumLock,
	XK_Scroll_Lock,             Qt::Key_ScrollLock,
	XK_Super_L,                 Qt::Key_Super_L,
	XK_Super_R,                 Qt::Key_Super_R,
	XK_Menu,                    Qt::Key_Menu,
	XK_Hyper_L,                 Qt::Key_Hyper_L,
	XK_Hyper_R,                 Qt::Key_Hyper_R,
	XK_Help,                    Qt::Key_Help,
	0x1000FF74,                 Qt::Key_Backtab,        // hardcoded HP backtab
	0x1005FF10,                 Qt::Key_F11,            // hardcoded Sun F36 (labeled F11)
	0x1005FF11,                 Qt::Key_F12,            // hardcoded Sun F37 (labeled F12)

	// numeric and function keypad keys

	XK_KP_Space,                Qt::Key_Space,
	XK_KP_Tab,                  Qt::Key_Tab,
	XK_KP_Enter,                Qt::Key_Enter,
	//XK_KP_F1,                 Qt::Key_F1,
	//XK_KP_F2,                 Qt::Key_F2,
	//XK_KP_F3,                 Qt::Key_F3,
	//XK_KP_F4,                 Qt::Key_F4,
	XK_KP_Home,                 Qt::Key_Home,
	XK_KP_Left,                 Qt::Key_Left,
	XK_KP_Up,                   Qt::Key_Up,
	XK_KP_Right,                Qt::Key_Right,
	XK_KP_Down,                 Qt::Key_Down,
	XK_KP_Prior,                Qt::Key_PageUp,
	XK_KP_Next,                 Qt::Key_PageDown,
	XK_KP_End,                  Qt::Key_End,
	XK_KP_Begin,                Qt::Key_Clear,
	XK_KP_Insert,               Qt::Key_Insert,
	XK_KP_Delete,               Qt::Key_Delete,
	XK_KP_Equal,                Qt::Key_Equal,
	XK_KP_Multiply,             Qt::Key_Asterisk,
	XK_KP_Add,                  Qt::Key_Plus,
	XK_KP_Separator,            Qt::Key_Comma,
	XK_KP_Subtract,             Qt::Key_Minus,
	XK_KP_Decimal,              Qt::Key_Period,
	XK_KP_Divide,               Qt::Key_Slash,

	// International input method support keys

	// International & multi-key character composition
	XK_ISO_Level3_Shift,        Qt::Key_AltGr,
	XK_Multi_key,               Qt::Key_Multi_key,
	XK_Codeinput,               Qt::Key_Codeinput,
	XK_SingleCandidate,         Qt::Key_SingleCandidate,
	XK_MultipleCandidate,       Qt::Key_MultipleCandidate,
	XK_PreviousCandidate,       Qt::Key_PreviousCandidate,

	// Misc Functions
	XK_Mode_switch,             Qt::Key_Mode_switch,
	XK_script_switch,           Qt::Key_Mode_switch,

	// Japanese keyboard support
	XK_Kanji,                   Qt::Key_Kanji,
	XK_Muhenkan,                Qt::Key_Muhenkan,
	//XK_Henkan_Mode,           Qt::Key_Henkan_Mode,
	XK_Henkan_Mode,             Qt::Key_Henkan,
	XK_Henkan,                  Qt::Key_Henkan,
	XK_Romaji,                  Qt::Key_Romaji,
	XK_Hiragana,                Qt::Key_Hiragana,
	XK_Katakana,                Qt::Key_Katakana,
	XK_Hiragana_Katakana,       Qt::Key_Hiragana_Katakana,
	XK_Zenkaku,                 Qt::Key_Zenkaku,
	XK_Hankaku,                 Qt::Key_Hankaku,
	XK_Zenkaku_Hankaku,         Qt::Key_Zenkaku_Hankaku,
	XK_Touroku,                 Qt::Key_Touroku,
	XK_Massyo,                  Qt::Key_Massyo,
	XK_Kana_Lock,               Qt::Key_Kana_Lock,
	XK_Kana_Shift,              Qt::Key_Kana_Shift,
	XK_Eisu_Shift,              Qt::Key_Eisu_Shift,
	XK_Eisu_toggle,             Qt::Key_Eisu_toggle,
	//XK_Kanji_Bangou,          Qt::Key_Kanji_Bangou,
	//XK_Zen_Koho,              Qt::Key_Zen_Koho,
	//XK_Mae_Koho,              Qt::Key_Mae_Koho,
	XK_Kanji_Bangou,            Qt::Key_Codeinput,
	XK_Zen_Koho,                Qt::Key_MultipleCandidate,
	XK_Mae_Koho,                Qt::Key_PreviousCandidate,

#ifdef XK_KOREAN
	// Korean keyboard support
	XK_Hangul,                  Qt::Key_Hangul,
	XK_Hangul_Start,            Qt::Key_Hangul_Start,
	XK_Hangul_End,              Qt::Key_Hangul_End,
	XK_Hangul_Hanja,            Qt::Key_Hangul_Hanja,
	XK_Hangul_Jamo,             Qt::Key_Hangul_Jamo,
	XK_Hangul_Romaja,           Qt::Key_Hangul_Romaja,
	//XK_Hangul_Codeinput,      Qt::Key_Hangul_Codeinput,
	XK_Hangul_Codeinput,        Qt::Key_Codeinput,
	XK_Hangul_Jeonja,           Qt::Key_Hangul_Jeonja,
	XK_Hangul_Banja,            Qt::Key_Hangul_Banja,
	XK_Hangul_PreHanja,         Qt::Key_Hangul_PreHanja,
	XK_Hangul_PostHanja,        Qt::Key_Hangul_PostHanja,
	//XK_Hangul_SingleCandidate,Qt::Key_Hangul_SingleCandidate,
	//XK_Hangul_MultipleCandidate,Qt::Key_Hangul_MultipleCandidate,
	//XK_Hangul_PreviousCandidate,Qt::Key_Hangul_PreviousCandidate,
	XK_Hangul_SingleCandidate,  Qt::Key_SingleCandidate,
	XK_Hangul_MultipleCandidate,Qt::Key_MultipleCandidate,
	XK_Hangul_PreviousCandidate,Qt::Key_PreviousCandidate,
	XK_Hangul_Special,          Qt::Key_Hangul_Special,
	//XK_Hangul_switch,         Qt::Key_Hangul_switch,
	XK_Hangul_switch,           Qt::Key_Mode_switch,
#endif  // XK_KOREAN

	// dead keys
	XK_dead_grave,              Qt::Key_Dead_Grave,
	XK_dead_acute,              Qt::Key_Dead_Acute,
	XK_dead_circumflex,         Qt::Key_Dead_Circumflex,
	XK_dead_tilde,              Qt::Key_Dead_Tilde,
	XK_dead_macron,             Qt::Key_Dead_Macron,
	XK_dead_breve,              Qt::Key_Dead_Breve,
	XK_dead_abovedot,           Qt::Key_Dead_Abovedot,
	XK_dead_diaeresis,          Qt::Key_Dead_Diaeresis,
	XK_dead_abovering,          Qt::Key_Dead_Abovering,
	XK_dead_doubleacute,        Qt::Key_Dead_Doubleacute,
	XK_dead_caron,              Qt::Key_Dead_Caron,
	XK_dead_cedilla,            Qt::Key_Dead_Cedilla,
	XK_dead_ogonek,             Qt::Key_Dead_Ogonek,
	XK_dead_iota,               Qt::Key_Dead_Iota,
	XK_dead_voiced_sound,       Qt::Key_Dead_Voiced_Sound,
	XK_dead_semivoiced_sound,   Qt::Key_Dead_Semivoiced_Sound,
	XK_dead_belowdot,           Qt::Key_Dead_Belowdot,
	XK_dead_hook,               Qt::Key_Dead_Hook,
	XK_dead_horn,               Qt::Key_Dead_Horn,
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
	else if (xkbKeySymbol >= XK_F1 && xkbKeySymbol <= XK_F35)
	{
		// function keys
		qtKey = Qt::Key_F1 + ((int)xkbKeySymbol - XK_F1);
	}
	else if (xkbKeySymbol >= XK_KP_Space && xkbKeySymbol <= XK_KP_9)
	{
		if (xkbKeySymbol >= XK_KP_0)
		{
			// numeric keypad keys
			qtKey = Qt::Key_0 + ((int)xkbKeySymbol - XK_KP_0);
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
		&& !(xkbKeySymbol >= XK_dead_grave && xkbKeySymbol <= XK_dead_currency))
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
	if (xkbModifiers & ShiftMask)
		ret |= Qt::ShiftModifier;
	if (xkbModifiers & ControlMask)
		ret |= Qt::ControlModifier;
	if (xkbModifiers & Mod1Mask)
		ret |= Qt::AltModifier;
	if (xkbModifiers & Mod2Mask)
		ret |= Qt::KeypadModifier;
	if (xkbModifiers & Mod3Mask)
		ret |= Qt::GroupSwitchModifier;
	if (xkbModifiers & Mod4Mask)
		ret |= Qt::MetaModifier;
	if (xkbModifiers & Mod5Mask)
		ret |= Qt::GroupSwitchModifier;
	if (xkbModifiers & LockMask)
		ret |= Qt::KeypadModifier;

	return ret;
}
} // namespace tvqtsdk
