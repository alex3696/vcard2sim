#ifndef __ERROR_HANDLER
#define __ERROR_HANDLER

#include "stdafx.h"
#include "VCard.h"

// аналог перехватчика ошибок написанного на фениксе
// с той разницей, что он будкт искать конец строки/ конец блока end:vcard
// смотреть тут file:///C:/_prog/LIB/boost_1_55_0/libs/spirit/example/support/utree/error_handler.hpp

///////////////////////////////////////////////////////////////////////////////
//  The error handler
///////////////////////////////////////////////////////////////////////////////
template <typename Iterator>
struct error_handler_item_skipper
{
	template <class, class, class, class>
	struct result{ typedef void type; };

	template <class Info, class Iter>
	void operator () (Iter &first, Iter const &last, Iter const &err, const Info &what) const
	{
		Iter errEnd = err;
		short maxErrSym = 50;
		while (!(last == errEnd++) && maxErrSym--)
			;
		auto errPlace = phx::construct<std::string>(err, errEnd)();

		std::cout << "\n Error! expecting: " << what << " here: " << std::endl
			<< errPlace << " ... move to next beginTag \n" << std::endl;

		using namespace qi;
		// TODO: как узнать тип скипера из итератора ??? похоже никак
		qi::rule<Iter, standard_wide::blank_type > beginTag, endTag;
		beginTag %= lit(KW_BEGIN) >> KW_DP >> KW_VCARD >> eol;

		first = err;
		bool r = phrase_parse(first, last, (omit[*(char_ - beginTag - eoi)]), standard_wide::blank);

	}
};





/*
// аналог перехватчика ошибок написанного на фениксе
struct err_impl
{
	template <class, class, class, class>
	struct result{ typedef void type; };

	template <class Info, class Iter>
	void operator () (Iter &first, Iter const &last, Iter const &err, const Info &what) const
	{
		std::cout << "\n Error! expecting: " << what << " here: " << std::endl
			<< phx::construct<std::string>(err, last)() << std::endl;
		
		first = err; std::cout << "\n move first parsing pos";

		auto at_eol = [&](){ return  (*first == L'\n') || (*first == L'\r'); };
		auto at_eoi = [&](){ return  (first == last); };

		while (!at_eoi() && !at_eol())
		{
			++first;
			std::cout << "\n advance to newline";
		}

		while (!at_eoi() && at_eol())
		{
			++first;
			std::cout << "\n eat newline";
		}

		if (at_eoi())
		{
			// _pass = fail ??
		}
	
	}
};
*/



#endif //