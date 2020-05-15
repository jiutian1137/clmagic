#pragma once
namespace d3d12 {
	struct uncopyable {
		uncopyable() = default;
		uncopyable(const uncopyable&) = delete;
		uncopyable& operator=(const uncopyable&) = delete;
	};
}// namespace dx12