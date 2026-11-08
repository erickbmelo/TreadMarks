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

#include "../Web.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void OpenWebLink(const char* link)
{
#ifdef __APPLE__
	const char* opener = "open";
#else
	const char* opener = "xdg-open";
#endif

	// Fork twice so the browser launcher is reparented to init and never
	// lingers as a zombie of the game process.
	pid_t child = fork();
	if(child == 0)
	{
		setsid();
		if(fork() == 0)
		{
			execlp(opener, opener, link, (char*)NULL);
		}
		_exit(0);
	}
	else if(child > 0)
	{
		waitpid(child, NULL, 0);
	}
}
