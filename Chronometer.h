#pragma once

#include <chrono>
#include <iostream>

namespace chronometer {

	class Chronometer {
	public:
		Chronometer();
		~Chronometer();
	private:
		std::chrono::time_point<std::chrono::steady_clock> start;
	};

} // chronometer