#include "glsl.h"

namespace clmagic 
{

	std::vector<char> raw_from_file(const char* _Filename) {
		auto _Fin = std::ifstream(_Filename, std::ios::in | std::ios::binary | std::ios::ate);

		if (!_Fin) {
			std::cerr << "ERROR::Reading from file " << _Filename << " failed." << std::endl;
			return (std::vector<char>());
		}

		_Fin.seekg(0, std::ios::end);

		auto _Size = _Fin.tellg();
		auto _Result = std::vector<char>(static_cast<size_t>(_Size));

		_Fin.seekg(0, std::ios::beg);
		_Fin.read(_Result.data(), _Size);

		return (_Result);
	}

	std::vector<float> rawf_from_file(const char* filePath) {
		auto _Fin = std::ifstream(filePath, std::ios::in | std::ios::binary | std::ios::ate);

		if (!_Fin) {
			std::cerr << "EEROR::Reading from file " << filePath << "failed." << std::endl;
			return (std::vector<float>());
		}

		_Fin.seekg(0, std::ios::end);

		auto _Size = _Fin.tellg();
		auto _Result = std::vector<float>(size_t(_Size) / sizeof(float));

		_Fin.seekg(0, std::ios::beg);
		_Fin.read(reinterpret_cast<char*>(_Result.data()), _Size);

		return (_Result);
	}

	std::string text_from_file(const char* filePath) {
		const auto text = raw_from_file(filePath);
		return ( std::string(text.begin(), text.end()) );
	}
}// namespace clmagic 