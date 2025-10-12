#pragma once

namespace PGRenderCore {
	
	enum class KeyState {
		Pressed,   // La tecla ha sido pulsada  
		Released,  // La tecla ha sido soltada  
		Unknown
	};

	
	enum class KeyCode {
		Unknown = 0,
		Space = 32,
		Apostrophe = 39, // '
		Comma = 44, // ,
		Minus = 45, // -
		Period = 46, // .
		Slash = 47, // /
		Num0 = 48, // 0
		Num1 = 49, // 1
		Num2 = 50, // 2
		Num3 = 51, // 3
		Num4 = 52, // 4
		Num5 = 53, // 5
		Num6 = 54, // 6
		Num7 = 55, // 7
		Num8 = 56, // 8
		Num9 = 57, // 9
		Semicolon = 59, // ;
		Equal = 61, // =
		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,
		LBracket = 91, // [
		RBracket = 93, // ]
		Escape = 256,
		Return = 257,
		Tab = 258,
		Delete = 259,
		Backspace,
		Insert, 
		PageUp,
		PageDown,
		Home,
		End,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,


		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,

		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,

		Up = 265,
		Down = 264,
		Left = 263,
		Right = 262
	};

	enum class KeyModifier {
		None = 0,
		Shift = 1 << 0,
		Control = 1 << 1,
		Alt = 1 << 2,
		Super = 1 << 3 // Tecla de Windows o Command en Mac

	};

	inline KeyModifier operator|(KeyModifier a, KeyModifier b) {
		return static_cast<KeyModifier>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}
	inline KeyModifier& operator|=(KeyModifier& a, KeyModifier b) {
		a = a | b;
		return a;
	}
	inline KeyModifier operator&(KeyModifier a, KeyModifier b) {
		return static_cast<KeyModifier>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
	}
	struct KeyEvent {
		KeyCode keyCode; // Código de la tecla presionada  
		KeyState state;   // Estado de la tecla (presionada o liberada)
		KeyModifier modifiers = KeyModifier::None; // Modificadores activos (Shift, Ctrl, Alt, etc.)
	};
}
