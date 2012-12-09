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

#include "bnbt.h"
#include "client.h"
#include "config.h"
#include "server.h"
#include "tracker.h"
#include "util.h"

#ifdef WIN32
 #include "util_ntservice.h"
#endif

CServer :: CServer( )
{
	m_bKill = false;

	m_iSocketTimeOut = CFG_GetInt( "socket_timeout", 15 );
	m_strBind = CFG_GetString( "bind", string( ) );
	m_iCompression = CFG_GetInt( "bnbt_compression_level", 6 );

	// clamp

	if( m_iCompression > 9 )
		m_iCompression = 9;

	struct sockaddr_in sin;

	memset( &sin, 0, sizeof( sin ) );

	sin.sin_family = AF_INET;

	if( !m_strBind.empty( ) )
	{
		// bind to m_strBind

		if( gbDebug )
			UTIL_LogPrint( "server - binding to %s\n", m_strBind.c_str( ) );

		if( ( sin.sin_addr.s_addr = inet_addr( m_strBind.c_str( ) ) ) == INADDR_NONE )
		{
			UTIL_LogPrint( "server error - unable to bind to %s\n", m_strBind.c_str( ) );

			Kill( );
		}
	}
	else
	{
		// bind to all available addresses

		if( gbDebug )
			UTIL_LogPrint( "server - binding to all available addresses\n" );

		sin.sin_addr.s_addr = INADDR_ANY;
	}

	if( ( sin.sin_port = htons( (u_short)CFG_GetInt( "port", 6969 ) ) ) == 0 )
	{
		UTIL_LogPrint( "server error - invalid port %d\n", CFG_GetInt( "port", 6969 ) );

		Kill( );
	}

	// map protocol name to protocol number

	struct protoent *pPE;

	if( ( pPE = getprotobyname( "tcp" ) ) == 0 )
	{
		UTIL_LogPrint( "server error - unable to get tcp protocol entry (error %d)\n", GetLastError( ) );

		Kill( );
	}

	// allocate socket

	if( ( m_sckServer = socket( PF_INET, SOCK_STREAM, pPE->p_proto ) ) == INVALID_SOCKET )
	{
		UTIL_LogPrint( "server error - unable to allocate socket (error %d)\n", GetLastError( ) );

		Kill( );
	}

	// bind socket

	int optval = 1;

#ifdef WIN32
	setsockopt( m_sckServer, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof( int ) );
#else
	setsockopt( m_sckServer, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof( int ) );
#endif

	if( bind( m_sckServer, (struct sockaddr *)&sin, sizeof( sin ) ) == SOCKET_ERROR )
	{
		UTIL_LogPrint( "server error - unable to bind socket (error %d)\n", GetLastError( ) );

		Kill( );
	}

	// listen, queue length 8

	if( listen( m_sckServer, 8 ) == SOCKET_ERROR )
	{
		UTIL_LogPrint( "server error - unable to listen (error %d)\n", GetLastError( ) );

		Kill( );
	}

	m_pTracker = new CTracker( );

	UTIL_LogPrint( "server - start\n" );
}

CServer :: ~CServer( )
{
	closesocket( m_sckServer );

	unsigned long iStart = GetTime( );

	while( 1 )
	{
		for( vector<CClient *> :: iterator i = m_vecClients.begin( ); i != m_vecClients.end( ); )
		{
			if( (*i)->m_iState == CS_WAITING1 || (*i)->m_iState == CS_WAITING2 || (*i)->m_iState == CS_DEAD )
			{
				delete *i;

				i = m_vecClients.erase( i );
			}
			else
				i++;
		}

		if( m_vecClients.size( ) > 0 )
		{
			UTIL_LogPrint( "server - waiting for %d clients to disconnect\n", m_vecClients.size( ) );

			MILLISLEEP( 1000 );
		}
		else
			break;

		if( GetTime( ) - iStart > 60 )
		{
			UTIL_LogPrint( "server - waited 60 seconds, exiting anyway\n" );

			break;
		}
	}

	if( m_pTracker )
		delete m_pTracker;

	m_pTracker = NULL;

	UTIL_LogPrint( "server - exit\n" );
}

void CServer :: Kill( )
{
	m_bKill = true;
}

bool CServer :: isDying( )
{
	return m_bKill;
}

bool CServer :: Update( bool bBlock )
{
	if( m_vecClients.size( ) < (unsigned int)giMaxConns )
	{
		fd_set fdServer;

		FD_ZERO( &fdServer );
		FD_SET( m_sckServer, &fdServer );

		struct timeval tv;

		if( bBlock )
		{
			// block for 100 ms to keep from eating up all cpu time

			tv.tv_sec = 0;
			tv.tv_usec = 100000;
		}
		else
		{
			tv.tv_sec = 0;
			tv.tv_usec = 0;
		}

#ifdef WIN32
		if( select( 1, &fdServer, NULL, NULL, &tv ) == SOCKET_ERROR )
#else
		if( select( m_sckServer + 1, &fdServer, NULL, NULL, &tv ) == SOCKET_ERROR )
#endif
		{
			UTIL_LogPrint( "server warning - select error (error %d)\n", GetLastError( ) );

			FD_ZERO( &fdServer );

			MILLISLEEP( 100 );
		}

		if( FD_ISSET( m_sckServer, &fdServer ) )
		{
			struct sockaddr_in adrFrom;

			int iAddrLen = sizeof( adrFrom );

			SOCKET sckClient;

#ifdef WIN32
			if( ( sckClient = accept( m_sckServer, (struct sockaddr *)&adrFrom, &iAddrLen ) ) == INVALID_SOCKET )
#else
			if( ( sckClient = accept( m_sckServer, (struct sockaddr *)&adrFrom, (socklen_t *)&iAddrLen ) ) == INVALID_SOCKET )
#endif
				UTIL_LogPrint( "server warning - accept error (error %d)\n", GetLastError( ) );
			else
			{
				// reuse the old timeval structure just because

				tv.tv_sec = m_iSocketTimeOut;
				tv.tv_usec = 0;

				CClient *pClient = new CClient( sckClient, adrFrom, tv, m_iCompression );

#ifdef WIN32
				if( _beginthread( ( void (*)(void *) )StartReceiving, 0, (void *)pClient ) == -1 )
				{
					UTIL_LogPrint( "server warning - unable to spawn receiver thread\n" );

					pClient->m_iState = CS_DEAD;
				}
#else
				pthread_t thread;

				// set detached state since we don't need to join with any threads

				pthread_attr_t attr;
				pthread_attr_init( &attr );
				pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );

				int c = pthread_create( &thread, &attr, ( void * (*)(void *) )StartReceiving, (void *)pClient );

				if( c != 0 )
				{
					UTIL_LogPrint( "server warning - unable to spawn receiver thread (error %s)\n", strerror( c ) );

					pClient->m_iState = CS_DEAD;
				}
#endif

				m_vecClients.push_back( pClient );
			}
		}
	}
	else
	{
		// maximum connections reached

		MILLISLEEP( 100 );
	}

	// process

	for( vector<CClient *> :: iterator i = m_vecClients.begin( ); i != m_vecClients.end( ); )
	{
		if( (*i)->m_iState == CS_WAITING1 )
		{
			(*i)->Process( );

			i++;
		}
		else if( (*i)->m_iState == CS_WAITING2 )
		{
			(*i)->m_iState = CS_SENDING;

#ifdef WIN32
			if( _beginthread( ( void (*)(void *) )StartSending, 0, (void *)(*i) ) == -1 )
			{
				UTIL_LogPrint( "server warning - unable to spawn sender thread\n" );

				(*i)->m_iState = CS_DEAD;
			}
#else
			pthread_t thread;

			// set detached state since we don't need to join with any threads

			pthread_attr_t attr;
			pthread_attr_init( &attr );
			pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );

			int c = pthread_create( &thread, &attr, ( void * (*)(void *) )StartSending, (void *)(*i) );

			if( c != 0 )
			{
				UTIL_LogPrint( "server warning - unable to spawn sender thread (error %s)\n", strerror( c ) );

				(*i)->m_iState = CS_DEAD;
			}
#endif

			i++;
		}
		else if( (*i)->m_iState == CS_DEAD )
		{
			delete *i;

			i = m_vecClients.erase( i );
		}
		else
			i++;
	}

	if( m_pTracker )
		m_pTracker->Update( );

	return m_bKill;
}

CTracker *CServer :: getTracker( )
{
	return m_pTracker;
}
