#include "Parser.h"
#include "ParserString.h"
#include <exception>
#include <algorithm>
#include <execution>

namespace parser_tech_log_1c {

	ParserFile::ParserFile(const std::regex& in_r)
		: r(in_r)
	{
	};

	ParserFile::ParserFile():
	current_ymd(std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())),
	inFile(){
		inFile.imbue(std::locale("ru_RU.UTF-8"));
	};

	std::vector<Message> ParserFile::parsing(const std::filesystem::path& path) {
		std::vector<Message> result;
		inFile.open(path);
		if (!inFile.is_open()) {
			std::string error = "File is not open: " + path.string() + "\n";
			throw std::invalid_argument(error);
		}
		std::wstring filename_with_extension = path.filename().wstring();
		std::wstring filename = filename_with_extension.substr(0, filename_with_extension.size() - 4);
		YearMonthDayHour ymdh = parsing_filename(filename);
		std::wstring bufer;
		bool first = true;
		bool first_message = true; // for a special character at the beginning of the file
		int num_str = 0;
		//std::wstring buf_file;
		//std::getline(inFile, buf_file, wchar_t(EOF)); 
		size_t file_size = std::filesystem::file_size(path);
		std::wstring buf_file(file_size, L' ');
		inFile.read(&buf_file[0], file_size);
		std::wstring_view buf_file_v = parser_string::LRTrip(buf_file);

		std::vector<std::wstring_view> lines(parser_string::SplitView(buf_file_v, L'\n'));
		for (auto& line : lines) {
			++num_str;
			std::wstring_view timestamp;
			if (line.size() > 6) {
				if (first) {
					timestamp = std::wstring_view(line.begin() + 1, line.begin() + 6);
				}
				else {
					timestamp = std::wstring_view(line.begin(), line.begin() + 5);
				}
			}
			first = false;
			if (is_timestamp(timestamp)) {
				if (!bufer.empty()) {
					result.push_back(parsing_message(path, bufer, ymdh, first_message, num_str));
					first_message = false;
				}
				bufer.clear();
				bufer += line;
			}
			else {
				bufer += L" ";
				bufer += line;
			}
		}
		if (!bufer.empty()) {
			result.push_back(parsing_message(path, bufer, ymdh, first_message, num_str));
		}
		inFile.close();
		return result;
	}

	bool ParserFile::is_timestamp(std::wstring_view str) const {
		bool result = false;
		if (str.size() == 5 && str[2] == L':') {
			std::wstring_view m = str.substr(0, 2);
			std::wstring_view s = str.substr(3, 2);
			if (is_minute(m) && is_seconds(s)) {
				result = true;
			}
		}
		return result;
	}

	bool ParserFile::validate(const YearMonthDayHour& ymdh) const {
		return ymdh.year >= 2000 && std::chrono::year(ymdh.year) <= current_ymd.year() 
			&& ymdh.month >= 1 && ymdh.month <= 12 
			&& ymdh.day >= 1 && ymdh.day <= 31 
			&& ymdh.hh >= 0 && ymdh.hh <= 24;
	}

	YearMonthDayHour ParserFile::parsing_filename(const std::wstring& filename) const {
		std::wstring_view filename_view(filename);
		std::wstring_view year_str_v = filename_view.substr(0, 2);
		std::wstring_view month_str_v = filename_view.substr(2, 2);
		std::wstring_view day_str_v = filename_view.substr(4, 2);
		std::wstring_view hh_str_v = filename_view.substr(6, 2);
		if (!is_number(year_str_v) || !is_number(month_str_v) || !is_number(day_str_v) || !is_number(hh_str_v)) {
			throw std::invalid_argument("Error parsing filename, not number\n");
		}
		int year = std::stoi(std::wstring(year_str_v));
		year += 2000; // converting the year format from two-digit to four-digit
		int month = std::stoi(std::wstring(month_str_v));
		int day = std::stoi(std::wstring(day_str_v));
		int hh = std::stoi(std::wstring(hh_str_v));
		YearMonthDayHour ymdh{ .year = year, .month = month, .day = day, .hh = hh };
		if (!validate(ymdh)) {
			throw std::invalid_argument("Error parsing filename, not date");
		}
		return ymdh;
	}

	std::pair<std::wstring, std::wstring> ParserFile::parsing_property(const std::filesystem::path& path, std::wstring_view property, int num_str) const {
		std::vector<std::wstring_view> splited_property = parser_string::SplitView(property, L'=');
		std::wstring empty_str = L"";
		if (splited_property.size() == 1) { // example: ...,name_property=,...
			splited_property.push_back(std::wstring_view(property.end(), property.end()));
		}
		if (splited_property.size() < 2) {
			throw std::runtime_error("Error parsing property. File: " + path.string() + "; str: " + std::to_string(num_str));
		}
		std::wstring pr2;
		if (splited_property.size() > 2) {
			for (int i = 1; i < splited_property.size(); ++i) {
				pr2 += splited_property[i];
			}
		}
		else {
			pr2 = splited_property[1];
		}
		return std::pair<std::wstring, std::wstring>{std::wstring(splited_property[0]), pr2};
	}

	Message ParserFile::parsing_message(const std::filesystem::path& path, const std::wstring& message, const YearMonthDayHour& ymdh, bool first_message, int num_str) const {
		std::wstring_view message_view;
		if (first_message) {
			message_view = std::wstring_view(message.begin() + 1, message.end());
		}
		else {
			message_view = message;
		}
		std::wstring_view mm_ss_mcss = message_view.substr(0, message_view.find(L"-"));
		std::wstring_view mm_v = mm_ss_mcss.substr(0, 2);
		std::wstring_view ss_v = mm_ss_mcss.substr(3, 2);
		std::wstring_view mcss_v = mm_ss_mcss.substr(6);
		int mm = std::stoi(std::wstring(mm_v));
		int ss = std::stoi(std::wstring(ss_v));
		int mcss = std::stoi(std::wstring(mcss_v));
		std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds> timestamp = std::chrono::sys_days{ std::chrono::month(ymdh.month) / ymdh.day / ymdh.year }
		+ std::chrono::hours(ymdh.hh) + std::chrono::minutes(mm) + std::chrono::seconds(ss) + std::chrono::microseconds(mcss);
		std::vector<std::wstring_view> splited_one(parser_string::SplitViewTwoSymbols(message, L'\"', L'\''));
		std::vector<std::wstring> splited_event;
		for (int i = 0; i < splited_one.size(); ++i) {
			if (i % 2 == 0) {
				std::vector<std::wstring_view> splited_two(parser_string::SplitView(splited_one[i], L','));
				for (auto& el : splited_two) {
					splited_event.emplace_back(std::wstring(el));
				}
			}
			else {
				std::wstring& val_property = splited_event.back();
				val_property += L"\"";
				val_property += splited_one[i];
				val_property += L"\"";
			}
		}
		std::vector<std::pair<std::wstring, std::wstring>> properties;
		if (splited_event.size() < 3) {
			throw std::runtime_error("Error parsing event. File: " + path.string() + "; str: " + std::to_string(num_str));
		}
		for (int i = 3; i < splited_event.size(); ++i) { // we skip timestamp, event, and level message
			properties.emplace_back(parsing_property(path, splited_event[i], num_str));
		}
		return Message{ .point = timestamp, .event = std::wstring(std::move(splited_event[1])), .properties = std::move(properties)};
	}

	bool ParserFile::is_number(std::wstring_view number) const {
		bool result = true;
		for (char c : number) {
			if (!std::isdigit(c)) {
				result = false;
				break;
			}
		}
		return result;
	}

	bool ParserFile::is_hour(std::wstring_view hour) const {
		bool result = false;
		if (is_number(hour)) {
			int h = std::stoi(std::wstring(hour));
			result = (h >= 0 && h <= 23);
		}
		return result;
	}

	bool ParserFile::is_minute(std::wstring_view minute) const {
		bool result = false;
		if (is_number(minute)) {
			int m = std::stoi(std::wstring(minute));
			result = (m >= 0 && m <= 59);
		}
		return result;
	}

	bool ParserFile::is_seconds(std::wstring_view seconds) const {
		bool result = false;
		if (is_number(seconds)) {
			int ss = std::stoi(std::wstring(seconds));
			result = (ss >= 0 && ss <= 59);
		}
		return result;
	}

	std::wostream& operator<<(std::wostream& os, const Message& message) {
		os << message.point << L'\n';
		os << message.event << L'\n';
		for (const auto& el : message.properties) {
			os << el.first << L" = " << el.second << L'\n';
		}
		return os;
	}

	std::vector<Message> Parser::parsing(const std::filesystem::path& path) const {
		std::vector<std::filesystem::path> files(parsing_directory(path));
		ParserFile parser_file;
		std::vector<Message> result;
		for (const auto& file : files) {
			for (auto& message : parser_file.parsing(file)) {
				message.file = file;
				result.push_back(message);
			}
		}
		return result;
	}

	std::vector<std::filesystem::path> Parser::parsing_directory(const std::filesystem::path& path) const {
		std::vector<std::filesystem::path> result;
		if (!std::filesystem::is_directory(path)) {
			if (is_empty_utf_file(path)) {
				result.push_back(path);
			}
		}
		else {
			for (const auto& entry : std::filesystem::directory_iterator(path)) {
				for (const auto& p : parsing_directory(entry.path())) {
					result.push_back(p);
				}
			}
		}
		return result;
	}

	bool Parser::is_empty_utf_file(const std::filesystem::path& path) const {
		return std::filesystem::file_size(path) > size_empty_utf_file;
	}

} // namespace parser_tech_log_1c



