#include "stdafx.h"
#include "VCard.h"
#include "error_handler.h"


namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;
namespace repo = boost::spirit::repository::qi;

using  namespace vcard;


template <typename It, typename Skipper>
struct ContactParser
	: qi::grammar<It, std::vector<Contact>(), Skipper>
{
	//px::function<ErrorHandler> 
	phx::function<error_handler_item_skipper<It>> const mErrorHandler;
	
	
	
	ContactParser()
	:ContactParser::base_type(rContactVec, "rContactVec")
	{
		//using boost::spirit::repository::qi::kwd;
		using boost::spirit::standard_wide::char_;
		using boost::spirit::qi::rule;
		using boost::spirit::qi::lit;
		using boost::spirit::qi::lexeme;
		using boost::spirit::qi::eol;
		using boost::spirit::qi::omit;
		using boost::spirit::qi::eps;
		using boost::spirit::qi::eoi;
		using repo::seek;

		using qi::on_error;
		using qi::fail;
		using qi::accept;
		using qi::retry;

		using namespace qi::labels;

		using boost::phoenix::construct;
		using boost::phoenix::val;




		// вспомогательные правила
		rNameVal %= lexeme[*(char_ - KW_PC - eol)];	// имя 
		rParamVal %= *(char_ - KW_DP - KW_C - KW_PC);	// параметр 
		rTelNumVal %= lexeme[*(char_ - eol)];

		beginTag %= lit(KW_BEGIN) > KW_DP > KW_VCARD > eol;
		endTag %= lit(KW_END) > KW_DP > KW_VCARD > eol;


		rFormName %= seek[KW_FN - endTag] > KW_DP > lexeme[*(char_ - eol)] > eol;

		rName %= seek[KW_N - endTag] > KW_DP > 
			rNameVal > KW_PC > rNameVal > KW_PC > rNameVal > KW_PC >
			rNameVal > KW_PC > rNameVal > eol;

		rTelType %= lit(KW_TYPE) > KW_EQ > rParamVal % KW_C;  //kwd(KW_TYPE, 0, 1)['=' >> rTelTypeVec]



		rTel %= lit(KW_TEL) >> *(KW_PC > rTelType) > KW_DP > rTelNumVal > eol;


		rSkipString %= omit[*(char_ - eol)] > eol;

		rSeekTel %= *(rSkipString - KW_TEL - endTag) >> rTel;

		rTelVec %= *(rSeekTel - endTag);

		rContact %= eps
					> beginTag
					> rFormName
					> rName
					> rTelVec
					> *(rSkipString - endTag)
					> endTag
					;


		rContactVec %= *(rContact - eoi >> *eol );  //	//*((eps > (rContact | eoi)) - eoi);



		// error handling names
		rSkipString.name("rSkipString");
		beginTag.name("beginTag");
		endTag.name("endTag");

		rFormName.name("rFormName");
		rNameVal.name("rNameVal");
		rTelNumVal.name("rTelNumVal");
		rParamVal.name("rParamVal");

		rName.name("rName");

		rTelType.name("rTelType");

		rTel.name("rTel"); rSeekTel.name("rSeekTel");

		rContact.name("rContact");

		rContactVec.name("rContactVec");

		rTelVec.name("rTelVec");


		using namespace qi;

		
		
		on_error<retry>(rContactVec, mErrorHandler(_1, _2, _3, _4));
		
		/*
		// восстановление ошибок путём пропуска строк (элементов)
		// сделаное на фениксе, лямбда обработки ошибки гененрится фениксом ( злое колдунство :-) )
		// как только сработал экцепшн перемещаем начало парсера на строку с ошибкой phx::if_(!at_eoi)[_1 = _3],
		// затем пропускаем символы до \n
		// пропускаем \n
		// парсер перезапускается с новыми параметрами итератор начала first перемещается на строку идущюю после ошибочеой строки

		
		static auto const at_eol = (*_1 == L'\n') || (*_1 == L'\r');
		static auto const at_eoi = (_1 == _2);

		on_error<retry>(rContactVec,
			(
			(phx::ref(std::cout) << "\n Error! expecting: " << _4 << " here: " << std::endl << construct<std::string>(_3, _2 ) << std::endl),
			phx::if_(!at_eoi)[_1 = _3, phx::ref(std::cout) << "\n move first parsing pos to error pos" ],
			phx::while_(!at_eoi && !at_eol)[++_1, phx::ref(std::cout) << "\n advance to newline"],
			phx::while_(!at_eoi && at_eol)[++_1, phx::ref(std::cout) << "\n eat newline"],
			phx::if_(at_eoi)[_pass = fail]
			)
			);
		*/
		
		/*
		// выбрасывать сообщение на ошибке и прекращать парсинг - скопировано с документашки
		on_error<fail >(rContactVec,
			std::cout
			<< val("Error! Expecting ")
			<< _4                               // what failed?
			<< val(" here:") << std::endl
			//<< construct<std::string>(_3, _2 ) 
			<< std::endl
			);
		*/

	}

	// 
	

	

	qi::rule<It, Skipper> rSkipString, beginTag, endTag ;
	qi::rule<It, std::wstring(), Skipper> rFormName, rNameVal, rTelNumVal, rParamVal;
	qi::rule<It, Name(), Skipper> rName;
	qi::rule<It, std::vector<std::wstring>(), Skipper> rTelType;
	qi::rule<It, Phone(), Skipper> rTel, rSeekTel;
	qi::rule<It, Contact(), Skipper> rContact;

	qi::rule<It, std::vector<Contact>(), Skipper> rContactVec;

	qi::rule<It, std::vector<Phone>(), Skipper> rTelVec;

};





//-----------------------------------------------------------------------------
template <typename It, typename Skipper, typename Result>
void doParseContact(It first,It last, const Skipper& skipper, Result& ret)
{
	ContactParser<decltype(last), Skipper> p;
	bool ok = qi::phrase_parse(first, last, p, skipper, ret);

	if (!ok)
		std::cout << "parse failed\n";
	if (first != last)
		std::cout << "parse errors\n";
}


//-----------------------------------------------------------------------------
void vcard::Parse(const std::wstring& input, std::vector<Contact>& ret)
{
	auto first(std::begin(input)), last(std::end(input));
	
	doParseContact(first, last, qi::standard_wide::blank, ret);

}

//-----------------------------------------------------------------------------
void vcard::Parse(std::wifstream& in, std::vector<Contact>& ret)
{
	in.unsetf(std::ios::skipws);

	typedef std::istreambuf_iterator<wchar_t> base_iterator_type;
	
	boost::spirit::multi_pass<base_iterator_type> first =
		boost::spirit::make_default_multi_pass(base_iterator_type(in));

	boost::spirit::multi_pass<base_iterator_type> last =
		boost::spirit::make_default_multi_pass(base_iterator_type());

	doParseContact(first, last, qi::standard_wide::blank, ret);

	/*
	// это для потоков и парсера без перехватчика+обработчика ошибок
	typedef boost::spirit::basic_istream_iterator<wchar_t> wistream_iterator;

	in.unsetf(std::ios::skipws);
	//boost::spirit::istream_iterator f(in);
	//boost::spirit::istream_iterator l;
	wistream_iterator f(in);
	wistream_iterator l;

	ContactParser<decltype(f), qi::standard_wide::blank_type > p;

	bool ok = qi::phrase_parse(f, l, p, qi::standard_wide::blank, ret);

	if (!ok)
		std::cout << "parse failed\n";
	if (f != l)
		std::cout << "parse errors\n";
	*/
		
}

