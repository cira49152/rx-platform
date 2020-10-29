

/****************************************************************************
*
*  gnu_hosts\rx_gnu_common.cpp
*
*  Copyright (c) 2020 ENSACO Solutions doo
*  Copyright (c) 2018-2019 Dusan Ciric
*
*
*  This file is part of rx-platform
*
*
*  rx-platform is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  rx-platform is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/


#include "pch.h"


// rx_gnu_common
#include "gnu_hosts/rx_gnu_common.h"


namespace gnu
{
std::string get_full_path(const std::string& base)
{
	char lpath[PATH_MAX + 1];
	char buff[PATH_MAX + 1];
	//memset(buff,0,sizeof(buff)); // readlink does not null terminate!
	// does not need this, we'll read the place where to put zero
	// struct stat info;
	int ret;
	pid_t pid = getpid();
	sprintf(lpath, "/proc/%d/exe", pid);
	ret = readlink(lpath, buff, PATH_MAX);
	if (ret == -1)
		perror("readlink");
	else
	{
		// now plase zero at the end!
		buff[ret] = '\0';
	}
	if (ret != -1)
	{
		size_t j = strlen(buff);
		for (size_t i = j - 1; i > 0; i--)
		{
			if (buff[i] == L'/')
			{
				buff[i + 1] = L'\0';
				break;
			}
		}

		strcat(buff, base.c_str());

		return buff;
	}
	return "";
}
void get_gnu_host_name(std::string& name)
{
	char buff[0x100];
	gethostname(buff, sizeof(buff));
	name = buff;
}
std::string get_full_path_from_relative(const std::string& path)
{
	char resolved_path[PATH_MAX];
	auto result = realpath(path.c_str(), resolved_path);
	if(result==nullptr)
        return path;
    else
        return resolved_path;
}
rx_result build_directories(hosting::rx_host_directories& data)
{
	char buff[PATH_MAX];
	data.system_config = "/etc/rx-platform/config";
	data.system_storage = "/etc/rx-platform/storage/rx-system-storage";
	data.user_config = "~/.rx-platform/config";
	data.user_storage = "~/.rx-platform/storage/rx-system-storage";
	data.manuals = "/usr/share/man/rx-platform";
	if (getcwd(buff, sizeof(buff)) == NULL)
	{
		return rx_result::create_from_last_os_error("Error reading current directory!");
	}
	data.local_folder = buff;

	return true;
}
}


