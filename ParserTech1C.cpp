// ParserTech1C.cpp: определяет точку входа для приложения.
//

#include "ParserTech1C.h"
#include "Parser.h"
#include "Chronometer.h"
#include <fstream>
#include <string>
#include <io.h>
#include <fcntl.h>
#include <Windows.h>
#include <codecvt>
#include <filesystem>
#include <exception>
#include <execution>

using namespace std;

void TestParser() {
	chronometer::Chronometer ct;
	try {
		std::filesystem::path path("D:\\tech_log_parser\\1C_LOG");
		parser_tech_log_1c::Parser parser;
		auto messages = parser.parsing(std::execution::par_unseq, path);
		//auto messages = parser.parsing(path);
		/*for (int i = 0; i < 100; ++i) {
			std::wcout << messages[i] << L'\n';
		}*/
		std::wcout << messages.size() << L" messages\n";
	}
	catch(const std::exception& ex){
		std::cerr << ex.what();
	}
}

void TestParserFile() {
	try {
		std::filesystem::path path("D:\\tech_log_parser\\1C_LOG\\EXCP\\rphost_3912\\25100109.log");
		parser_tech_log_1c::ParserFile parser;
		auto messages = parser.parsing(path);
		std::wcout << messages.size() << L" messages";
	}
	catch (const std::exception& ex) {
		std::cerr << ex.what();
	}
}

int main()
{
	std::ios::sync_with_stdio(false);
	_setmode(_fileno(stdout), _O_U16TEXT);
	_setmode(_fileno(stdin), _O_U16TEXT);
	//_setmode(_fileno(stderr), _O_U16TEXT);
	TestParser();
	return 0;
}
