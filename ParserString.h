#pragma once
#include <vector>
#include <string>

namespace parser_string {

	std::vector<std::wstring_view> SplitView(std::wstring_view, wchar_t separator);

	std::vector<std::wstring_view> SplitViewTwoSymbols(std::wstring_view str, wchar_t separator_priority_one, wchar_t separator_priority_two);

} // namespace parser_string