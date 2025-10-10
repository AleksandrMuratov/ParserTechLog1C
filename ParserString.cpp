#include "ParserString.h"

namespace parser_string {

	std::vector<std::wstring_view> SplitView(std::wstring_view str, wchar_t separator) {
		std::vector<std::wstring_view> result;
		while (!str.empty()) {
			auto pos_separator = str.find(separator);
			if (pos_separator == std::string::npos) {
				pos_separator = str.size();
			}
			if (pos_separator != 0) {
				std::wstring_view prefix = str.substr(0, pos_separator);
				result.push_back(prefix);
			}
			if (pos_separator == str.size()) {
				str.remove_prefix(pos_separator);
			}
			else {
				str.remove_prefix(pos_separator + 1);
			}
			
		}
		return result;
	}

	std::vector<std::wstring_view> SplitViewTwoSymbols(std::wstring_view str, wchar_t separator_priority_one, wchar_t separator_priority_two) {
		std::vector<std::wstring_view> result;
		std::wstring_view str_one = str;
		std::vector<std::wstring_view> one = SplitView(str_one, separator_priority_one);
		for (int i = 0; i < one.size(); ++i) {
			if (i % 2 == 0) {
				std::vector<std::wstring_view> two = SplitView(one[i], separator_priority_two);
				for (auto& el : two) {
					result.push_back(el);
				}
			}
			else {
				result.push_back(one[i]);
			}
		}
		return result;
	}

} // namespace parser_string