#pragma once

#include <regex>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <filesystem>
#include <chrono>
#include <string_view>
#include <iostream>
#include <utility>

namespace parser_tech_log_1c {

	struct Message
	{
		std::filesystem::path file;
		std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds> point;
		std::wstring event;
		int level_message;
		std::vector<std::pair<std::wstring, std::wstring>> properties;
	};

	std::wostream& operator<<(std::wostream& os, const Message& message);

	struct YearMonthDayHour
	{
		int year;
		int month;
		int day;
		int hh;
	};

	class ParserFile
	{
	public:
		ParserFile(const std::regex& in_r);
		ParserFile();

		std::vector<Message> parsing(const std::filesystem::path& path);

	private:
		bool is_timestamp(std::wstring_view str) const;

		Message parsing_message(const std::filesystem::path& path, const std::wstring& message, const YearMonthDayHour& ymdh, bool first_message, int num_str) const;

		YearMonthDayHour parsing_filename(const std::wstring& filename) const;

		std::pair<std::wstring, std::wstring> parsing_property(const std::filesystem::path& path, std::wstring_view property, int num_str) const;

		bool is_number(std::wstring_view number) const;

		bool is_hour(std::wstring_view hour) const;

		bool is_minute(std::wstring_view minute) const;

		bool is_seconds(std::wstring_view seconds) const;

		bool validate(const YearMonthDayHour& ymdh) const;

		std::chrono::year_month_day current_ymd;
		std::wifstream in_file;

		const std::regex r;
	};

	class Parser {
	public:

		std::vector<Message> parsing(const std::filesystem::path& path) const;

		template<class ExecutionPolicy>
		std::vector<Message> parsing(ExecutionPolicy&& policy, const std::filesystem::path& path) const;

	private:
		std::vector<std::filesystem::path> parsing_directory(const std::filesystem::path&) const;

		bool is_empty_utf_file(const std::filesystem::path& path) const;

		size_t size_empty_utf_file = 3;

		struct Cache {
			std::vector<Message> messages;
			std::vector<std::pair<YearMonthDayHour, YearMonthDayHour>> requests;
		};

		Cache cache;
	};

	template<class ExecutionPolicy>
	std::vector<Message> Parser::parsing(ExecutionPolicy&& policy, const std::filesystem::path& path) const {
		std::vector<std::filesystem::path> pathes(parsing_directory(path));
		std::vector<std::pair<std::filesystem::path, std::vector<Message>>> messages;
		for (const auto& path : pathes) {
			messages.emplace_back(std::pair<std::filesystem::path, std::vector<Message>>{path, std::vector<Message>()});
		}

		auto parsing_file = [](std::pair<std::filesystem::path, std::vector<Message>>& el) {
			ParserFile parser_file;
			for (auto& message : parser_file.parsing(el.first)) {
				message.file = el.first;
				el.second.push_back(message);
			}
			};

		std::for_each(std::forward<ExecutionPolicy>(policy), messages.begin(), messages.end(), parsing_file);
		std::vector<Message> result;
		for (auto& el : messages) {
			for (auto& message : el.second) {
				result.emplace_back(std::move(message));
			}
		}
		return result;
	}

}// namespace parser_tech_log_1c

