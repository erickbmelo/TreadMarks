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


#ifndef __DIRECTORIES_H__
#define __DIRECTORIES_H__

#include "CStr.h"

#include <cstdio>

// These are a couple functions I've added to find the AppData and CommonAppData
// directories on your Windows install.  You'll find different versions of these
// functions peppered throughout our products from when I Vista-proofed them.
// -Rick-
//
// Both return a path ending in '/' and create the directory if needed.  On
// Unix-like systems there is no machine-wide directory every user can write
// to, so both return the same per-user directory there.

CStr GetAppDataDir();
CStr GetCommonAppDataDir();

// The game and its data files were written on Windows, so paths throughout the
// code and data use '\\' separators and don't always match the case of the
// files on disk.  ResolvePath() turns such a path into one the local
// filesystem understands: separators become '/', and on case-sensitive
// filesystems every component that exists on disk under a different case is
// replaced by its on-disk spelling.  Components that don't exist are left as
// given, so the result can also name a file about to be created.
CStr ResolvePath(const char* path);

// fopen() on ResolvePath(path).  Use this instead of fopen() for any path that
// came from the game's code or data files.
FILE* OpenFile(const char* path, const char* mode);

// Creates the directory and any missing parents.  Returns true if the
// directory exists afterwards.
bool CreateDirectories(const char* path);

// The game loads its data through paths relative to the current directory.
// If the data isn't there, this changes to the directory holding the
// executable, so the game can be started from anywhere.  Returns true if the
// game's data is reachable afterwards.
bool ChangeToGameDirectory(const char* argv0);

#endif
