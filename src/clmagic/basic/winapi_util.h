#pragma once
#include <windowsx.h>
#include <Windows.h>
#include <assert.h>
#include <map>
#include <deque>
#include <string>
#include <iostream>
#include <functional>

namespace winapi_util {
	
	struct xRECT : public RECT {
		xRECT() = default;
		xRECT(int x, int y, int width, int height)
			: RECT{x, y, x + width, y + height}  {}
	
		int x() const {
			return RECT::left;
		}
		int y() const {
			return RECT::top;
		}
		int width() const {
			return RECT::right - RECT::left;
		}
		int height() const {
			return RECT::bottom - RECT::top;
		}
	
		void move_to(int x2, int y2) {
			int xdist = x2 - this->x();
			int ydist = y2 - this->y();
			this->left = x2;
			this->top  = y2;
			this->right  += xdist;
			this->bottom += ydist;
		}

		xRECT copy_move_to(int x2, int y2) const {
			xRECT _Clone = *this;
			_Clone.move_to(x2, y2);
			return _Clone;
		}

		std::string to_string() const {
			return "RECT{ left: " + std::to_string(this->left)
					+ ", right: " + std::to_string(this->right)
					+ ", top: "   + std::to_string(this->top)
					+ ", bottom: " + std::to_string(this->bottom) + "}";
		}

		friend std::ostream& operator<<(std::ostream& _Ostr, xRECT& r) {
			return (_Ostr << r);
		}
	};

	void register_WNDCLASS(HINSTANCE hInstance, WNDPROC lpfnWndProc, LPCWSTR windowName) {
		WNDCLASS _My_wc;
		_My_wc.style = CS_HREDRAW | CS_VREDRAW;
		_My_wc.lpfnWndProc   = lpfnWndProc;
		_My_wc.cbClsExtra    = 0;
		_My_wc.cbWndExtra    = 0;
		_My_wc.hInstance     = hInstance;
		_My_wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
		_My_wc.hCursor       = LoadCursor(0, IDC_ARROW);
		_My_wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		_My_wc.lpszMenuName  = windowName;
		_My_wc.lpszClassName = windowName;
		assert(RegisterClass(&_My_wc));
	}

	void make_HWND(HWND& _Result, HINSTANCE hInstance, LPCWSTR windowName, xRECT rect, int dwStyle) {
		AdjustWindowRect(&rect, dwStyle, false);
		_Result = CreateWindow(windowName, windowName, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rect.width(), rect.height(),
			nullptr, nullptr, hInstance, nullptr);
		assert(_Result != nullptr);
	}
	

	/*- - - - - - - - - - - - - - - - - - - - - - - - - window - - - - - - - - - - - - - - - - - - - - - - - -*/
	enum button_states {
		press,
		repeat,
		release,
	};

	enum mouse_types {
		left,
		middle,
		right
	};


	class window {
		template<typename _Fn, typename ..._Tys>
		static void send(_Fn _Memfn, HWND hwnd, _Tys... _Args) {
			window* _Pwin = window::find(hwnd);
			if (_Pwin != nullptr) {
				_Memfn(_Pwin, _Args...);
			}
		}
		static void _Push_massage_to_window_from_HWND(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
			window* _Pwin = winapi_util::window::find(hwnd);
			if (_Pwin != nullptr) {
				if (msg == WM_KEYDOWN) {
					_Pwin->_My_FSM.push_back(WM_KEYDOWN + wParam);
				} else if (msg == WM_KEYUP) {
					_Pwin->_My_FSM.push_back(WM_KEYUP + wParam);
				} else {
					_Pwin->_My_FSM.push_back(msg);
				}
				_Pwin->_My_FSM.pop_front();
			}
		}

		static LRESULT CALLBACK MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
			_Push_massage_to_window_from_HWND(hwnd, msg, wParam, lParam);
			
			switch (msg) {
				case WM_ACTIVATE:
					/*if (LOWORD(wParam) == WA_INACTIVE) {
						MessageBoxA(nullptr, "WM_ACTIVATE-WA_INACTIVE", "MSG", MB_OK);
					} else {
						MessageBoxA(nullptr, "WM_ACTIVATE-NO", "MSG", MB_OK);
					}*/
					break;
				case WM_DESTROY:
					{
						auto       _First = window::_All_opened_event_windows().begin();
						const auto _Last  = window::_All_opened_event_windows().end();
						for (; _First != _Last; ++_First) {
							_First->second->on_close();
						}
					}
					PostQuitMessage(0);
					break;
				case WM_SETFOCUS:
					send(std::mem_fn(&window::on_focus), hwnd);
					break;
				case WM_KILLFOCUS:
					send(std::mem_fn(&window::on_lose_focus), hwnd);
					break;
				case WM_SIZE: 
					{
						window* _Pwin = window::find(hwnd);
						if (_Pwin != nullptr) {
							xRECT  _Current = _Pwin->get_RECT();
							xRECT& _Prev    = _Pwin->_My_old_rect;
							_Pwin->on_resize(_Current, _Prev);
							_Prev = _Current;
						}
					}
					break;
				case WM_ENTERSIZEMOVE:
					//MessageBoxA(nullptr, "WM_ENTERSIZEMOVE", "MSG", MB_OK);
					break;
				case WM_EXITSIZEMOVE:
					//MessageBoxA(nullptr, "WM_EXITSIZEMOVE", "MSG", MB_OK);
					break;
				case WM_CLOSE:
					send(std::mem_fn(&window::on_close), hwnd);
					break;
				case WM_MENUCHAR:
					MessageBoxA(nullptr, "WM_MENUCHAR", "MSG", MB_OK);
					MAKELRESULT(0, MNC_CLOSE);
					break;
				case WM_LBUTTONDOWN:
					send(std::mem_fn(&window::on_mouse), hwnd, mouse_types::left, button_states::press);
					break;
				case WM_RBUTTONDOWN:
					send(std::mem_fn(&window::on_mouse), hwnd, mouse_types::right, button_states::press);
					break;
				case WM_MBUTTONDOWN:
					send(std::mem_fn(&window::on_mouse), hwnd, mouse_types::middle, button_states::press);
					break;
				case WM_LBUTTONUP:
					send(std::mem_fn(&window::on_mouse), hwnd, mouse_types::left, button_states::release);
					break;
				case WM_RBUTTONUP:
					send(std::mem_fn(&window::on_mouse), hwnd, mouse_types::right, button_states::release);
					break;
				case WM_MBUTTONUP:
					send(std::mem_fn(&window::on_mouse), hwnd, mouse_types::middle, button_states::release);
					break;
				case WM_MOUSEMOVE:
					{
						window* _Pwin = window::find(hwnd);
						if (_Pwin != nullptr) {
							POINT  _Current = POINT{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
							POINT& _Prev    = _Pwin->_My_old_cursor_pos;
							_Pwin->on_cursor(_Current, _Prev);
							_Prev = _Current;
						}
					}
					break;
				case WM_KEYDOWN:
					send(std::mem_fn(&window::on_key), hwnd, wParam, button_states::press);
					break;
				case WM_KEYUP:
					send(std::mem_fn(&window::on_key), hwnd, wParam, button_states::release);
					break;
			}
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		
		static std::map<HWND, window*>& _All_opened_event_windows() {
 			static std::map<HWND, window*> _static_windows;
			return _static_windows;
		}
		static void _Open_event(window& _Win) {
			_All_opened_event_windows()[_Win._My_win] = &_Win;
		}
		static void _Close_evene(window& _Win) {
			if (_Win._My_win != nullptr) {
				_All_opened_event_windows().erase(_Win._My_win);
			}
		}

	public:
		static window* find(HWND _Key) {
			auto _Where = _All_opened_event_windows().find(_Key);
			return (_Where == _All_opened_event_windows().end()) ? nullptr : _Where->second;
		}
	
		static void run() {
			MSG msg = { 0 };
			while (msg.message != WM_QUIT) {
				if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				} else {
					auto       _First = _All_opened_event_windows().begin();
					const auto _Last  = _All_opened_event_windows().end();
					for ( ; _First != _Last; ++_First) {
						_First->second->on_frame();
					}
				}
			}
		}
		
	protected:
		void _Finite_State_Machine_init() {
			_My_FSM = std::deque<uint32_t>(30, 0);
		}

		void _Window_init(HINSTANCE hInstance, LPCWSTR windowName, int x, int y, int width, int height, int dwStyle) {
			xRECT _Client_rect = xRECT(x, y, width, height);
			register_WNDCLASS(hInstance, winapi_util::window::MsgProc, windowName);
			make_HWND(std::ref(_My_win), hInstance, windowName, _Client_rect, dwStyle);
		   _Open_event(*this);
		   _Finite_State_Machine_init();
		}

	public:
		window() = default;
		window(const window&) = delete;
		window(window&& _Right) {
			std::swap(_My_win, _Right._My_win);
			std::swap(_My_old_rect, _Right._My_old_rect);
			std::swap(_My_old_cursor_pos, _Right._My_old_cursor_pos);
			window::_Open_event(*this);
		}
		window& operator=(const window&) = delete;
		window& operator=(window&& _Right) {
			std::swap(_My_win, _Right._My_win);
			std::swap(_My_old_rect, _Right._My_old_rect);
			std::swap(_My_old_cursor_pos, _Right._My_old_cursor_pos);
			window::_Open_event(*this);
			return *this;
		}
		window(HINSTANCE hInstance, LPCWSTR windowName, int x, int y, int width, int height, int dwStyle = WS_OVERLAPPEDWINDOW) : _My_win(nullptr) {
			// Now, can be error when invoke this->init(...), invoke virtual function before parent construct
			_Window_init(hInstance, windowName, x, y, width, height, dwStyle);
		}
		~window() { this->release(); }
		virtual void release() {
			if ( this->valid() ) {
				_Close_evene(*this);
				DestroyWindow(_My_win);
				_My_win = nullptr;
			}
		}

		HWND  get_HWND() const {
			assert(_My_win != nullptr);
			return _My_win;
		}
		xRECT get_RECT() const {
			assert(_My_win != nullptr);
			xRECT _Result;
			GetClientRect(_My_win, &_Result);
			return _Result;
		}
		void init(HINSTANCE hInstance, LPCWSTR windowName, int x, int y, int width, int height, int dwStyle = WS_OVERLAPPEDWINDOW) {
			release();
		   _Window_init(hInstance, windowName, x, y, width, height, dwStyle);
			on_init();
		}
		bool valid() const {
			return _My_win != nullptr;
		}
		void flash() {
			FlashWindow(_My_win, true);
		} 
		void move(int x, int y, UINT flag = SWP_SHOWWINDOW) {
			SetWindowPos(_My_win, HWND_TOPMOST, x, y, get_RECT().width(), get_RECT().height(), SWP_SHOWWINDOW);
		}
		void move(int x, int y, int width, int height) {
			MoveWindow(_My_win, x, y, width, height, true);
		}
		void focus() const {
			SetFocus(_My_win);
		}
		void show() const {
			assert(_My_win != nullptr);
			ShowWindow(_My_win, SW_SHOW);
		}
		void update() {
			assert(_My_win != nullptr);
			UpdateWindow(_My_win);
		}
		void close() {
			CloseWindow(_My_win);
		}

		// window-message-event
		virtual void on_resize(xRECT _Current, xRECT _Prev) {}
		virtual void on_cursor(POINT _Current, POINT _Prev) {}
		virtual void on_mouse(mouse_types, button_states) {}
		virtual void on_key(int key, button_states) {}
		virtual void on_focus() {}
		virtual void on_lose_focus() {}
		virtual void on_close() {}

		// other-event
		virtual void on_init() {}
		virtual void on_frame() {}// compute-graphics-function

		HWND _My_win;
		std::deque<uint32_t> _My_FSM;
	private:
		xRECT _My_old_rect;// only varing in WM_SIZE event
		POINT _My_old_cursor_pos;
	};

}// winapi_util
