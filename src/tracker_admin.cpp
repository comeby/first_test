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
#include "link.h"
#include "tracker.h"
#include "server.h"
#include "util.h"

void CTracker :: serverResponseAdmin( struct request_t *pRequest, struct response_t *pResponse, user_t user )
{
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

	// The Trinity Edition - MOD BEGINS
	// Uses a Custom Tracker Title in place of BNBT - bnbt_tracker_title
	if ( !gstrTrackerTitle.empty( ) ) {
		pResponse->strContent += "<title>" + gstrTrackerTitle + " Admin Panel</title>\n";
	}
	else {	
		pResponse->strContent += "<title>BNBT Admin Panel</title>\n"; // <-- Original Source Code
	}

	if( !gstrStyle.empty( ) )
		pResponse->strContent += "<link rel=stylesheet type=\"text/css\" href=\"" + gstrStyle + "\">\n";

	// The Trinity Edition - SHUTDOWN SCRIPT

	pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
	pResponse->strContent += " function shutdown() {\n";
	pResponse->strContent += "   if ( confirm ( \"Are you sure you want to shutdown the tracker?\") ) {\n";
	pResponse->strContent += "        alert('After closing all active connections, your tracker will shutdown. \\n' + 'Your connection will now be dropped, and no response will be sent to your browser.');\n";
	pResponse->strContent += "        window.location = \"/admin.html?ap_kill=1\";\n";
	pResponse->strContent += "   }\n";
	pResponse->strContent += " }\n";
	pResponse->strContent += "</script>\n\n";

	// ------------------------------------ End of Addition

	pResponse->strContent += "</head>\n";
	pResponse->strContent += "<body>\n";

	// The Trinity Edition - NAVBAR

if( m_bUseNavbar ) {

	pResponse->strContent += "<table class=\"navbar\" width=\"100%\"><tr class=\"navbar\"><td class=\"navbar\">\n\n";

	if( user.iAccess & ACCESS_ADMIN ) {

		pResponse->strContent += "<input class=\"rtt\" type=\"button\" value=\"Return To Tracker\" onClick=\"window.location=\'/index.html\'\">\n";
		pResponse->strContent += "| <input class=\"index_info\" type=\"button\" value=\"Tracker Info\" onClick=\"window.location=\'/info.html\'\">\n";
		pResponse->strContent += "| <input class=\"admin_users\" type=\"button\" value=\"Manage Users\" onClick=\"window.location=\'/users.html\'\">\n";

		if( m_bCountUniquePeers ) {
			pResponse->strContent += "<input class=\"admin_cup\" type=\"button\" value=\"Count Unique Peers\" onClick=\"window.location=\'/admin.html?ap_recount=1\'\">\n";
		}
		else {
			pResponse->strContent += "<input disabled class=\"admin_cup\" type=\"button\" value=\"Count Unique Peers\">\n";
		}

		pResponse->strContent += "<input class=\"admin_shutdown\" type=\"button\" value=\"Shutdown Tracker\" onClick=\"shutdown()\">\n";
		pResponse->strContent += "| <input class=\"back\" type=\"button\" value=\"&laquo; Back\" onClick=\"window.history.back()\">\n\n";

	}

	else { // Disable ALL buttons except for RTT and back if user is not Admin

		pResponse->strContent += "<input class=\"rtt\" type=\"button\" value=\"Return To Tracker\" onClick=\"window.location=\'/index.html\'\">\n";
		pResponse->strContent += "| <input disabled class=\"index_info\" type=\"button\" value=\"Tracker Info\" onClick=\"window.location=\'/info.html\'\">\n";
		pResponse->strContent += "| <input disabled class=\"admin_users\" type=\"button\" value=\"Manage Users\">\n";
		pResponse->strContent += "<input disabled class=\"admin_cup\" type=\"button\" value=\"Count Unique Peers\">\n";
		pResponse->strContent += "<input disabled class=\"admin_shutdown\" type=\"button\" value=\"Shutdown Tracker\">\n";
		pResponse->strContent += "| <input class=\"back\" type=\"button\" value=\"&laquo; Back\" onClick=\"window.history.back()\">\n\n";

	}

	pResponse->strContent += "</td></tr></table>\n\n";
}

	// -------------------------------- END OF ADDITION

// The Trinity Edition - Modification Begins
// Removes Login and Logged In Message when using the NAVBAR

if( !m_bUseNavbar ) {

	if( user.strLogin.empty( ) )
		pResponse->strContent += "<p class=\"login1_admin\">You are not logged in. Click <a href=\"/login.html\">here</a> to login.</p>\n";
	else
		pResponse->strContent += "<p class=\"login2_admin\">You are logged in as <span class=\"username\">" + UTIL_RemoveHTML( user.strLogin ) + "</span>. Click <a href=\"/login.html?logout=1\">here</a> to logout.</p>\n";

}

	/* Original Source Code:
	if( user.strLogin.empty( ) )
		pResponse->strContent += "<p class=\"login1_admin\">You are not logged in. Click <a href=\"/login.html\">here</a> to login.</p>\n";
	else
		pResponse->strContent += "<p class=\"login2_admin\">You are logged in as <span class=\"username\">" + UTIL_RemoveHTML( user.strLogin ) + "</span>. Click <a href=\"/login.html?logout=1\">here</a> to logout.</p>\n";
	*/

// ------------------------------------------------- END OF MODIFICATION

	// The Trinity Edition - MOD BEGINS
	// Uses a Custom Tracker Title in place of BNBT - bnbt_tracker_title
	if ( !gstrTrackerTitle.empty( ) ) {
		pResponse->strContent += "<h3>" + gstrTrackerTitle + " Admin Panel</h3>\n";
	}
	else {
		pResponse->strContent += "<h3>BNBT Admin Panel</h3>\n"; // <-- Original Source Code
	}

	if( user.iAccess & ACCESS_ADMIN )
	{
		//
		// kill tracker
		//

		if( pRequest->mapParams["ap_kill"] == "1" )
		{
			gpServer->Kill( );

			return;
		}

		if( pRequest->mapParams["ap_recount"] == "1" )
		{
			gpServer->getTracker( )->CountUniquePeers( );

			pResponse->strContent += "<p>Counting unique peers. Click <a href=\"/admin.html\">here</a> to return to the admin page.</p>\n";

			// The Trinity Edition - ADD BEGINS - Displays JavaScript alert and returns user to Admin Panel
			
			pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
			pResponse->strContent += "  alert(\"Counting Unique Peers... \\n\\n Press OK to return to the Admin CP \\n\\n\");\n";
			pResponse->strContent += "  window.location = \"/admin.html\";\n";
			pResponse->strContent += "</script>\n\n";
			
			// --------------------------------------------------- END OF ADD

			pResponse->strContent += "</body>\n";
			pResponse->strContent += "</html>\n";

			return;
		}

		//
		// reset tracker link
		//

		if( pRequest->mapParams["ap_relink"] == "1" )
		{
			if( gpLink )
			{
				gpLink->Kill( );

				pResponse->strContent += "<p>Resetting tracker link. Click <a href=\"/admin.html\">here</a> to return to the admin page.</p>\n";
			}
			else
				pResponse->strContent += "<p>This tracker does not own a tracker link (it might be a tracker hub). Click <a href=\"/admin.html\">here</a> to return to the admin page.</p>\n";

			pResponse->strContent += "</body>\n";
			pResponse->strContent += "</html>\n";

			return;
		}

		//
		// clients
		//

		pResponse->strContent += "<p>Currently serving ";
		pResponse->strContent += CAtomInt( gpServer->m_vecClients.size( ) ).toString( );
		pResponse->strContent += " clients (including you)!</p>\n";

		//
		// tracker links
		//

		pResponse->strContent += "<table summary=\"tlink\">\n";
		pResponse->strContent += "<tr><th colspan=2>Tracker Links</th></tr>\n";
		pResponse->strContent += "<tr><td>Type</td>";

		if( gpLinkServer )
		{
			pResponse->strContent += "<td>Primary Tracker</td></tr>\n";
			pResponse->strContent += "<tr><td>Connections</td><td>";

			gpLinkServer->m_mtxLinks.Claim( );

			for( vector<CLinkClient *> :: iterator i = gpLinkServer->m_vecLinks.begin( ); i != gpLinkServer->m_vecLinks.end( ); i++ )
			{
				pResponse->strContent += (*i)->getName( );

				if( (*i)->m_bActive )
					pResponse->strContent += " (ACTIVE)";
				else
					pResponse->strContent += " (NOT ACTIVE)";

				if( i + 1 != gpLinkServer->m_vecLinks.end( ) )
					pResponse->strContent += "<br>";
			}

			gpLinkServer->m_mtxLinks.Release( );

			pResponse->strContent += "</td></tr>\n";
		}
		else if( gpLink )
		{
			pResponse->strContent += "<td>Secondary Tracker</td></tr>\n";
			pResponse->strContent += "<tr><td>Connection</td><td>" + gpLink->getName( ) + "</td></tr>\n";
			pResponse->strContent += "<tr><td colspan=2><a href=\"/admin.html?ap_relink=1\">Reset Tracker Link</a>\n";
		}
		else
			pResponse->strContent += "<td>No Link</td></tr>\n";

		pResponse->strContent += "</table>\n";

		//
		// kill tracker
		//

		// The Trinity Edition - MOD BEGINS
		// Kill Tracker link uses javascript instead...

		pResponse->strContent += "<p><a href=\"javascript:void shutdown()\">Kill Tracker</a></p>\n";

		/* Original Source Code:
		pResponse->strContent += "<p><a href=\"/admin.html?ap_kill=1\">Kill Tracker</a></p>\n";
		pResponse->strContent += "<p>If you kill the tracker your connection will be dropped and no response will be sent to your browser.</p>\n";
		*/

		//
		// count unique peers
		//

		// The Trinity Edition - Modification Begins
		// Only shows the Count Unique Peers link when bnbt_count_unique_peers is set to a non-zero value

		if( m_bCountUniquePeers ) {
			pResponse->strContent += "<p><a href=\"/admin.html?ap_recount=1\">Count Unique Peers</a></p>\n";
		}
		else {
			pResponse->strContent += "<p>Count Unique Peers &mdash; DISABLED</p>\n";
		}

		/* Original Source Code:
		pResponse->strContent += "<p><a href=\"/admin.html?ap_recount=1\">Count Unique Peers</a></p>\n";
		*/
	}
	else
		pResponse->strContent += "<p class=\"denied\">You are not authorized to view this page.</p>\n";

	pResponse->strContent += "</body>\n";
	pResponse->strContent += "</html>\n";
}
