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


#include "../Directories.h"
#include "../Filesystem.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <string>
#include <system_error>
#include <vector>

#include <dirent.h>
#include <pwd.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

static string GetDataHome()
{
	// https://specifications.freedesktop.org/basedir-spec/latest/
	const char* xdg = getenv("XDG_DATA_HOME");
	if(xdg && xdg[0] == '/')
		return xdg;

	const char* home = getenv("HOME");
	if(!home || !home[0])
	{
		struct passwd* pw = getpwuid(getuid());
		home = pw ? pw->pw_dir : NULL;
	}
	return string(home && home[0] ? home : ".") + "/.local/share";
}

CStr GetAppDataDir()
{
	static const string sDir = GetDataHome() + "/Tread Marks/";
	CreateDirectories(sDir.c_str());
	return CStr(sDir.c_str());
}

CStr GetCommonAppDataDir()
{
	return GetAppDataDir();
}

static bool Exists(const string& path)
{
	struct stat st;
	return stat(path.c_str(), &st) == 0;
}

// Looks for an entry in dir whose name matches name ignoring case.
static bool FindEntryIgnoringCase(const string& dir, string& name)
{
	DIR* d = opendir(dir.empty() ? "." : dir.c_str());
	if(!d)
		return false;

	bool found = false;
	while(struct dirent* entry = readdir(d))
	{
		if(strcasecmp(entry->d_name, name.c_str()) == 0)
		{
			name = entry->d_name;
			found = true;
			break;
		}
	}
	closedir(d);
	return found;
}

CStr ResolvePath(const char* path)
{
	if(!path)
		return CStr();

	string in(path);
	replace(in.begin(), in.end(), '\\', '/');
	if(in.empty() || Exists(in))
		return CStr(in.c_str());

	// Walk the path one component at a time, fixing the case of each one that
	// only exists under a different spelling.  Once a component is missing
	// entirely, everything after it is kept as given.
	string out;
	string::size_type pos = 0;
	bool missing = false;
	while(pos <= in.size())
	{
		string::size_type slash = in.find('/', pos);
		if(slash == string::npos)
			slash = in.size();

		string component = in.substr(pos, slash - pos);
		if(!component.empty() && component != "." && component != ".." && !missing && !Exists(out + component))
		{
			if(!FindEntryIgnoringCase(out, component))
				missing = true;
		}
		out += component;

		if(slash < in.size())
			out += '/';
		pos = slash + 1;
	}

	return CStr(out.c_str());
}

FILE* OpenFile(const char* path, const char* mode)
{
	if(!path || !mode)
		return NULL;
	return fopen(ResolvePath(path).get(), mode);
}

static bool HasGameData()
{
	FILE* f = OpenFile("Local/Language.TXT", "rb");
	if(f)
		fclose(f);
	return f != NULL;
}

bool ChangeToGameDirectory(const char* argv0)
{
	if(HasGameData())
		return true;

	vector<string> aDirs;
	char buf[4096];
	ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
	if(len > 0)
	{
		buf[len] = '\0';
		aDirs.push_back(fs::path(buf).parent_path().string());
	}
	if(argv0 && strchr(argv0, '/'))
		aDirs.push_back(fs::path(argv0).parent_path().string());

	for(const string& sDir : aDirs)
	{
		if(!sDir.empty() && chdir(sDir.c_str()) == 0 && HasGameData())
			return true;
	}
	return false;
}

bool CreateDirectories(const char* path)
{
	if(!path || !path[0])
		return false;

	error_code ec;
	fs::path dir(ResolvePath(path).get());
	fs::create_directories(dir, ec);
	return fs::is_directory(dir, ec);
}
