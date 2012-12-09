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

void CTracker :: serverResponseLogin( struct request_t *pRequest, struct response_t *pResponse, user_t user )
{
	struct bnbttv btv = UTIL_CurrentTime( );

	if( user.strLogin.empty( ) )
	{
		pResponse->strCode = "401 Unauthorized";

		pResponse->mapHeaders.insert( pair<string, string>( "Content-Type", "text/plain" ) );
		pResponse->mapHeaders.insert( pair<string, string>( "WWW-Authenticate", string( "Basic realm=\"" ) + gstrRealm + "\"" ) );

		pResponse->strContent += "401 Unauthorized";

		// The Trinity Edition - Addition Begins
		// The following resolves ZLib Errors
		// Thank you DreadWingKnight
		
		pResponse->bCompressOK = false;

		// --------------------------------------- END OF ADDITION
		return;
	}

	pResponse->strCode = "200 OK";

	// cookies

	time_t tNow = time( NULL );

	struct tm tmFuture = *gmtime( &tNow );
	tmFuture.tm_mon++;
	mktime( &tmFuture );
	struct tm tmPast = *gmtime( &tNow );
	tmPast.tm_mon--;
	mktime( &tmPast );

	char pTime[256];
	memset( pTime, 0, sizeof( char ) * 256 );

	string strLogout = pRequest->mapParams["logout"];

	if( strLogout == "1" )
		strftime( pTime, sizeof( char ) * 256, "%a, %d-%b-%Y %H:%M:%S GMT", &tmPast );
	else
		strftime( pTime, sizeof( char ) * 256, "%a, %d-%b-%Y %H:%M:%S GMT", &tmFuture );

	pResponse->mapHeaders.insert( pair<string, string>( "Content-Type", string( "text/html; charset=" ) + gstrCharSet ) );
	pResponse->mapHeaders.insert( pair<string, string>( "Pragma", "No-Cache" ) );
	pResponse->mapHeaders.insert( pair<string, string>( "Set-Cookie", "login=\"" + user.strLogin + "\"; expires=" + pTime + "; path=/" ) );
	pResponse->mapHeaders.insert( pair<string, string>( "Set-Cookie", "md5=\"" + UTIL_StringToEscaped( user.strMD5 ) + "\"; expires=" + pTime + "; path=/" ) );

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
		pResponse->strContent += "<title>" + gstrTrackerTitle + " Login</title>\n";
	}
	else {
		pResponse->strContent += "<title>BNBT Login</title>\n"; // <-- Original Source Code
	}

	if( !gstrStyle.empty( ) )
		pResponse->strContent += "<link rel=stylesheet type=\"text/css\" href=\"" + gstrStyle + "\">\n";

	pResponse->strContent += "</head>\n";
	pResponse->strContent += "<body>\n";

	// The Trinity Edition - RTT and Logout buttons

if( m_bUseNavbar ) {

	pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
	pResponse->strContent += "  function logout() {\n";
	pResponse->strContent += "    if ( confirm ('PLEASE CONFIRM! \\n\\n Are you sure you want to logout? \\n\\n') ) {\n";
	pResponse->strContent += "         document.location = \"/login.html?logout=1\";\n";
	pResponse->strContent += "    }\n";
	pResponse->strContent += "  }\n";
	pResponse->strContent += "</script>\n\n";

	pResponse->strContent += "<table class=\"navbar\" width=\"100%\"><tr class=\"navbar\"><td class=\"navbar\">\n\n";

	pResponse->strContent += "<input class=\"rtt\" type=\"button\" value=\"Return To Tracker\" onClick=\"window.location=\'/index.html\'\">\n";	
	pResponse->strContent += "<input class=\"logout\" type=\"button\" value=\"Logout &raquo; " + UTIL_RemoveHTML( user.strLogin ) + "\" onClick=\"logout()\">\n";

	if( user.iAccess & ACCESS_UPLOAD ) { // Upload Torrent File button - ONLY ACTIVE IF USER HAS UPLOAD ACCESS
		pResponse->strContent += "<input class=\"upload\" type=\"button\" value=\"Upload Torrent File\" onClick=\"window.location=\'/upload.html\'\">\n";
	}
	else {
		pResponse->strContent += "<input disabled class=\"upload\" type=\"button\" value=\"Upload Torrent File\">\n";
	}

	if( user.iAccess & ACCESS_ADMIN ) { // Manage Users and Admin CP buttons - ONLY ACTIVE IF USER IS ADMIN
		pResponse->strContent += "| <input class=\"users\" type=\"button\" value=\"Manage Users\" onClick=\"window.location=\'/users.html\'\">\n";
		pResponse->strContent += "<input class=\"admin\" type=\"button\" value=\"Admin CP\" onClick=\"window.location=\'/admin.html\'\">\n";
	}
	else {
		pResponse->strContent += "| <input disabled class=\"users\" type=\"button\" value=\"Manage Users\">\n";
		pResponse->strContent += "<input disabled class=\"admin\" type=\"button\" value=\"Admin CP\">\n";
	}

	pResponse->strContent += "</td></tr></table>\n\n";

}

	// -------------------------------- END OF ADDITION

	// Uses a Custom Tracker Title in place of BNBT - bnbt_tracker_title
	// Prints "My Torrents" instead of "Login" when user has upload rights

	if ( !gstrTrackerTitle.empty( ) ) {
		if( user.iAccess & ACCESS_UPLOAD ) {
			pResponse->strContent += "<h3>" + gstrTrackerTitle + " - " + UTIL_RemoveHTML( user.strLogin ) + "'s torrents</h3>\n";
		}
		else {
			pResponse->strContent += "<h3>" + gstrTrackerTitle + " Login - " + UTIL_RemoveHTML( user.strLogin ) + "</h3>\n";
		}
	}
	else {
		if( user.iAccess & ACCESS_UPLOAD ) {
			pResponse->strContent += "<h3>BNBT - " + UTIL_RemoveHTML( user.strLogin ) + "'s torrents</h3>\n";
		}
		else {
			pResponse->strContent += "<h3>BNBT Login - " + UTIL_RemoveHTML( user.strLogin ) + "</h3>\n";
		}
	}

	/* Original Source Code:
	pResponse->strContent += "<h3>BNBT Login - " + UTIL_RemoveHTML( user.strLogin ) + "</h3>\n";
	*/

	// The Trinity Edition - Modification Begins
	// The following will display a JavaScript Alert that will
	// notify the user that he/she has been logged out and give
	// him/her the option to automatically close his/her browser.

	/* Original Source Code:
	if( strLogout == "1" )
		pResponse->strContent += "<p>Logging out... You may need to close your browser window to completely logout.</p>\n";
	*/

	if( strLogout == "1" ) {
		pResponse->strContent += "<p>Logging out... You may need to close your browser window to completely logout.</p>\n";
		pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
		pResponse->strContent += "if ( confirm ('You have been logged out successfully. It will most likely be necessary for you to close your browser window to completely logout. \\n\\n' + 'Click OK to close Internet Explorer ONLY.') ) {\n";
		pResponse->strContent += "  window.self.close();\n";
		pResponse->strContent += "}\n";
		pResponse->strContent += "</script>\n\n";
	}

	// -------------------------------------------- END OF MODIFICATION


	else
	{
		pResponse->strContent += "<ul>\n";
		pResponse->strContent += "<li>You signed up on " + user.strCreated + ".</li>\n";
		pResponse->strContent += "<li>Click <a href=\"/login.html?logout=1\">here</a> to logout.</li>\n";
		pResponse->strContent += "<li>Click <a href=\"/index.html\">here</a> to return to the tracker.</li>\n";
		pResponse->strContent += "</ul>\n";

		if( m_bDeleteOwnTorrents )
		{
			if( pRequest->mapParams.find( "del" ) != pRequest->mapParams.end( ) )
			{
				string strDelHashString = pRequest->mapParams["del"];
				string strDelHash = UTIL_StringToHash( strDelHashString );
				string strOK = pRequest->mapParams["ok"];

				if( strDelHash.empty( ) )
				{
					pResponse->strContent += "<p>Unable to delete torrent " + strDelHashString + ". The info hash is invalid. Click <a href=\"/login.html\">here</a> to return to the login page.</p>\n";

					if( m_bGen )
						pResponse->strContent += "<p class=\"gen\">Generated in " + UTIL_ElapsedTimeStr( btv, UTIL_CurrentTime( ) ) + " seconds.</p>\n";

					pResponse->strContent += "</body>\n";
					pResponse->strContent += "</html>\n";

					return;
				}
				else
				{
					if( strOK == "1" )
					{
						if( m_pTags )
						{
							CAtom *pTagInfo = m_pTags->getItem( strDelHash );

							if( pTagInfo && pTagInfo->isDicti( ) )
							{
								CAtom *pUploader = ( (CAtomDicti *)pTagInfo )->getItem( "uploader" );

								string strUploader;

								if( pUploader )
									strUploader = pUploader->toString( );

								if( strUploader != user.strLogin )
								{
									pResponse->strContent += "<p>Unable to delete torrent " + strDelHashString + ". You didn't upload that torrent. Click <a href=\"/login.html\">here</a> to return to the login page.</p>\n";

									if( m_bGen )
										pResponse->strContent += "<p class=\"gen\">Generated in " + UTIL_ElapsedTimeStr( btv, UTIL_CurrentTime( ) ) + " seconds.</p>\n";

									pResponse->strContent += "</body>\n";
									pResponse->strContent += "</html>\n";

									return;
								}
							}
						}

						if( m_pAllowed )
						{
							// delete from disk

							CAtom *pList = m_pAllowed->getItem( strDelHash );

							if( pList && pList->isList( ) )
							{
								vector<CAtom *> vecTorrent = ( (CAtomList *)pList )->getValue( );

								if( vecTorrent.size( ) == 6 )
								{
									CAtom *pFileName = vecTorrent[0];

									if( pFileName )
									{
										if( m_strArchiveDir.empty( ) )
											UTIL_DeleteFile( ( m_strAllowedDir + pFileName->toString( ) ).c_str( ) );
										else
											UTIL_MoveFile( ( m_strAllowedDir + pFileName->toString( ) ).c_str( ), ( m_strArchiveDir + pFileName->toString( ) ).c_str( ) );
									}
								}
							}

							m_pAllowed->delItem( strDelHash );
							m_pDFile->delItem( strDelHash );
							m_pTags->delItem( strDelHash );

							// The Trinity Edition - Modification Begins
							// The following alerts the user that the torrent has been deleted
							// and AUTOMATICALLY returns him/her to the login/mytorrents page.

							/* Original Source Code:
							pResponse->strContent += "<p>Deleted torrent " + strDelHashString + ". Click <a href=\"/login.html\">here</a> to return to the login page.</p>\n";
							*/

							pResponse->strContent += "<p>Torrent Deleted: " + strDelHashString + "</p>\n\n";

							pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
							pResponse->strContent += "alert('TORRENT DELETED! \\n\\n The following torrent has been deleted: \\n\\n Hash ID: " + strDelHashString + "\\n\\n' + 'Press OK to return to the My Torrents page.');\n";
							pResponse->strContent += "window.location=\"/login.html\";\n";
							pResponse->strContent += "</script>\n\n";

							// ------------------------------------------------- END OF MODIFICATION

							if( m_bGen )
								pResponse->strContent += "<p class=\"gen\">Generated in " + UTIL_ElapsedTimeStr( btv, UTIL_CurrentTime( ) ) + " seconds.</p>\n";

							pResponse->strContent += "</body>\n";
							pResponse->strContent += "</html>\n";

							return;
						}
					}
					else
					{

						// The Trinity Edition - Modification Begins
						// The following uses a JavaScript confirmation when deleting a torrent
						// and removes the standard prompt.

						/* Original Source Code:
						pResponse->strContent += "<p>Are you sure you want to delete the torrent " + strDelHashString + "?</p>\n";
						pResponse->strContent += "<p><a href=\"/login.html?del=" + strDelHashString + "&ok=1\">OK</a></p>\n";
						*/

						pResponse->strContent += "<p>Deleting Torrent...</p>\n\n";

						pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
						pResponse->strContent += "if ( confirm ('DELETE CONFIRMATION! \\n\\n Are you sure you want to delete the following torrent? \\n\\n Hash ID: " + strDelHashString + "') ) {\n";
						pResponse->strContent += "  window.location=\"/login.html?del=" + strDelHashString + "&ok=1\";\n";
						pResponse->strContent += "}\n";
						pResponse->strContent += "else {\n";
						pResponse->strContent += "  window.history.back();\n";
						pResponse->strContent += "}\n";
						pResponse->strContent += "</script>\n\n";

						// ------------------------------------------------------ END OF MODIFICATION


						pResponse->strContent += "</body>\n";
						pResponse->strContent += "</html>\n";

						return;
					}
				}
			}
		}

		if( m_pTags )
		{
			bool bFound = false;

			map<string, CAtom *> mapDicti = m_pTags->getValue( );

			for( map<string, CAtom *> :: iterator i = mapDicti.begin( ); i != mapDicti.end( ); i++ )
			{
				if( (*i).second->isDicti( ) )
				{
					CAtom *pUploader = ( (CAtomDicti *)(*i).second )->getItem( "uploader" );

					string strUploader;

					if( pUploader )
						strUploader = pUploader->toString( );

					if( strUploader != user.strLogin )
						continue;

					if( !bFound )
					{
						pResponse->strContent += "<p>Your Torrents</p>\n";
						pResponse->strContent += "<ul>\n";

						bFound = true;
					}

					// The Trinity Edition - Modification Begins
					// The following places a button at the beginning 
					// of each Torrent Name, to delete one's own torrents

					pResponse->strContent += "<li>\n";

					if( m_bDeleteOwnTorrents ) {
                        pResponse->strContent += "<input class=\"delete\" type=\"button\" value=\"Delete\" onClick=\"window.location=\'/login.html?del=" + UTIL_HashToString( (*i).first ) + "\'\">\n";
					}
					
					/* Original Source Code:
					pResponse->strContent += "<li><a href=\"/stats.html?info_hash=" + UTIL_HashToString( (*i).first ) + "\">";
					*/

					// --------------------------------------- END OF MODIFICATION

					string strName = UTIL_HashToString( (*i).first );

					if( m_pAllowed )
					{
						CAtom *pTorrent = m_pAllowed->getItem( (*i).first );

						if( pTorrent && pTorrent->isList( ) )
						{
							vector<CAtom *> vecTorrent = ( (CAtomList *)pTorrent )->getValue( );

							if( vecTorrent.size( ) == 6 )
							{
								if( vecTorrent[1] )
									strName = vecTorrent[1]->toString( );
							}
						}
					}

					// The Trinity Edition - Modification Begins
					// Creates a STATS button in front of each torrent name on the MyTorrents page
					// and removes the hyperlink on the actual torrent name

					if( user.iAccess & ACCESS_VIEW ) {
						pResponse->strContent += "<input class=\"stats\" type=\"button\" value=\"Torrent Stats\" onClick=\"window.location=\'/stats.html?info_hash=" + UTIL_HashToString( (*i).first ) + "\'\"> " + strName + "\n";
					}
					else {
						pResponse->strContent += "<input disabled class=\"stats\" type=\"button\" value=\"Torrent Stats\"> " + strName + "\n";
					}

					/* Original Source Code:
					pResponse->strContent += strName + "</a>";
					*/

					// The Trinity Edition - Removal Begins
					// Removes Delete Link; New Delete button
					// will be placed at the beginning of each
					// Torrent Name

					/* Original Source Code:
					if( m_bDeleteOwnTorrents )
						pResponse->strContent += " [<a href=\"/login.html?del=" + UTIL_HashToString( (*i).first ) + "\">DELETE</a>]";
					*/

					// ------------------------------------- END OF REMOVAL

					pResponse->strContent += "</li>\n";

				}
			}

			if( bFound )
				pResponse->strContent += "</ul>\n";
		}
	}

	if( m_bGen )
		pResponse->strContent += "<p class=\"gen\">Generated in " + UTIL_ElapsedTimeStr( btv, UTIL_CurrentTime( ) ) + " seconds.</p>\n";

	pResponse->strContent += "</body>\n";
	pResponse->strContent += "</html>\n";
}
