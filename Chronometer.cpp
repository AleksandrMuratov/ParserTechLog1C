#include "Chronometer.h"

namespace chronometer {

	Chronometer::Chronometer(): start(std::chrono::steady_clock::now()){}

	Chronometer::~Chronometer() {
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> duration = end - start;
		std::wcout << L"Execution time: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << L" milliseconds\n";
	}

}// chronometer