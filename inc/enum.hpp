#pragma once

#include <string_view>
#include <array>

namespace const_enum
{

/*
* The functions below allow you to extract enum member name/value pairs, at compile time.
* For example, if you have an enum, and want a map of that enum with a string of the member
* name as a key, associated with its value, you can just do that.
*
* This is really cool, it essentially gives you information that's only available at compile-time,
* which you can copy and use at runtime!
*/

using namespace std::string_view_literals;

/*
* When this function gets templated with a valid number casted to the enum, its signature looks something
* like "bool isEnumMemberValid<enumType, enumMemberName>()". But when it's invocated with an
* invalid number casted to the enum, it looks like "bool isEnumMemberValid<enumType, (enumType)42>()".
* If there's a parenthesis, your enum does not contain this integer!
*
* So, calling this function with an arbitrary int, like isEnumMemberValid<MyEnum, (MyEnum)42>() tells
* you whether your enum contains the value 42 or not.
* This technique is used along with parameter pack expansion in the code below.
*/
template <typename E, E EnumMember>
consteval bool	is_enum_member_valid()
{
	constexpr std::string_view	funcsig = __FUNCSIG__;
	constexpr size_t			start = funcsig.find(',', funcsig.rfind('<')) + 1;
	static_assert(start != std::string_view::npos);
	return funcsig[start] != '(';
}

/*
* Using the same signature trick as above, this extracts the enum member's name at compile-time.
* Only working with string_view allows easy compile-time work.
*/
template <typename E, E EnumMember>
consteval std::string_view	get_enum_member_name()
{
	constexpr std::string_view	funcsig = __FUNCSIG__;
	constexpr size_t			start = funcsig.find(',', funcsig.rfind('<')) + 1;
	constexpr size_t			end = funcsig.rfind('>');
	static_assert(start != std::string_view::npos && end != std::string_view::npos);

	if (funcsig[start] == '(')
		return ""sv;
	else
		return std::string_view(&funcsig[start], end - start);	// Using iterators here fucks things up for some reason. Some sort of constexpr stack/step limit?
}

/*
* Using an index_sequence and parameter pack expansion (https://en.cppreference.com/w/cpp/language/parameter_pack),
* this returns an array containing the enum representation of all numbers in the given sequence.
* If an index is invalid, the string is empty. Otherwise, the string contains the enum member's name.
*/
template <typename E, size_t N, size_t... Is>
consteval std::array<std::string_view, N>	get_enum_array(std::index_sequence<Is...>)
{
	constexpr std::array<std::string_view, N>	array = {
		get_enum_member_name<E, (E)Is>()...
	};
	return array;
}

template <typename E, size_t N>
consteval std::array<std::string_view, N>	get_enum_array()
{
	return get_enum_array<E, N>(std::make_index_sequence<N>{});
}

// From https://stackoverflow.com/questions/35625079/offset-for-variadic-template-integer-sequence
template <std::size_t Offset, std::size_t ... Is>
constexpr std::index_sequence<(Offset + Is)...> add_offset(std::index_sequence<Is...>)
{
	return {};
}

template <std::size_t Offset, std::size_t N>
constexpr auto make_offset_index_sequence()
{
	return add_offset<Offset>(std::make_index_sequence<N>{});
}

}	// namespace constexpr enum