#pragma once

#include <string_view>
#include <array>

namespace const_enum
{

using namespace std::string_view_literals;

namespace detail
{

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

}	// namespace detail

/*
* The functions below allow you to extract enum member name/value pairs, at compile time.
* For example, if you have an enum, and want a map of that enum with a string of the member
* name as a key, associated with its value, you can just do that.
*
* This is really cool, it essentially gives you information that's only available at compile-time,
* which you can copy and use at runtime!
* 
* Using an index_sequence and parameter pack expansion (https://en.cppreference.com/w/cpp/language/parameter_pack),
* get_enum_array() returns an array containing the enum's string representation of all integers
* in the given sequence. If an index is invalid, the string is empty. Otherwise, the string
* contains the enum member's name.
*/


/*
* Tests whether or not the given integer matches a valid enum member.
*/
template <typename E, int EnumMember>
consteval bool	is_enum_member_valid()
{
	return detail::is_enum_member_valid<E, (E)EnumMember>();
}

/*
* Gets a string representation of the enum member matching given integer. If no such
* member exists, returns an empty string.
*/
template <typename E, int EnumMember>
consteval std::string_view	get_enum_member_name()
{
	return detail::get_enum_member_name<E, (E)EnumMember>();
}

/*
* Returns an array containing the string representation of all enum members in the given
* integer sequence. If a member does not correspond to an integer, its string representation
* is an empty string.
*/
template <typename E, size_t... Is>
consteval auto	get_enum_array(std::index_sequence<Is...> sequence)
{
	constexpr std::array<std::string_view, sequence.size()>	array = {
		detail::get_enum_member_name<E, (E)Is>()...
	};
	return array;
}

/*
* Returns an array containing the string representation of all enum members up to N.
* If a member does not correspond to an integer, its string representation is an empty string.
*/
template <typename E, size_t N>
consteval std::array<std::string_view, N>	get_enum_array()
{
	return get_enum_array<E>(std::make_index_sequence<N>{});
}

/*
* Returns an array containing the string representation of all enum members in the range [Start;End).
* If a member does not correspond to an integer, its string representation is an empty string.
*/
template <typename E, size_t Start, size_t End>
consteval auto	get_enum_array_range()
{
	static_assert(Start <= End, "Invalid range given to " __FUNCTION__);
	return get_enum_array<E>(detail::make_offset_index_sequence<Start, End - Start>());
}

}	// namespace constexpr enum