/*
    Rose Online Server Emulator
    Copyright (C) 2006,2007 OSRose Team http://www.dev-osrose.com

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

    depeloped with Main erose/hrose source server + some change from the original eich source
*/
#ifndef __ROSE_LOGGING_FUNCTIONS__
#define __ROSE_LOGGING_FUNCTIONS__

// Log path
//LMA: we add another dir for packets logs.
#ifdef _WIN32
#	define LOG_DIRECTORY "log/"
#	define PLOG_DIRECTORY "plog/"
#endif

#ifdef __unix__
#	define LOG_DIRECTORY "/var/log/roseserver/"
#	define PLOG_DIRECTORY "/var/log/roseserver/p/"
#endif

#define LOG_LOGIN_SERVER		0x01
#define LOG_CHARACTER_SERVER	0x02
#define LOG_WORLD_SERVER     	0x03
#define LOG_SAME_FILE           0x04

// Log File names
extern unsigned char LOG_THISSERVER;
extern bool PRINT_LOG;
#define LOG_FILENAME_LOGINFILE  "loginserver.log"
#define LOG_FILENAME_CHARFILE   "charserver.log"
#define LOG_FILENAME_WORLDFILE  "worldserver.log"
#define LOG_DEFAULT_FILE        "server.log"
#define LOG_FILENAME_ERRORFILE  "errors.log"

#define LOG_LOGINPACKETS "loginpacket.log"
#define LOG_CHARPACKETS  "charpackets.log"
#define LOG_WORLDPACKETS "worldpackets.log"
#define LOG_DEFAULTPACKETS "packets.log"

// Error Types
enum msg_type {
	MSG_NONE,
	MSG_STATUS,
	MSG_SQL,
	MSG_INFO,
	MSG_NOTICE,
	MSG_WARNING,
	MSG_DEBUG,
	MSG_ERROR,
	MSG_FATALERROR,
	MSG_HACK,
	MSG_LOAD,
	MSG_SDEBUG,
	MSG_GMACTION,
	MSG_START,
	MSG_QUERY,
	MSG_CONSOLE
};

// Logging function
void Log( msg_type flag, char* Format, ... );


extern FILE *fhSp1;
void LogSp( enum msg_type flag, char *Format, ... );
void LogHandleSp(int type);
void LogDebugPriority(int priority=3, bool warning=false);
void LogDebug(char *Format, ... );


#endif
