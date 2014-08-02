// PhilipsContactConvert.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "VCard.h"
#include "encoding_utils.h"


int _tmain(int argc, _TCHAR* argv[])
{
	_TCHAR const* fileName;
	if (argc > 1)
	{
		fileName = argv[1];
	}
	else
	{
		std::cerr << "Error: No input file provided." << std::endl;
		return 1;
	}
	//std::wstring fileName = L"c:\\Users\\snake\\Downloads\\contacts.vcf";
	
	std::vector<vcard::Contact> contact;
	
	std::wifstream in;
	in.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
	in.open(fileName, std::wifstream::in);
	if (in.is_open())
	{
		vcard::Parse(in, contact);
		std::cout << "Read contacts: " << contact.size() << std::endl;
	}
	in.close();


	std::ofstream out;
	out.open("contacts_sim.csv", 
		std::ofstream::binary | std::ofstream::out | std::ofstream::trunc);


	std::locale global_locale;

	out <<	"Name\t"
			"Mobile\t"
			"Home\t"
			"Company\t"
			"Email\t"
			"Office\t"
			"Fax\t"
			"Birthday\n";

	for (const auto& con : contact)
	{
		std::wstring name = (!con.mName.mLastName.empty()) ?
			con.mName.mLastName + L" " + con.mName.mFirstName : con.mFormatedName;
		
		std::map< std::wstring, int > typeIndex;
		
		for (const auto& phone : con.mPhone)
		{
			std::wstring telTypeStr;
			if (!phone.mType.empty())
			{
				telTypeStr = phone.mType[0];
				for (size_t i = 1; i < phone.mType.size(); i++)
					telTypeStr += L',' + phone.mType[i];
			}



			auto find_id = typeIndex.find(telTypeStr);
			if (find_id != typeIndex.end())
				(*find_id).second++;
			else
			{
				auto ret = typeIndex.insert(std::make_pair(telTypeStr, 0));
				find_id = ret.first;
			}


			std::wstring nameAndType = name;

			if (!telTypeStr.empty())
			{
				nameAndType += L'(' + find_id->first;

				if (find_id->second != 0)
					nameAndType += L' ' + std::to_wstring(find_id->second);
					
				nameAndType += L')';
			}

			std::replace(	nameAndType.begin(), nameAndType.end(), L'\"', L'\'' );

			out << '\"' << wchar2char(nameAndType)   << '\"' << '\t';
			out << '\"' << wchar2char(phone.mNumber) << '\"' << '\t';

			for (char i = 0; i < 6; i++)
				out << "\"\"\t";

			out << std::endl;

		}//for (const auto& phone : con.mPhone)
	}
	out.close();
		

	




	return 0;
}


