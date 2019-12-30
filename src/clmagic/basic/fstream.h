#pragma once
#ifndef __CLMAGIC_CORE_BASIC___FILE___H__
#define __CLMAGIC_CORE_BASIC___FILE___H__
#include "logcout.h"
#include <thread>
#include <fstream>

namespace clmagic {

	inline 
	bool fexist(const char* _Filename) {
		auto _Fin = std::ifstream(_Filename);
		bool _Result = _Fin.is_open( );
		if ( _Result ) {
			_Fin.close();
		}
		return (_Result);
	}

	inline 
	std::shared_ptr<std::fstream> _Fopen_uncheck(const char* _Filename, int _Mode = 3) {
		return ( std::shared_ptr<std::fstream>(new std::fstream(_Filename, _Mode)) );
	}

	std::shared_ptr<std::fstream> _Fopen(int _Stack, const char* _Filename, int _Mode = 3);

	inline 
	std::shared_ptr<std::fstream> fopen(const char* _Filename, int _Mode = 3) {
		return (_Fopen(0, _Filename, _Mode));
	}

	class fileproxy {
	public:
		fileproxy() {
			// empty
		}

		fileproxy(const std::string& _Filename)
			: _Myfilename(_Filename) {
			// construct by filename
			std::fstream _Fin;
			_Fin.open(_Filename, std::ios::in);
			if (!_Fin.is_open()) {
				_Fin.open(_Filename, std::ios::out);
			}
			_Fin.close();
		}

		bool empty_source() const {// is _Mysource empty
			return (_Mysource.empty());
		}

		std::string& source() {// get source
			if (this->empty_source()) {
				std::fstream _Fin;
				_Fin.open(_Myfilename, std::ios::in);
				while (!_Fin.is_open()) {
					logcout << std_mes("error", "filename error, is " + _Myfilename, "fileproxy::source()", __FILE__, __LINE__);
					std::this_thread::sleep_for(std::chrono::seconds(3));
					_Fin.open(_Myfilename, std::ios::in);// try open the file again
				}

				_Fin.seekg(0, std::ios::end);
				_Mysource.resize(size_t(_Fin.tellg()));
				_Fin.seekg(std::ios::beg);
				_Fin.read(&_Mysource[0], _Mysource.size());

				_Fin.close();
			}

			return (_Mysource);
		}

		const std::string& source() const {
			return (_Mysource);
		}

		void save() {
			if (!this->empty_source()) {
				std::fstream _Fout;
				_Fout.open(_Myfilename, std::ios::ate | std::ios::out);
				while (!_Fout.is_open()) {
					logcout << std_mes("error", "filename error, is " + _Myfilename, "fileproxy::source()", __FILE__, __LINE__);
					std::this_thread::sleep_for(std::chrono::seconds(3));
					_Fout.open(_Myfilename, std::ios::ate | std::ios::out);// try open the file again
				}

				_Fout.write(&_Mysource[0], _Mysource.size());

				_Fout.close();
			}
			else {
				logcout << std_mes("warn", "fileproxy source empty, is " + _Myfilename, "fileproxy::save()", __FILE__, __LINE__);
			}
		}

		void save(const std::string& _Str) {
			_Mysource = _Str;
			this->save();
		}

		void clear_source() {
			this->save();
			_Mysource.clear();
		}

	private:
		std::string _Myfilename;
		std::string _Mysource;
	};// class fileproxy

}// namespace clmagic





#endif