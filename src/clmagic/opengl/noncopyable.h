#pragma once
#ifndef glut_NONCOPYABLE_h_
#define glut_NONCOPYABLE_h_

namespace gl {
	struct _Noncopyable {
		_Noncopyable() { }
		_Noncopyable(const _Noncopyable&) = delete;
		_Noncopyable& operator=(const _Noncopyable&) = delete;
	};
}

#endif
