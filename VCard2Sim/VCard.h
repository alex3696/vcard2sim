#ifndef __VCARD_H
#define __VCARD_H

#include "stdafx.h"

//-----------------------------------------------------------------------------
// Структуру VCard считаем такой 
//BEGIN:VCARD
//FN : 1
//N : 1; 1; 1; 1; 1
//TEL; TYPE = WORK, CELL  :+1
//END : VCARD
// 
// строки, не начинающиеся с ключевых слов FN, N, TEL игнорируются парсером

namespace vcard
{

//-----------------------------------------------------------------------------
// ключевые слова вытащил когда переводил на юникод
// тут о юникоде http://stackoverflow.com/questions/402283/stdwstring-vs-stdstring
// так же нужны для хандлера ошибок, чтоб пропускать ошибочный блок
//-----------------------------------------------------------------------------
#define KW_VCARD	L"VCARD"
#define KW_BEGIN	L"BEGIN"
#define KW_END		L"END"

#define KW_FN		L"FN"
#define KW_N		L"N"
#define KW_TEL		L"TEL"
#define KW_TYPE		L"TYPE"
#define KW_PREF		L"PREF"

#define KW_DP		L':'
#define KW_PC		L';'
#define KW_C		L','
#define KW_EQ		L'='




//-----------------------------------------------------------------------------
// структуры VCard
//-----------------------------------------------------------------------------
struct Phone
{
	std::vector<std::wstring>		mType;
	std::wstring					mNumber;
};


//-----------------------------------------------------------------------------
struct Name
{
	std::wstring mLastName;			// фамилия
	std::wstring mFirstName;		// имя
	std::wstring mAdditionalName;	// отчество
	std::wstring mPrefix;			// префикс имени
	std::wstring mSuffix;			// постфикс имени
};


//-----------------------------------------------------------------------------
struct Contact
{
	std::wstring		mFormatedName;	// форматированное имя
	Name				mName;			// полное имя
	std::vector<Phone>	mPhone;			// телефон
};


//-----------------------------------------------------------------------------
void Parse(std::wifstream & in, std::vector<Contact>& ret);
void Parse(const std::wstring& in, std::vector<Contact>& ret);


}//namespace vcard


//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOST_FUSION_ADAPT_STRUCT(
	vcard::Phone,
	(std::vector< std::wstring >,	mType)
	(std::wstring,					mNumber)
	);
//-----------------------------------------------------------------------------
BOOST_FUSION_ADAPT_STRUCT(
	vcard::Name,
	(std::wstring, mLastName)
	(std::wstring, mFirstName)
	(std::wstring, mAdditionalName)
	(std::wstring, mPrefix)
	(std::wstring, mSuffix)
	);
//-----------------------------------------------------------------------------
BOOST_FUSION_ADAPT_STRUCT(
	vcard::Contact,
	(std::wstring,					mFormatedName)
	(vcard::Name,					mName)
	(std::vector< vcard::Phone >,	mPhone)
	);
//-----------------------------------------------------------------------------
#endif //