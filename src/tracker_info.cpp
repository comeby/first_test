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

#include "bnbt.h"
#include "atom.h"
#include "tracker.h"
#include "util.h"

void CTracker :: serverResponseInfo( struct request_t *pRequest, struct response_t *pResponse, user_t user )
{
	struct bnbttv btv = UTIL_CurrentTime( );

	pResponse->strCode = "200 OK";

	pResponse->mapHeaders.insert( pair<string, string>( "Content-Type", string( "text/html; charset=" ) + gstrCharSet ) );

	pResponse->strContent += "<html>\n";
	pResponse->strContent += "<head>\n";
	
	// The Trinity Edition - Addition BEGINS
	// Inserts an HTML <script> tag to reference an external Javascript (.js) document when an ONLINE file path is specified in the tracker's configuration file...

	if ( !gstrExternalJavascript.empty( ) ) {
            pResponse->strContent += "<script src=\"" + gstrExternalJavascript + "\" language=\"JavaScript\" type=\"text/javascript\"></script>\n\n";
        }

        // ---------------------------------------- End of Addition

	// Uses a Custom Tracker Title in place of BNBT - bnbt_tracker_title
	if ( !gstrTrackerTitle.empty( ) ) {
		pResponse->strContent += "<title>" + gstrTrackerTitle + " &mdash; Tracker Info</title>\n";
	}
	else {
		pResponse->strContent += "<title>BNBT Tracker Info</title>\n"; // <-- Original Source Code
	}

	if( !gstrStyle.empty( ) )
		pResponse->strContent += "<link rel=stylesheet type=\"text/css\" href=\"" + gstrStyle + "\">\n";

	pResponse->strContent += "</head>\n";
	pResponse->strContent += "<body>\n";

	// The Trinity Edition - RTT Button

if( m_bUseNavbar ) {

	pResponse->strContent += "<table class=\"navbar\" width=\"100%\"><tr class=\"navbar\"><td class=\"navbar\">\n\n";

	pResponse->strContent += "<input class=\"rtt\" type=\"button\" value=\"Return To Tracker\" onClick=\"window.location=\'/index.html\'\">\n\n";

	pResponse->strContent += "</td></tr></table>\n\n";

}

	// -------------------------------- END OF ADDITION

// The Trinity Edition - Modification Begins
// Removes Login and Logged In Message when using the NAVBAR

if( !m_bUseNavbar ) {

	if( user.strLogin.empty( ) )
		pResponse->strContent += "<p class=\"login1_info\">You are not logged in. Click <a href=\"/login.html\">here</a> to login.</p>\n";
	else
		pResponse->strContent += "<p class=\"login2_info\">You are logged in as <span class=\"username\">" + UTIL_RemoveHTML( user.strLogin ) + "</span>. Click <a href=\"/login.html?logout=1\">here</a> to logout.</p>\n";

}

	/* Original Source Code:
	if( user.strLogin.empty( ) )
		pResponse->strContent += "<p class=\"login1_info\">You are not logged in. Click <a href=\"/login.html\">here</a> to login.</p>\n";
	else
		pResponse->strContent += "<p class=\"login2_info\">You are logged in as <span class=\"username\">" + UTIL_RemoveHTML( user.strLogin ) + "</span>. Click <a href=\"/login.html?logout=1\">here</a> to logout.</p>\n";
	*/

// ------------------------------------------------- END OF MODIFICATION

	// Uses a Custom Tracker Title in place of BNBT - bnbt_tracker_title
	if ( !gstrTrackerTitle.empty( ) ) {
		pResponse->strContent += "<h3>" + gstrTrackerTitle + " &mdash; Tracker Info</h3>\n";
	}
	else {
		pResponse->strContent += "<h3>BNBT Tracker Info</h3>\n"; // <-- Original Source Code
	}

	if( user.iAccess & ACCESS_VIEW )
	{
		time_t tNow = time( NULL );
		char *szTime = asctime( localtime( &tNow ) );
		szTime[strlen( szTime ) - 1] = '\0';

		pResponse->strContent += "<ul>\n";
		// Trinity Edition Modification Begins
		// Adds extra Version info to the standard string
		pResponse->strContent += "<li><strong>Tracker Version:</strong> BNBT TrinEdit" + string( BNBT_VER ) + "</li>\n";
		pResponse->strContent += "<li><strong>Tracker Build:</strong> 7.7r3.2004.10.27</li>\n";
		/* Original Source Code:
		pResponse->strContent += "<li><strong>Tracker Version:</strong> BNBT " + string( BNBT_VER ) + "</li>\n";
		*/
		// ------------------------- END OF MOD
		pResponse->strContent += "<li><strong>Server Time:</strong> " + string( szTime ) + "</li>\n";
		pResponse->strContent += "<li><strong>Uptime:</strong> " + UTIL_SecondsToString( GetTime( ) ) + "</li>\n";

		if( m_pDFile )
		{
			pResponse->strContent += "<li><strong>Tracking " + CAtomLong( m_pDFile->getValue( ).size( ) ).toString( ) + " Files, ";

			unsigned long iPeers = 0;

			map<string, CAtom *> mapDicti = m_pDFile->getValue( );

			for( map<string, CAtom *> :: iterator i = mapDicti.begin( ); i != mapDicti.end( ); i++ )
			{
				if( (*i).second->isDicti( ) )
					iPeers += ( (CAtomDicti *)(*i).second )->getValue( ).size( );
			}

			pResponse->strContent += CAtomLong( iPeers ).toString( ) + " Peers";

			if( m_bCountUniquePeers )
				pResponse->strContent += ", " + CAtomLong( m_pIPs->getValue( ).size( ) ).toString( ) + " Unique";

			pResponse->strContent += "</strong></li>\n";
		}

		if( m_pUsers )
			pResponse->strContent += "<li><strong>" + CAtomLong( m_pUsers->getValue( ).size( ) ).toString( ) + " Users</strong></li>\n";

		pResponse->strContent += "</ul>\n";
	}
	else
		pResponse->strContent += "<p class=\"denied\">You are not authorized to view this page.</p>\n";

	if( m_bGen )
		pResponse->strContent += "<p class=\"gen\">Generated in " + UTIL_ElapsedTimeStr( btv, UTIL_CurrentTime( ) ) + " seconds.</p>\n";

	pResponse->strContent += "</body>\n";
	pResponse->strContent += "</html>\n";
}
