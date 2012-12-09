/***
*
* BNBT Beta 7.7 - A C++ BitTorrent Tracker
* Copyright (C) 2003 Trevor Hogan
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
***/

// Trinity Edition Modified Source File

#include <signal.h>

#include "bnbt.h"
#include "config.h"
#include "link.h"
#include "server.h"

#ifdef WIN32
 #include "util_ntservice.h"
#endif

#include "util.h"

#ifndef WIN32
 int GetLastError( ) { return errno; }
#endif

time_t giStartTime;

unsigned long GetTime( )
{
	return (unsigned long)( time( NULL ) - giStartTime );
}

CServer *gpServer;
CLink *gpLink;
CLinkServer *gpLinkServer;
CMutex gmtxOutput;
string gstrErrorLogDir;
string gstrErrorLogFile;
FILE *gpErrorLog;
string gstrAccessLogDir;
string gstrAccessLogFile;
FILE *gpAccessLog;
unsigned long giErrorLogCount;
unsigned long giAccessLogCount;
int giFlushInterval;
bool gbDebug;
int giMaxConns;
string gstrStyle;
string gstrCharSet;
string gstrRealm;
// The Trinity Edition - Addition Begins
// declares variable for Custom NT Service Name
string gstrNTServiceName;
// declares variable for Alternative Signup URL
string gstrAltSignupURL;
// declares variable for Tracker Title
string gstrTrackerTitle;
// declares variable for External Javascript File
string gstrExternalJavascript;
// ---------------------------------- End of Addition

void sigCatcher( int sig )
{
	signal( SIGABRT, sigCatcher );
	signal( SIGINT, sigCatcher );

	if( gpServer )
	{
		if( gpServer->isDying( ) )
			exit( 1 );
		else
			gpServer->Kill( );
	}
	else
		exit( 1 );
}

int main( int argc, char *argv[] )
{
#ifdef WIN32

	// parse arguments

	if( argv[0] )
	{
		char *szEndPos = strrchr( argv[0], '\\' );

		if( szEndPos )
		{
			char *szEXEPath = new char[szEndPos - argv[0] + 1];

			memcpy( szEXEPath, argv[0], szEndPos - argv[0] );

			szEXEPath[szEndPos - argv[0]] = '\0';

			SetCurrentDirectory( szEXEPath );

			delete [] szEXEPath;
		}
	}

	if( argc > 1 )
	{
		// The Trinity Edition - Addition Begins
		// Added for Custom NT Service Name Code
		CFG_Open( CFG_FILE );
		#define BNBT_SERVICE_NAME const_cast<LPSTR> (CFG_GetString( "trinity_nt_service_name", "BNBT Service" ).c_str())
		CFG_Close( CFG_FILE );

		printf( "Service name ");
		printf( BNBT_SERVICE_NAME );
		printf( "\n" ); 
		// ------------------------------------------ End of Addition

		if( _stricmp( argv[1], "-i" ) == 0 )
		{
			// install service

			if( UTIL_NTServiceTest( ) )
				printf( "BNBT Service is already installed!\n" );
			else
			{
				if( UTIL_NTServiceInstall( ) )
					printf( "BNBT Service installed.\n" );
				else
					printf( "BNBT Service failed to install (error %d).\n", GetLastError( ) );
			}

			return 0;
		}
		else if( _stricmp( argv[1], "-u" ) == 0 )
		{
			// uninstall service

			if( !UTIL_NTServiceTest( ) )
				printf( "BNBT Service is not installed!\n" );
			else
			{
				if( UTIL_NTServiceUninstall( ) )
					printf( "BNBT Service uninstalled.\n" );
				else
					printf( "BNBT Service failed to uninstall (error %d).\n", GetLastError( ) );
			}

			return 0;
		}
		else if( _stricmp( argv[1], "-start" ) == 0 )
		{
			// start

			if( !UTIL_NTServiceTest( ) )
				printf( "BNBT Service is not installed!\n" );
			else
			{
				printf( "Starting BNBT Service.\n" );

				if( !UTIL_NTServiceStart( ) )
					printf( "BNBT Service failed to start (error %d).\n", GetLastError( ) );
			}

			return 0;
		}
		else if( _stricmp( argv[1], "-stop" ) == 0 )
		{
			// stop

			if( !UTIL_NTServiceTest( ) )
				printf( "BNBT Service is not installed!\n" );
			else
			{
				printf( "Stopping BNBT Service.\n" );

				if( !UTIL_NTServiceStop( ) )
					printf( "BNBT Service failed to stop (error %d).\n", GetLastError( ) );
			}

			return 0;
		}
		else if( _stricmp( argv[1], "-s" ) == 0 )
		{
			// internal service start

			SERVICE_TABLE_ENTRY st[] = {
				{ BNBT_SERVICE_NAME, NTServiceMain },
				{ NULL, NULL }
			};

			// ask the Service Control Manager to spawn a thread and call NTServiceMain for us, then block

			StartServiceCtrlDispatcher( st );

			return 0;
		}
	}

#else
	// disable SIGPIPE since some systems like OS X don't define MSG_NOSIGNAL

	signal( SIGPIPE, SIG_IGN );
#endif

	// catch SIGABRT and SIGINT

	signal( SIGABRT, sigCatcher );
	signal( SIGINT, sigCatcher );

	return bnbtmain( );
}

int bnbtmain( )
{
	gmtxOutput.Initialize( );

	srand( time( NULL ) );

	giStartTime = time( NULL );

	CFG_Open( CFG_FILE );
	CFG_SetDefaults( );
	CFG_Close( CFG_FILE );

	gstrErrorLogDir = CFG_GetString( "bnbt_error_log_dir", string( ) );

	if( !gstrErrorLogDir.empty( ) && gstrErrorLogDir[gstrErrorLogDir.size( ) - 1] != PATH_SEP )
		gstrErrorLogDir += PATH_SEP;

	gpErrorLog = NULL;
	gstrAccessLogDir = CFG_GetString( "bnbt_access_log_dir", string( ) );

	if( !gstrAccessLogDir.empty( ) && gstrAccessLogDir[gstrAccessLogDir.size( ) - 1] != PATH_SEP )
		gstrAccessLogDir += PATH_SEP;

	gpAccessLog = NULL;
	giErrorLogCount = 0;
	giAccessLogCount = 0;
	giFlushInterval = CFG_GetInt( "bnbt_flush_interval", 100 );
	gbDebug = CFG_GetInt( "bnbt_debug", 1 ) == 0 ? false : true;
	giMaxConns = CFG_GetInt( "bnbt_max_conns", 64 );
	gstrStyle = CFG_GetString( "bnbt_style_sheet", string( ) );
	gstrCharSet = CFG_GetString( "bnbt_charset", "iso-8859-1" );
	gstrRealm = CFG_GetString( "bnbt_realm", "BNBT" );
	// The Trinity Edition - Addition Begins
	// Sets the value for the Custom NT Service Name variable
	gstrNTServiceName = CFG_GetString( "trinity_nt_service_name", "BNBT Service" );
	// Sets the "bnbt_alt_signup_url" value
	gstrAltSignupURL = CFG_GetString( "bnbt_alt_signup_url", string( ) );
	// Sets the "bnbt_tracker_title" value
	gstrTrackerTitle = CFG_GetString( "bnbt_tracker_title", string( ) );
	// Sets the "bnbt_external_js" value
	gstrExternalJavascript = CFG_GetString( "bnbt_external_js", string( ) );
	// --------------------------------------- End of Addition

	// start winsock

#ifdef WIN32
	WSADATA wsadata;

	if( WSAStartup( MAKEWORD( 2, 0 ), &wsadata ) != 0 )
	{
		UTIL_LogPrint( "error - unable to start winsock (error %d)\n", GetLastError( ) );

		if( gpAccessLog )
			fclose( gpAccessLog );

		if( gpErrorLog )
			fclose( gpErrorLog );

		gmtxOutput.Destroy( );

		return 1;
	}
#endif

	gpServer = new CServer( );
	gpLink = NULL;
	gpLinkServer = NULL;

	if( CFG_GetInt( "bnbt_tlink_server", 0 ) != 0 )
		gpLinkServer = new CLinkServer( );
	else
	{
		if( !CFG_GetString( "bnbt_tlink_connect", string( ) ).empty( ) )
		{
#ifdef WIN32
			if( _beginthread( ( void (*)(void *) )StartLink, 0, NULL ) == -1 )
				UTIL_LogPrint( "error - unable to spawn link thread\n" );
#else
			pthread_t thread;

			// set detached state since we don't need to join with any threads

			pthread_attr_t attr;
			pthread_attr_init( &attr );
			pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );

			int c = pthread_create( &thread, &attr, ( void * (*)(void *) )StartLink, NULL );

			if( c != 0 )
				UTIL_LogPrint( "error - unable to spawn link thread (error %s)\n", strerror( c ) );
#endif
		}
	}

	while( 1 )
	{
		if( gpServer->Update( true ) )
		{
			delete gpServer;

			gpServer = NULL;

			if( gpLinkServer )
			{
				delete gpLinkServer;

				gpLinkServer = NULL;
			}

			break;
		}

		if( gpLinkServer )
			gpLinkServer->Update( );
	}

	// wait for the link or it might make a big mess

	if( gpLink )
		gpLink->Kill( );

	unsigned long iStart = GetTime( );

	while( gpLink )
	{
		UTIL_LogPrint( "server - waiting for link to disconnect\n" );

		MILLISLEEP( 1000 );

		if( GetTime( ) - iStart > 60 )
		{
			UTIL_LogPrint( "server - waited 60 seconds, exiting anyway\n" );

			break;
		}
	}

	if( gpAccessLog )
		fclose( gpAccessLog );

	if( gpErrorLog )
		fclose( gpErrorLog );

	gmtxOutput.Destroy( );

#ifdef WIN32
	WSACleanup( );
#endif

	return 0;
}
