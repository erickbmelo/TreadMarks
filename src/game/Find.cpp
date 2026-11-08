// This file is part of Tread Marks
// 
// Tread Marks is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Tread Marks is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Tread Marks.  If not, see <http://www.gnu.org/licenses/>.

//File finder wrapper for Windows 32 by Seumas McNally.

#include "Find.h"
#include "TextLine.hpp"
#include "Directories.h"
#include "Filesystem.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <string>
#include <system_error>
#include <vector>
using namespace std;

// Matches a file name against a wildcard where '*' matches any run of
// characters and '?' matches any single character.  Like on Windows, case is
// ignored.
static bool WildcardMatch(const char* pattern, const char* name)
{
	const char* star = NULL;
	const char* retry = NULL;
	while(*name)
	{
		if(*pattern == '*')
		{
			star = pattern++;
			retry = name;
		}
		else if(*pattern == '?' || tolower((unsigned char)*pattern) == tolower((unsigned char)*name))
		{
			pattern++;
			name++;
		}
		else if(star)
		{
			pattern = star + 1;
			name = ++retry;
		}
		else
			return false;
	}
	while(*pattern == '*')
		pattern++;
	return *pattern == '\0';
}

static bool LessIgnoringCase(const string& a, const string& b)
{
	return lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(),
		[](char x, char y) { return toupper((unsigned char)x) < toupper((unsigned char)y); });
}

Find::Find(){
	Clear();
}
Find::Find(const char *name){
	Clear();
	Search(name);
}
Find::~Find(){
	Clear();
}

bool Find::Clear(){
	nItems = 0;
	ListHead.DeleteList();
	return true;
}
int Find::Search(const char *name, bool recursive, const char *addpath){
	Clear();
	return AddSearch(name, recursive, addpath);
}
int Find::AddSearch(const char *name, bool recursive, const char *addpath){
	int found = 0, db;

	if(name)
	{
		if(!addpath)
			addpath = "";

		//First find and search subdirectories.
		CStr sName = ResolvePath(name);
		for(db = sName.len() - 1; db > 0 && sName.chr(db) != '/' && sName.chr(db) != '\\' && sName.chr(db) != ':'; db--);
		std::string left(Left(sName, db + 1)), right(Mid(sName, db + 1));	//Break up path and wildcard.
		if(left == "") left = "./";

		error_code ec;
		if(fs::is_directory(left, ec))
		{
			// Gather the names first so results come back in the same order on
			// every platform.  NTFS returns directory entries sorted without
			// regard to case, and the order maps and tanks are enumerated in
			// affects the game.
			struct Entry { std::string sName; bool bDir; };
			vector<Entry> aEntries;
			for(fs::directory_iterator iter(left, ec), end; !ec && iter != end; iter.increment(ec))
			{
				std::string sFileName = iter->path().filename().string();
				error_code ecStatus;
				fs::file_status status = iter->status(ecStatus);
				if(fs::is_directory(status))
				{
					if(recursive)
						aEntries.push_back({sFileName, true});
				}
				else if(fs::is_regular_file(status) && WildcardMatch(right.c_str(), sFileName.c_str()))
					aEntries.push_back({sFileName, false});
			}
			sort(aEntries.begin(), aEntries.end(), [](const Entry& a, const Entry& b) { return LessIgnoringCase(a.sName, b.sName); });

			for(const Entry& entry : aEntries)
			{
				if(entry.bDir)
					found += AddSearch((left + entry.sName + "/" + right).c_str(), recursive, (std::string(addpath) + entry.sName + "/").c_str());
				else if(ListHead.AddItem(new CStr(CStr(addpath) + entry.sName.c_str())))
				{
					found++;
					nItems++;
				}
			}
		}
	}

	return found;
}
const char *Find::operator[](int n){
	CStrList *lp = ListHead.NextLink();
	if(lp && n >= 0 && n < nItems){
		while(n > 0 && lp){
			lp = lp->NextLink();
			n--;
		}
		if(lp) return lp->Data->get();
	}
	return NULL;
}
