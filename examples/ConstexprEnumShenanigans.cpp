#include <iostream>
#include <format>

#include "../inc/const_enum.hpp"

enum e_example {
	FOO = 0,
	BAR,
	FOOBAR = 4,
	FIZZ,
	BUZZ,
	SIZEOF_ENUM
};

int main()
{
	/*
	* You can get an array matching your enum, containing all enum names as string_views.
	* If there are values not contained in your enum, they're replaced by an empty string.
	*/
	constexpr auto names_array = const_enum::get_enum_array<e_example, SIZEOF_ENUM>();
	for (auto const &i : names_array)
		std::cout << std::format("Enum value is: '{}'\n", i);

	/*
	* You can also just take a range in the enum instead of the whole thing:
	*/
	constexpr size_t offset = 4;
	constexpr auto ranged_names_array = const_enum::get_enum_array_range<e_example, offset, SIZEOF_ENUM>();
	for (auto const &i : ranged_names_array)
		std::cout << std::format("Enum value is: '{}' (from offset)\n", i);

	/*
	* Or simply get the string representation of any integer you'd like.
	* Once again, empty string means no matching enum value.
	*/
	constexpr int check_me = 6;
	constexpr auto name = const_enum::get_enum_member_name<e_example, check_me>();
	std::cout << std::format("Integer {} matches enum member '{}'\n", check_me, name);

	/*
	* If you just want to check if a certain value has a matching enum member, use this:
	*/
	constexpr bool has_member_3 = const_enum::is_enum_member_valid<e_example, 3>();
	constexpr bool has_member_4 = const_enum::is_enum_member_valid<e_example, 4>();
	std::cout << std::format("Contains 3: {}, contains 4: {}\n", has_member_3, has_member_4);

	/*
	* Once you have determined enum values at compile-time, it's very easy to get what
	* you want at runtime, just index the array. If you use a library with constexpr maps,
	* you could even make it so you have a constexpr map of values.
	*/
	for (int i = 0; i < SIZEOF_ENUM; ++i)
		std::cout << std::format("At runtime, {} corresponds to '{}'\n", i, names_array[i]);
}
