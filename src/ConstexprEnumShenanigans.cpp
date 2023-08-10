#include <iostream>
#include <format>

#include "../inc/enum.hpp"

enum e_example {
	FOO = 0,
	BAR,
	FOOBAR = 6,
	FIZZ,
	BUZZ,
	SIZEOF_ENUM
};

int main()
{
	std::cout << "get_enum_array:\n";
	/*
	* You can get an array matching your enum, containing all enum names as string_views.
	* If there are values not contained in your enum, they're replaced by an empty string.
	*/
	constexpr auto names_array = const_enum::get_enum_array<e_example, SIZEOF_ENUM>();
	for (auto const &i : names_array)
		std::cout << std::format("Enum value is: '{}'\n", i);

	std::cout << "\nget_enum_array with offset:\n";
	/*
	* If you want to start your array with an offset in the enum, add an offset in the
	* std::index_sequence with this helper function:
	*/
	constexpr auto offset_names_array = const_enum::get_enum_array<e_example, 3>(
		const_enum::make_offset_index_sequence<FOOBAR, 3>());
	for (auto const &i : offset_names_array)
		std::cout << std::format("Enum value is: '{}' (from offset)\n", i);

	std::cout << "\nget_enum_member_name:\n";
	/*
	* Or simply get the string representation of any integer you'd like.
	* Once again, empty string means no matching enum value.
	*/
	constexpr int check_me = 7;
	constexpr auto name = const_enum::get_enum_member_name<e_example, (e_example)check_me>();
	std::cout << std::format("Integer {} matches enum member '{}'\n", check_me, name);

	std::cout << "\nis_enum_member_valid:\n";
	/*
	* If you just want to check if a certain value has a matching enum member, use this:
	*/
	constexpr bool has_member = const_enum::is_enum_member_valid<e_example, (e_example)4>();
	std::cout << std::format("Enum member valid: {}", has_member);
}
