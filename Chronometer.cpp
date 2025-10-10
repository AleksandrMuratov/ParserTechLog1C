#include "Chronometer.h"

namespace chronometer {

	Chronometer::Chronometer(): start(std::chrono::steady_clock::now()){}

	Chronometer::~Chronometer() {
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> duration = end - start;
		std::wcout << L"Execution time: " << duration.count() << L" seconds\n";
	}

}// chronometer