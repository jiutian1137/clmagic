#include "fstream.h"

namespace clmagic {
	std::shared_ptr<std::fstream> _Fopen(int _Stack, const char* _Filename, int _Mode) {
		if (_Stack > 10) {// stack fulled
			logcout << std_mes("warn", "file system is error!!!!!!", "_Fopen(int, const char*, int)", __FILE__, __LINE__);
			return (nullptr);
		}

		if ( !fexist(_Filename) ) {
			return (_Fopen_uncheck(_Filename, _Mode));
		} else {// filename exist
			if ( _Mode & std::ios::app ) {
				return (_Fopen_uncheck(_Filename, _Mode));
			} else {
				if ( (_Mode & std::ios::in) && (_Mode & std::ios::out) ) {
					logcout << std_mes("warn", "argument is error: pos: 3", "_Fopen(int, const char*, int)", __FILE__, __LINE__);
					return (nullptr);
				}
				auto _Copyname = std::string(_Filename) + "-Copy";
				return (_Fopen(_Stack + 1, _Copyname.c_str(), _Mode));
			}
		}
	}
}// namespace clmagic