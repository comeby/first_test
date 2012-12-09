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
#include "sort.h"
#include "tracker.h"
#include "util.h"

void CTracker :: serverResponseIndex( struct request_t *pRequest, struct response_t *pResponse, user_t user )
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

	// The Trinity Edition - Modification Begins
	// Changed Title of Page

	// Uses a Custom Tracker Title in place of BNBT - bnbt_tracker_title
	if ( !gstrTrackerTitle.empty( ) ) {
		pResponse->strContent += "<title>" + gstrTrackerTitle + " Tracker Info</title>\n";
	}
	else {
		pResponse->strContent += "<title>BNBT Tracker Info</title>\n"; // <-- Original Source Code
	}

	if( !gstrStyle.empty( ) )
		pResponse->strContent += "<link rel=stylesheet type=\"text/css\" href=\"" + gstrStyle + "\">\n";

	pResponse->strContent += "</head>\n";
	pResponse->strContent += "<body>\n";

	// The Trinity Edition - Addition Begins
	// The following displays a special navigation bar on index.html
	// Specific Buttons are displayed based on the User's Access Level

if( m_bUseNavbar ) {

	pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
	pResponse->strContent += "  function torrentsearch() {\n";
	pResponse->strContent += "    var search = document.navbar.search.value;\n";
	pResponse->strContent += "    var newurl = document.location + \"?search=\" + search;\n";
	pResponse->strContent += "    document.location = newurl;\n";
	pResponse->strContent += "  }\n";
	pResponse->strContent += "  function logout() {\n";
	pResponse->strContent += "    if ( confirm ('PLEASE CONFIRM! \\n\\n Are you sure you want to logout? \\n\\n') ) {\n";
	pResponse->strContent += "         document.location = \"/login.html?logout=1\";\n";
	pResponse->strContent += "    }\n";
	pResponse->strContent += "  }\n";
	pResponse->strContent += "</script>\n\n";

	pResponse->strContent += "<table class=\"navbar\" width=\"100%\"><tr class=\"navbar\"><td class=\"navbar\">\n\n";

	pResponse->strContent += "<form style=\"display:inline\" class=\"navbar_search\" name=\"navbar\">";

	if( user.strLogin.empty( ) ) {
		pResponse->strContent += "<input class=\"index_login\" type=\"button\" value=\"Login\" onClick=\"window.location=\'/login.html\'\">\n";
		
		if( user.iAccess & ACCESS_SIGNUP ) {
			pResponse->strContent += "<input class=\"index_signup\" type=\"button\" value=\"Register\" onClick=\"window.location=\'/signup.html\'\">\n";
		}
		else {
			pResponse->strContent += "<input disabled class=\"index_signup\" type=\"button\" value=\"Register\">\n";
		}

		if( user.iAccess & ACCESS_UPLOAD ) {
			pResponse->strContent += "<input class=\"index_upload\" type=\"button\" value=\"Upload Torrent File\" onClick=\"window.location=\'/upload.html\'\">\n";
		}
		else {
			pResponse->strContent += "<input disabled class=\"index_upload\" type=\"button\" value=\"Upload Torrent File\">\n";
		}

		if( user.iAccess & ACCESS_VIEW ) {
			pResponse->strContent += "<input class=\"index_info\" type=\"button\" value=\"Tracker Info\" onClick=\"window.location=\'/info.html\'\">\n";
		}
		else {
			pResponse->strContent += "<input disabled class=\"index_info\" type=\"button\" value=\"Tracker Info\">\n";
		}

		if( user.iAccess & ACCESS_VIEW ) {

			pResponse->strContent += "</td><td class=\"navbar_search\">\n\n";

				if( m_bUseCustomlabels ) {
					pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
					pResponse->strContent += "if (typeof(labelSearch) == \"undefined\") {\n";
					pResponse->strContent += "   document.write(\"Search\");\n";
					pResponse->strContent += "}\n";
					pResponse->strContent += "else {\n";
					pResponse->strContent += "   document.write(labelSearch);\n";
					pResponse->strContent += "}\n";
					pResponse->strContent += "</script>\n";
				}
				else {
					pResponse->strContent += "Search\n";
				}

			pResponse->strContent += "<input class=\"navbar_search\" type=\"text\" name=\"search\">\n";
			pResponse->strContent += "<input class=\"go\" type=\"button\" value=\"Go\" onClick=\"torrentsearch()\">\n";

			pResponse->strContent += "<input class=\"index_clear\" type=\"button\" value=\"Clear Results\" onClick=\"window.location=\'/index.html\'\">\n";

		}
		else {

			pResponse->strContent += "</td><td class=\"navbar_search\">\n\n";

				if( m_bUseCustomlabels ) {
					pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
					pResponse->strContent += "if (typeof(labelSearch) == \"undefined\") {\n";
					pResponse->strContent += "   document.write(\"Search\");\n";
					pResponse->strContent += "}\n";
					pResponse->strContent += "else {\n";
					pResponse->strContent += "   document.write(labelSearch);\n";
					pResponse->strContent += "}\n";
					pResponse->strContent += "</script>\n";
				}
				else {
					pResponse->strContent += "Search\n";
				}

			pResponse->strContent += "<input disabled class=\"navbar_search\" type=\"text\" name=\"search\">\n";
			pResponse->strContent += "<input disabled class=\"go\" type=\"button\" value=\"Go\">\n";

			pResponse->strContent += "<input disabled class=\"index_clear\" type=\"button\" value=\"Clear Results\">\n";

		}

	}
	else { // Display the following when the User String is not empty (User is logged in)

		pResponse->strContent += "<input class=\"index_logout\" type=\"button\" value=\"Logout &raquo; " + UTIL_RemoveHTML( user.strLogin ) + "\" onClick=\"logout()\">\n";

		if( user.iAccess & ACCESS_UPLOAD ) {
			pResponse->strContent += "| <input class=\"index_mytorrents\" type=\"button\" value=\"My Torrents\" onClick=\"window.location=\'/login.html\'\">\n";
			pResponse->strContent += "<input class=\"index_upload\" type=\"button\" value=\"Upload Torrent File\" onClick=\"window.location=\'/upload.html\'\">\n";
		}
		else {
			pResponse->strContent += "| <input disabled class=\"index_mytorrents\" type=\"button\" value=\"My Torrents\">\n";
			pResponse->strContent += "<input disabled class=\"index_upload\" type=\"button\" value=\"Upload Torrent File\">\n";
		}

		if( user.iAccess & ACCESS_VIEW ) {
			pResponse->strContent += "<input class=\"index_info\" type=\"button\" value=\"Tracker Info\" onClick=\"window.location=\'/info.html\'\">\n";
		}
		else {
			pResponse->strContent += "<input disabled class=\"index_info\" type=\"button\" value=\"Tracker Info\">\n";
		}

		if( user.iAccess & ACCESS_ADMIN ) { // Manage Users and Admin CP buttons - ONLY ACTIVE IF USER IS ADMIN
			pResponse->strContent += "| <input class=\"index_users\" type=\"button\" value=\"Manage Users\" onClick=\"window.location=\'/users.html\'\">\n";
			pResponse->strContent += "<input class=\"index_admin\" type=\"button\" value=\"Admin CP\" onClick=\"window.location=\'/admin.html\'\">\n";
		}
		else {
			pResponse->strContent += "| <input disabled class=\"index_users\" type=\"button\" value=\"Manage Users\">\n";
			pResponse->strContent += "<input disabled class=\"index_admin\" type=\"button\" value=\"Admin CP\">\n";
		}

		if( user.iAccess & ACCESS_VIEW ) {

			pResponse->strContent += "</td><td class=\"navbar_search\">\n\n";

				if( m_bUseCustomlabels ) {
					pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
					pResponse->strContent += "if (typeof(labelSearch) == \"undefined\") {\n";
					pResponse->strContent += "   document.write(\"Search\");\n";
					pResponse->strContent += "}\n";
					pResponse->strContent += "else {\n";
					pResponse->strContent += "   document.write(labelSearch);\n";
					pResponse->strContent += "}\n";
					pResponse->strContent += "</script>\n";
				}
				else {
					pResponse->strContent += "Search\n";
				}

			pResponse->strContent += "<input class=\"navbar_search\" type=\"text\" name=\"search\">\n";
			pResponse->strContent += "<input class=\"go\" type=\"button\" value=\"Go\" onClick=\"torrentsearch()\">\n";

			pResponse->strContent += "<input class=\"index_clear\" type=\"button\" value=\"Clear Results\" onClick=\"window.location=\'/index.html\'\">\n";

		}
		else {

			pResponse->strContent += "</td><td class=\"navbar_search\">\n\n";

				if( m_bUseCustomlabels ) {
					pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
					pResponse->strContent += "if (typeof(labelSearch) == \"undefined\") {\n";
					pResponse->strContent += "   document.write(\"Search\");\n";
					pResponse->strContent += "}\n";
					pResponse->strContent += "else {\n";
					pResponse->strContent += "   document.write(labelSearch);\n";
					pResponse->strContent += "}\n";
					pResponse->strContent += "</script>\n";
				}
				else {
					pResponse->strContent += "Search\n";
				}

			pResponse->strContent += "<input disabled class=\"navbar_search\" type=\"text\" name=\"search\">\n";
			pResponse->strContent += "<input disabled class=\"go\" type=\"button\" value=\"Go\">\n";

			pResponse->strContent += "<input disabled class=\"index_clear\" type=\"button\" value=\"Clear Results\">\n";

		}

	}

	pResponse->strContent += "</form>\n";

	pResponse->strContent += "</td></tr></table>\n\n";

}

	// ------------------------------------------------- END OF ADDITION

// The Trinity Edition - Modification Begins
// Removes Login and Logged In Message when using the NAVBAR

if( !m_bUseNavbar ) {

	if( user.strLogin.empty( ) )
		pResponse->strContent += "<p class=\"login1_index\">You are not logged in. Click <a href=\"/login.html\">here</a> to login.</p>\n";
	else
		pResponse->strContent += "<p class=\"login2_index\">You are logged in as <span class=\"username\">" + UTIL_RemoveHTML( user.strLogin ) + "</span>. Click <a href=\"/login.html?logout=1\">here</a> to logout.</p>\n";

}

	/* Original Source Code:
	if( user.strLogin.empty( ) )
		pResponse->strContent += "<p class=\"login1_index\">You are not logged in. Click <a href=\"/login.html\">here</a> to login.</p>\n";
	else
		pResponse->strContent += "<p class=\"login2_index\">You are logged in as <span class=\"username\">" + UTIL_RemoveHTML( user.strLogin ) + "</span>. Click <a href=\"/login.html?logout=1\">here</a> to logout.</p>\n";
	*/

// ------------------------------------------------- END OF MODIFICATION

	if( user.iAccess & ACCESS_VIEW )
	{
		//
		// delete torrent
		//

		if( user.iAccess & ACCESS_EDIT )
		{
			if( pRequest->mapParams.find( "del" ) != pRequest->mapParams.end( ) )
			{
				string strDelHashString = pRequest->mapParams["del"];
				string strDelHash = UTIL_StringToHash( strDelHashString );
				string strOK = pRequest->mapParams["ok"];

				if( strDelHash.empty( ) )
				{
					pResponse->strContent += "<p>Unable to delete torrent " + strDelHashString + ". The info hash is invalid. Click <a href=\"/index.html\">here</a> to return to the tracker.</p>\n";

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
							pResponse->strContent += "<p>Deleted torrent " + strDelHashString + ". Click <a href=\"/index.html\">here</a> to return to the tracker.</p>\n";
							*/

							pResponse->strContent += "<p>Torrent Deleted: " + strDelHashString + "</p>\n\n";

							pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
							pResponse->strContent += "alert('TORRENT DELETED! \\n\\n The following torrent has been deleted: \\n\\n Hash ID: " + strDelHashString + "\\n\\n' + 'Press OK to return to the Tracker.');\n";
							pResponse->strContent += "window.location=\"/index.html\";\n";
							pResponse->strContent += "</script>\n\n";

							// ------------------------------------------------ END OF MODIFICATION

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
						// The following prompts the user for delete confirmation using a JavaScript Prompt

						/* Original Source Code:
						pResponse->strContent += "<p>Are you sure you want to delete the torrent " + strDelHashString + "?</p>\n";
						pResponse->strContent += "<p><a href=\"/index.html?del=" + strDelHashString + "&ok=1\">OK</a></p>\n";
						*/

						pResponse->strContent += "<p>Deleting Torrent...</p>\n\n";

						pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
						pResponse->strContent += "if ( confirm ('DELETE CONFIRMATION! \\n\\n Are you sure you want to delete the following torrent? \\n\\n Hash ID: " + strDelHashString + "') ) {\n";
						pResponse->strContent += "  window.location=\"/index.html?del=" + strDelHashString + "&ok=1\";\n";
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

		pResponse->strContent += m_strStaticHeader;

		// The Trinity Edition - Addition Begins
		// The following code sets a CSS class "index" using a <div> tag that can be used
		// to specify style attributes... hide, center, etc.

		pResponse->strContent += "<div class=\"index\">\n\n";

		// ------------------------------------------------- End of Addition

		if( m_pDFile )
		{
			if( m_pDFile->isEmpty( ) )
			{
				pResponse->strContent += "<p>Not tracking any files yet!</p>\n";
				
				pResponse->strContent += m_strStaticFooter;

				if( m_bGen )
					pResponse->strContent += "<p class=\"gen\">Generated in " + UTIL_ElapsedTimeStr( btv, UTIL_CurrentTime( ) ) + " seconds.</p>\n";

				pResponse->strContent += "</body>\n";
				pResponse->strContent += "</html>\n";

				return;
			}

			map<string, CAtom *> mapDicti = m_pDFile->getValue( );

			unsigned long iKeySize = mapDicti.size( );

			// add the torrents into this structure one by one and sort it afterwards

			struct torrent_t *pTorrents = new struct torrent_t[iKeySize];

			unsigned long i = 0;

			for( map<string, CAtom *> :: iterator it = mapDicti.begin( ); it != mapDicti.end( ); it++ )
			{
				pTorrents[i].strInfoHash = (*it).first;
				pTorrents[i].strName = "unknown";
				pTorrents[i].strLowerName = "unknown";
				pTorrents[i].iComplete = 0;
				pTorrents[i].iDL = 0;
				pTorrents[i].iSize = 0;
				pTorrents[i].iFiles = 0;
				pTorrents[i].iComments = 0;
				pTorrents[i].iAverageLeft = 0;
				pTorrents[i].iAverageLeftPercent = 0;
				pTorrents[i].iMinLeft = 0;
				pTorrents[i].iMaxiLeft = 0;
				pTorrents[i].iCompleted = 0;
				pTorrents[i].iTransferred = 0;

				if( (*it).second->isDicti( ) )
				{
					map<string, CAtom *> mapPeersDicti = ( (CAtomDicti *)(*it).second )->getValue( );

					bool bFirst = true;

					for( map<string, CAtom *> :: iterator j = mapPeersDicti.begin( ); j != mapPeersDicti.end( ); j++ )
					{
						if( (*j).second->isDicti( ) )
						{
							CAtom *pLeft = ( (CAtomDicti *)(*j).second )->getItem( "left" );

							if( pLeft && pLeft->isLong( ) )
							{
								int64 iLeft = ( (CAtomLong *)pLeft )->getValue( );

								if( iLeft == 0 )
									pTorrents[i].iComplete++;
								else
								{
									pTorrents[i].iDL++;

									// only calculate average / min / max on leechers

									pTorrents[i].iAverageLeft += iLeft;

									if( bFirst || iLeft < pTorrents[i].iMinLeft )
										pTorrents[i].iMinLeft = iLeft;

									if( bFirst || iLeft > pTorrents[i].iMaxiLeft )
										pTorrents[i].iMaxiLeft = iLeft;

									bFirst = false;
								}
							}
						}
					}

					if( m_pAllowed )
					{
						CAtom *pList = m_pAllowed->getItem( pTorrents[i].strInfoHash );

						if( pList && pList->isList( ) )
						{
							vector<CAtom *> vecTorrent = ( (CAtomList *)pList )->getValue( );

							if( vecTorrent.size( ) == 6 )
							{
								CAtom *pFileName = vecTorrent[0];
								CAtom *pName = vecTorrent[1];
								CAtom *pAdded = vecTorrent[2];
								CAtom *pSize = vecTorrent[3];
								CAtom *pFiles = vecTorrent[4];

								if( pFileName )
									pTorrents[i].strFileName = pFileName->toString( );

								if( pName )
								{
									// stick a lower case version in strNameLower for non case sensitive searching and sorting

									pTorrents[i].strName = pName->toString( );
									pTorrents[i].strLowerName = UTIL_ToLower( pTorrents[i].strName );
								}

								if( pAdded )
									pTorrents[i].strAdded = pAdded->toString( );

								if( pSize && pSize->isLong( ) )
									pTorrents[i].iSize = ( (CAtomLong *)pSize )->getValue( );

								if( pFiles && pFiles->isInt( ) )
									pTorrents[i].iFiles = ( (CAtomInt *)pFiles )->getValue( );
							}
						}

						if( m_bAllowComments )
						{
							if( m_pComments )
							{
								CAtom *pCommentList = m_pComments->getItem( pTorrents[i].strInfoHash );

								if( pCommentList && pCommentList->isList( ) )
									pTorrents[i].iComments = ( (CAtomList *)pCommentList )->getValue( ).size( );
							}
						}
					}

					if( m_pCompleted )
					{
						CAtom *pCompleted = m_pCompleted->getItem( pTorrents[i].strInfoHash );

						if( pCompleted && pCompleted->isLong( ) )
						{
							pTorrents[i].iCompleted = (int)( (CAtomLong *)pCompleted )->getValue( );

							// size has already been found, calculate transferred

							if( m_pAllowed && m_bShowTransferred )
								pTorrents[i].iTransferred = pTorrents[i].iCompleted * pTorrents[i].iSize;
						}
					}

					if( m_pTags )
					{
						CAtom *pDicti = m_pTags->getItem( pTorrents[i].strInfoHash );

						if( pDicti && pDicti->isDicti( ) )
						{
							CAtom *pTag = ( (CAtomDicti *)pDicti )->getItem( "tag" );
							CAtom *pName = ( (CAtomDicti *)pDicti )->getItem( "name" );
							CAtom *pUploader = ( (CAtomDicti *)pDicti )->getItem( "uploader" );
							CAtom *pInfoLink = ( (CAtomDicti *)pDicti )->getItem( "infolink" );

							if( pTag )
								pTorrents[i].strTag = pTag->toString( );

							if( pName )
							{
								// this will overwrite the previous name, i.e. the filename

								pTorrents[i].strName = pName->toString( );
								pTorrents[i].strLowerName = UTIL_ToLower( pTorrents[i].strName );
							}

							if( pUploader )
								pTorrents[i].strUploader = pUploader->toString( );

							if( pInfoLink )
								pTorrents[i].strInfoLink = pInfoLink->toString( );
						}
					}

					if( m_bShowAverageLeft )
					{
						// iAverageLeft actually contains the total left at this point, so find the average

						if( pTorrents[i].iDL > 0 )
							pTorrents[i].iAverageLeft /= pTorrents[i].iDL;

						if( pTorrents[i].iSize > 0 )
							pTorrents[i].iAverageLeftPercent = (int)( ( (float)pTorrents[i].iAverageLeft / pTorrents[i].iSize ) * 100 );
					}
				}

				i++;
			}

			string strSort = pRequest->mapParams["sort"];

			if( m_bSort )
			{
				if( !strSort.empty( ) )
				{
					int iSort = atoi( strSort.c_str( ) );

					if( iSort == SORT_ANAME )
					{
						if( m_pAllowed && m_bShowNames )
							qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), asortByName );
					}
					else if( iSort == SORT_ACOMPLETE )
						qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), asortByComplete );
					else if( iSort == SORT_AINCOMPLETE )
						qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), asortByDL );
					else if( iSort == SORT_AADDED )
					{
						if( m_pAllowed && m_bShowAdded )
							qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), asortByAdded );
					}
					else if( iSort == SORT_ASIZE )
					{
						if( m_pAllowed && m_bShowSize )
							qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), asortBySize );
					}
					else if( iSort == SORT_AFILES )
					{
						if( m_pAllowed && m_bShowNumFiles )
							qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), asortByFiles );
					}
					else if( iSort == SORT_ACOMMENTS )
					{
						if( m_pAllowed && m_bAllowComments )
							qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), asortByComments );
					}
					else if( iSort == SORT_AAVGLEFT )
					{
						if( m_bShowAverageLeft )
						{
							if( m_pAllowed )
							{
								if( m_bShowLeftAsProgress )
									qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), dsortByAvgLeftPercent );
								else
									qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), asortByAvgLeftPercent );
							}
							else
								qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), asortByAvgLeft );
						}
					}
					else if( iSort == SORT_ACOMPLETED )
					{
						if( m_bShowCompleted )
							qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), asortByCompleted );
					}
					else if( iSort == SORT_ATRANSFERRED )
					{
						if( m_pAllowed && m_bShowTransferred )
							qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), asortByTransferred );
					}
					else if( iSort == SORT_DNAME )
					{
						if( m_pAllowed && m_bShowNames )
							qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), dsortByName );
					}
					else if( iSort == SORT_DCOMPLETE )
						qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), dsortByComplete );
					else if( iSort == SORT_DINCOMPLETE )
						qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), dsortByDL );
					else if( iSort == SORT_DADDED )
					{
						if( m_pAllowed && m_bShowAdded )
							qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), dsortByAdded );
					}
					else if( iSort == SORT_DSIZE )
					{
						if( m_pAllowed && m_bShowSize )
							qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), dsortBySize );
					}
					else if( iSort == SORT_DFILES )
					{
						if( m_pAllowed && m_bShowNumFiles )
							qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), dsortByFiles );
					}
					else if( iSort == SORT_DCOMMENTS )
					{
						if( m_pAllowed && m_bAllowComments )
							qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), dsortByComments );
					}
					else if( iSort == SORT_DAVGLEFT )
					{
						if( m_bShowAverageLeft )
						{
							if( m_pAllowed )
							{
								if( m_bShowLeftAsProgress )
									qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), asortByAvgLeftPercent );
								else
									qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), dsortByAvgLeftPercent );
							}
							else
								qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), dsortByAvgLeft );
						}
					}
					else if( iSort == SORT_DCOMPLETED )
					{
						if( m_bShowCompleted )
							qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), dsortByCompleted );
					}
					else if( iSort == SORT_DTRANSFERRED )
					{
						if( m_pAllowed && m_bShowTransferred )
							qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), dsortByTransferred );
					}
					else
					{
						// default action is to sort by added if we can

						if( m_pAllowed && m_bShowAdded )
							qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), dsortByAdded );
					}
				}
				else
				{
					// default action is to sort by added if we can

					if( m_pAllowed && m_bShowAdded )
						qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), dsortByAdded );
				}
			}
			else
			{
				// sort is disabled, but default action is to sort by added if we can

				if( m_pAllowed && m_bShowAdded )
					qsort( pTorrents, iKeySize, sizeof( struct torrent_t ), dsortByAdded );
			}

			// some preliminary search crap

			string strSearch = pRequest->mapParams["search"];
			string strLowerSearch = UTIL_ToLower( strSearch );
			string strSearchResp = UTIL_StringToEscaped( strSearch );

			if( !strSearch.empty( ) && m_pAllowed && m_bShowNames && m_bSearch )
				pResponse->strContent += "<p class=\"search_results\">Search results for \"" + UTIL_RemoveHTML( strSearch ) + "\".</p>\n";

			// filters

			string strFilter = pRequest->mapParams["filter"];

			if( !m_vecTags.empty( ) )
			{
				// pResponse->strContent += "<p align=\"center\">";

				bool bFound = false;

				for( vector< pair<string, string> > :: iterator i = m_vecTags.begin( ); i != m_vecTags.end( ); i++ )
				{
					if( !bFound )
											
						// The Trinity Edition - Modification Begins

						// The following changes the Clear Filter link to a button.

						/* The following adds the Custom Label Feature
						to the NEW "Clear Filter" button and sets a CSS class
						"clearfilterbutton" which can be used to HIDE the button
						using the following CSS command: */

						// .clearfilterbutton{display:none}

					{

						if( m_bUseCustomlabels ) {
							pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
							pResponse->strContent += "if (typeof(buttonClearFilter) == \"undefined\") {\n";
							pResponse->strContent += "   document.write(\"<input class=\\\"clearfilter\\\" type=\\\"button\\\" value=\\\"Clear Filter\\\" onClick=\\\"window.location=\'/index.html\'\\\">\");\n";
							pResponse->strContent += "}\n";
							pResponse->strContent += "else {\n";
							pResponse->strContent += "   document.write(\"<input class=\\\"clearfilter\\\" type=\\\"button\\\" value=\\\"\" + buttonClearFilter + \"\\\" onClick=\\\"window.location=\'/index.html\'\\\">\");\n";
							pResponse->strContent += "}\n";
							pResponse->strContent += "</script>\n";	
						}
						else { // Use Original Source Code WITH PARAGRAPH TAGS AND BUTTON CODE:
							pResponse->strContent += "<input class=\"clearfilter\" type=\"button\" value=\"Clear Filter\" onClick=\"window.location=\'/index.html\'\">\n";
						}

						pResponse->strContent += "<p>\n";

					}

						/* Original Source Code:
						pResponse->strContent += "<a href=\"/index.html\">Clear Filter</a><br><br>\n";
						*/

						// ------------------------------------------------- End of Modification

					pResponse->strContent += "<a href=\"/index.html?filter=" + UTIL_StringToEscaped( (*i).first );

					if( !strSort.empty( ) )
						pResponse->strContent += "&sort=" + strSort;

					if( !strSearch.empty( ) )
						pResponse->strContent += "&search=" + strSearchResp;

					pResponse->strContent += "\">";

					// The Trinity Edition - Modification Begins
					// The following code does two things.

					// 1. Sets a TITLE parameter for each Tag Image, so that a TOOLTIP box will popup when
					// the user's mouse pointer hovers over the Tag Image.

					// 2. Assigns functions to onMouseOver and onMouseOut for each Tag Image
					// whose actions can be specified in the HEADER.HTML document to create Image Flips.
					// The function names are hoverOnTagname() and hoverOffTagname() and are case sensitive
					// Each Tag Image is referenced using JavaScript: document.Tagname.src and is case sensitive

					// MouseOvers should only be activated if you plan on creating the corresponding functions in HEADER.HTML
					// Activated by setting "trinity_use_mouseovers" to 1

					if( !(*i).second.empty( ) ) {

						if( m_bUseMouseovers ) {
							pResponse->strContent += "<img src=\"" + (*i).second + "\" border=\"0\" title=\"" + (*i).first + "\" name=\"" + (*i).first + "\" onMouseOver=\"hoverOn" + (*i).first + "()\" onMouseOut=\"hoverOff" + (*i).first + "()\"; return true;>";
						}
						else {
							pResponse->strContent += "<img src=\"" + (*i).second + "\" border=\"0\" title=\"" + (*i).first + "\" name=\"" + (*i).first + "\">";
						}
					}

					else {
						pResponse->strContent += UTIL_RemoveHTML( (*i).first );
					}

					/* Original Source Code:
					if( !(*i).second.empty( ) )
						pResponse->strContent += "<img src=\"" + (*i).second + "\" border=\"0\">";
					else
						pResponse->strContent += UTIL_RemoveHTML( (*i).first );
					*/

					pResponse->strContent += "</a>";

					if( i + 1 != m_vecTags.end( ) )
						pResponse->strContent += " <span class=\"pipe\">|</span> ";

					bFound = true;
				}

				pResponse->strContent += "</p>\n";
			}

			// which page are we viewing

			unsigned int iStart = 0;

			if( m_iPerPage > 0 )
			{
				string strPage = pRequest->mapParams["page"];

				if( !strPage.empty( ) )
					iStart = atoi( strPage.c_str( ) ) * m_iPerPage;

				// The Trinity Edition - Modification Begins

				// The following adds the Custom Label Feature to the string 
				// of text that appears just before the current page number,
				// which is displayed just after the Tag Filter Images.

				pResponse->strContent += "<p class=\"pagenum_top\">\n";

					if ( m_bUseCustomlabels ) {
						pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
						pResponse->strContent += "if (typeof(labelPageNumPrefix) == \"undefined\") {\n";
						pResponse->strContent += "   document.write(\"Page\");\n";
						pResponse->strContent += "}\n";
						pResponse->strContent += "else {\n";
						pResponse->strContent += "   document.write(labelPageNumPrefix);\n";
						pResponse->strContent += "}\n";
						pResponse->strContent += "</script>\n";
					}
					else { // Use Original String:
						pResponse->strContent += "Page";
					}

				pResponse->strContent += " " + CAtomInt( ( iStart / m_iPerPage ) + 1 ).toString( ) + "\n";

				/* Original Source Code:
				pResponse->strContent += "<p class=\"pagenum_top\">Page " + CAtomInt( ( iStart / m_iPerPage ) + 1 ).toString( ) + "</p>\n";
				*/

				/* The following creates an internal document link that will
				bring the user to the bottom of the Torrent Table. It also 
				sets a CSS class (bottomlink) which can be used to
				HIDE this link using the following CSS command: */

				// .bottomlink{display:none}

				// The text of the link is also customizable using the Custom Label Feature

				pResponse->strContent += "<span class=\"bottomlink\">| <a href=\"#search\">";

					if ( m_bUseCustomlabels ) {
						pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
						pResponse->strContent += "if (typeof(linkBottomOfPage) == \"undefined\") {\n";
						pResponse->strContent += "   document.write(\"Bottom of Page\");\n";
						pResponse->strContent += "}\n";
						pResponse->strContent += "else {\n";
						pResponse->strContent += "   document.write(linkBottomOfPage);\n";
						pResponse->strContent += "}\n";
						pResponse->strContent += "</script>\n";
					}
					else {
						pResponse->strContent += "Bottom of Page";
					}
					
				pResponse->strContent += "</a></span>";
				
				pResponse->strContent += "</p>\n";

				// ------------------------------------------------------------------- END OF MODIFICATION

			}

			bool bFound = false;

			int iAdded = 0;
			int iSkipped = 0;

			// for correct page numbers after searching

			int iFound = 0;

			for( unsigned long i = 0; i < iKeySize; i++ )
			{
				if( !strFilter.empty( ) )
				{
					// only display entries that match the filter

					if( pTorrents[i].strTag != strFilter )
						continue;
				}

				if( !strSearch.empty( ) )
				{
					// only display entries that match the search

					if( pTorrents[i].strLowerName.find( strLowerSearch ) == string :: npos )
						continue;
				}

				iFound++;

				if( m_iPerPage == 0 || iAdded < m_iPerPage )
				{
					if( !bFound )
					{
						// output table headers

						// The Trinity Edition - ADD BEGINS
						// Assigns a class to a <div> element that surrounds the table

						pResponse->strContent += "<div class=\"torrent_table\">\n\n";

						// ----------------------------------- END OF ADD

						pResponse->strContent += "<table summary=\"files\" class=\"torrent_table\">\n";
						pResponse->strContent += "<tr>";

						// <th> tag

						if( !m_vecTags.empty( ) )
							// Trinity Edition - Modification Begins
							// The following adds the Custom Label Feature
							// Also adds the CSS class th.tag
							if( m_bUseCustomlabels ) {
								pResponse->strContent += "<th class=\"tag\">\n";
								pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
								pResponse->strContent += "if (typeof(headerTag) == \"undefined\") {\n";
								pResponse->strContent += "   document.write(\"Tag\");\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "else {\n";
								pResponse->strContent += "   document.write(headerTag);\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "</script>\n";
								pResponse->strContent += "</th>\n\n";
							}
							else { // Use Original Source Code WITH ADDED CLASS:
							pResponse->strContent += "<th class=\"tag\">Tag</th>";
							}

							// ------------------------------------------ END OF MODIFICATION

						// <th> info hash

						if( m_bShowInfoHash )
							// Trinity Edition - Modification Begins
							// The following adds the Custom  Label Feature
							if( m_bUseCustomlabels ) {
								pResponse->strContent += "<th class=\"hash\">\n";
								pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
								pResponse->strContent += "if (typeof(headerInfoHash) == \"undefined\") {\n";
								pResponse->strContent += "   document.write(\"Info Hash\");\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "else {\n";
								pResponse->strContent += "   document.write(headerInfoHash);\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "</script>\n";
								pResponse->strContent += "</th>\n\n";
							}
							else { // Use Original Source Code:
							pResponse->strContent += "<th class=\"hash\">Info Hash</th>";
							}

							// ------------------------------------------ END OF MODIFICATION


						// <th> name

						if( m_pAllowed && m_bShowNames )
						{
							// Trinity Edition - Modification Begins
							// The following adds the Custom Label Feature
							if( m_bUseCustomlabels ) {
								pResponse->strContent += "<th class=\"name\">\n";
								pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
								pResponse->strContent += "if (typeof(headerName) == \"undefined\") {\n";
								pResponse->strContent += "   document.write(\"Name\");\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "else {\n";
								pResponse->strContent += "   document.write(headerName);\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "</script>\n";
							}
							else { // Use Original Source Code:
							pResponse->strContent += "<th class=\"name\">Name";
							}

							// ------------------------------------------ END OF MODIFICATION

							if( m_bSort )
							{
								pResponse->strContent += "<br><a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_ANAME;

								if( !strSearch.empty( ) )
									pResponse->strContent += "&search=" + strSearchResp;

								if( !strFilter.empty( ) )
									pResponse->strContent += "&filter=" + UTIL_StringToEscaped( strFilter );

								// The Trinity Edition - MOD BEGINS - Replaces the Ascending Link Text when using Custom Labels

								if( m_bUseCustomlabels ) {
									pResponse->strContent += "\">\n\n";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(linkA) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"A\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(linkA);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
								}
								else {
									pResponse->strContent += "\">A";
								}

								pResponse->strContent += "</a> <a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_DNAME;

								/* Original Source Code:
								pResponse->strContent += "\">A</a> <a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_DNAME;
								*/

								// ------------------------------------------------------------- END OF MOD

								if( !strSearch.empty( ) )
									pResponse->strContent += "&search=" + strSearchResp;

								if( !strFilter.empty( ) )
									pResponse->strContent += "&filter=" + UTIL_StringToEscaped( strFilter );

								// The Trinity Edition - MOD BEGINS - Replaces the Descending Link Text when using Custom Labels

								if( m_bUseCustomlabels ) {
									pResponse->strContent += "\">";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(linkZ) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"Z\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(linkZ);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
									pResponse->strContent += "</a>";
								}
								else {
									pResponse->strContent += "\">Z</a>"; // <<< Original Source Code:
								}

								// ----------------------------------------------- END OF MOD

							}

							pResponse->strContent += "</th>";
						}

						// <th> torrent

						if( m_pAllowed && m_bAllowTorrentDownloads && ( user.iAccess & ACCESS_DL ) )
							// Trinity Edition - Modification Begins
							// The following adds the Custom Label Feature
							if( m_bUseCustomlabels ) {
								pResponse->strContent += "<th class=\"download\">\n";
								pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
								pResponse->strContent += "if (typeof(headerTorrent) == \"undefined\") {\n";
								pResponse->strContent += "   document.write(\"Torrent\");\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "else {\n";
								pResponse->strContent += "   document.write(headerTorrent);\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "</script>\n";
								pResponse->strContent += "</th>\n\n";
							}
							else { // Use Original Source Code:
							pResponse->strContent += "<th class=\"download\">Torrent</th>\n";
							}

							// ------------------------------------------ END OF MODIFICATION

						// <th> comments

						if( m_pAllowed && m_bAllowComments )
						{
							if( m_pComments )
							{
							// Trinity Edition - Modification Begins
							// The following adds the Custom Label Feature
							if( m_bUseCustomlabels ) {
								pResponse->strContent += "<th class=\"number\">\n";
								pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
								pResponse->strContent += "if (typeof(headerComments) == \"undefined\") {\n";
								pResponse->strContent += "   document.write(\"Comments\");\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "else {\n";
								pResponse->strContent += "   document.write(headerComments);\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "</script>\n";
							}
							else { // Use Original Source Code:
								pResponse->strContent += "<th class=\"number\">Comments";
							}

								if( m_bSort )
								{
									pResponse->strContent += "<br><a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_ACOMMENTS;

									if( !strSearch.empty( ) )
										pResponse->strContent += "&search=" + strSearchResp;

									if( !strFilter.empty( ) )
										pResponse->strContent += "&filter=" + UTIL_StringToEscaped( strFilter );

									// The Trinity Edition - MOD BEGINS - Replaces the Ascending Link Text when using Custom Labels

									if( m_bUseCustomlabels ) {
										pResponse->strContent += "\">\n\n";
										pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
										pResponse->strContent += "if (typeof(linkA) == \"undefined\") {\n";
										pResponse->strContent += "   document.write(\"A\");\n";
										pResponse->strContent += "}\n";
										pResponse->strContent += "else {\n";
										pResponse->strContent += "   document.write(linkA);\n";
										pResponse->strContent += "}\n";
										pResponse->strContent += "</script>\n";
									}
									else {
										pResponse->strContent += "\">A";
									}

									pResponse->strContent += "</a> <a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_DCOMMENTS;

								/* Original Source Code:
								pResponse->strContent += "\">A</a> <a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_DCOMMENTS;
								*/

								// ------------------------------------------------------------- END OF MOD									

									if( !strSearch.empty( ) )
										pResponse->strContent += "&search=" + strSearchResp;

									if( !strFilter.empty( ) )
										pResponse->strContent += "&filter=" + UTIL_StringToEscaped( strFilter );

								// The Trinity Edition - MOD BEGINS - Replaces the Descending Link Text when using Custom Labels

								if( m_bUseCustomlabels ) {
									pResponse->strContent += "\">";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(linkZ) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"Z\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(linkZ);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
									pResponse->strContent += "</a>";
								}
								else {
									pResponse->strContent += "\">Z</a>"; // <<< Original Source Code:
								}

								// ----------------------------------------------- END OF MOD

								}

								pResponse->strContent += "</th>";
							}
						}

						// <th> added

						if( m_pAllowed && m_bShowAdded )
						{
							// Trinity Edition - Modification Begins
							// The following adds the Custom Label Feature
							if( m_bUseCustomlabels ) {
								pResponse->strContent += "<th class=\"date\">\n";
								pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
								pResponse->strContent += "if (typeof(headerAdded) == \"undefined\") {\n";
								pResponse->strContent += "   document.write(\"Added\");\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "else {\n";
								pResponse->strContent += "   document.write(headerAdded);\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "</script>\n";
							}
							else { // Use Original Source Code:
								pResponse->strContent += "<th class=\"date\">Added";
							}
							// ------------------------------------------ END OF MODIFICATION
							
							if( m_bSort )
							{
								pResponse->strContent += "<br><a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_AADDED;

								if( !strSearch.empty( ) )
									pResponse->strContent += "&search=" + strSearchResp;

								if( !strFilter.empty( ) )
									pResponse->strContent += "&filter=" + UTIL_StringToEscaped( strFilter );

								// The Trinity Edition - MOD BEGINS - Replaces the Ascending Link Text when using Custom Labels

								if( m_bUseCustomlabels ) {
									pResponse->strContent += "\">\n\n";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(linkA) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"A\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(linkA);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
								}
								else {
									pResponse->strContent += "\">A";
								}

								pResponse->strContent += "</a> <a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_DADDED;

								/* Original Source Code:
								pResponse->strContent += "\">A</a> <a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_DADDED;
								*/

								// ------------------------------------------------------------- END OF MOD

								if( !strSearch.empty( ) )
									pResponse->strContent += "&search=" + strSearchResp;

								if( !strFilter.empty( ) )
									pResponse->strContent += "&filter=" + UTIL_StringToEscaped( strFilter );

								// The Trinity Edition - MOD BEGINS - Replaces the Descending Link Text when using Custom Labels

								if( m_bUseCustomlabels ) {
									pResponse->strContent += "\">";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(linkZ) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"Z\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(linkZ);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
									pResponse->strContent += "</a>";
								}
								else {
									pResponse->strContent += "\">Z</a>"; // <<< Original Source Code:
								}

								// ----------------------------------------------- END OF MOD

							}

							pResponse->strContent += "</th>";
						}

						// <th> size

						if( m_pAllowed && m_bShowSize )
						{
							// Trinity Edition - Modification Begins
							// The following adds the Custom Label Feature
							if( m_bUseCustomlabels ) {
								pResponse->strContent += "<th class=\"bytes\">\n";
								pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
								pResponse->strContent += "if (typeof(headerSize) == \"undefined\") {\n";
								pResponse->strContent += "   document.write(\"Size\");\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "else {\n";
								pResponse->strContent += "   document.write(headerSize);\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "</script>\n";
							}
							else { // Use Original Source Code:
								pResponse->strContent += "<th class=\"bytes\">Size";
							}
							// ------------------------------------------ END OF MODIFICATION

							if( m_bSort )
							{
								pResponse->strContent += "<br><a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_ASIZE;

								if( !strSearch.empty( ) )
									pResponse->strContent += "&search=" + strSearchResp;

								if( !strFilter.empty( ) )
									pResponse->strContent += "&filter=" + UTIL_StringToEscaped( strFilter );

								// The Trinity Edition - MOD BEGINS - Replaces the Ascending Link Text when using Custom Labels

								if( m_bUseCustomlabels ) {
									pResponse->strContent += "\">\n\n";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(linkA) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"A\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(linkA);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
								}
								else {
									pResponse->strContent += "\">A";
								}

								pResponse->strContent += "</a> <a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_DSIZE;

								/* Original Source Code:
								pResponse->strContent += "\">A</a> <a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_DSIZE;
								*/

								// ------------------------------------------------------------- END OF MOD								

								if( !strSearch.empty( ) )
									pResponse->strContent += "&search=" + strSearchResp;

								if( !strFilter.empty( ) )
									pResponse->strContent += "&filter=" + UTIL_StringToEscaped( strFilter );

								// The Trinity Edition - MOD BEGINS - Replaces the Descending Link Text when using Custom Labels

								if( m_bUseCustomlabels ) {
									pResponse->strContent += "\">";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(linkZ) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"Z\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(linkZ);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
									pResponse->strContent += "</a>";
								}
								else {
									pResponse->strContent += "\">Z</a>"; // <<< Original Source Code:
								}

								// ----------------------------------------------- END OF MOD

							}

							pResponse->strContent += "</th>";
						}

						// <th> files

						if( m_pAllowed && m_bShowNumFiles )
						{
							// Trinity Edition - Modification Begins
							// The following adds the Custom Label Feature
							if( m_bUseCustomlabels ) {
								pResponse->strContent += "<th class=\"number\">\n";
								pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
								pResponse->strContent += "if (typeof(headerFiles) == \"undefined\") {\n";
								pResponse->strContent += "   document.write(\"Files\");\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "else {\n";
								pResponse->strContent += "   document.write(headerFiles);\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "</script>\n";
							}
							else { // Use Original Source Code:							
								pResponse->strContent += "<th class=\"number\">Files";
							}
							// ------------------------------------------ END OF MODIFICATION

							if( m_bSort )
							{
								pResponse->strContent += "<br><a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_AFILES;

								if( !strSearch.empty( ) )
									pResponse->strContent += "&search=" + strSearchResp;

								if( !strFilter.empty( ) )
									pResponse->strContent += "&filter=" + UTIL_StringToEscaped( strFilter );

								// The Trinity Edition - MOD BEGINS - Replaces the Ascending Link Text when using Custom Labels

								if( m_bUseCustomlabels ) {
									pResponse->strContent += "\">\n\n";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(linkA) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"A\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(linkA);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
								}
								else {
									pResponse->strContent += "\">A";
								}

								pResponse->strContent += "</a> <a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_DFILES;

								/* Original Source Code:
								pResponse->strContent += "\">A</a> <a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_DFILES;
								*/

								// ------------------------------------------------------------- END OF MOD									

								if( !strSearch.empty( ) )
									pResponse->strContent += "&search=" + strSearchResp;

								if( !strFilter.empty( ) )
									pResponse->strContent += "&filter=" + UTIL_StringToEscaped( strFilter );

								// The Trinity Edition - MOD BEGINS - Replaces the Descending Link Text when using Custom Labels

								if( m_bUseCustomlabels ) {
									pResponse->strContent += "\">";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(linkZ) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"Z\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(linkZ);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
									pResponse->strContent += "</a>";
								}
								else {
									pResponse->strContent += "\">Z</a>"; // <<< Original Source Code:
								}

								// ----------------------------------------------- END OF MOD
							}

							pResponse->strContent += "</th>";
						}

						// <th> seeders

							// Trinity Edition - Modification Begins
							// The following adds the Custom Label Feature
							if( m_bUseCustomlabels ) {
								pResponse->strContent += "<th class=\"number\">\n";
								pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
								pResponse->strContent += "if (typeof(headerSeeders) == \"undefined\") {\n";
								pResponse->strContent += "   document.write(\"Seeders\");\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "else {\n";
								pResponse->strContent += "   document.write(headerSeeders);\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "</script>\n";
							}
							else { // Use Original Source Code:
								pResponse->strContent += "<th class=\"number\">Seeders";
							}
							// ------------------------------------------ END OF MODIFICATION
						
						if( m_bSort )
						{
							pResponse->strContent += "<br><a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_ACOMPLETE;

							if( !strSearch.empty( ) )
								pResponse->strContent += "&search=" + strSearchResp;

							if( !strFilter.empty( ) )
								pResponse->strContent += "&filter=" + UTIL_StringToEscaped( strFilter );

								// The Trinity Edition - MOD BEGINS - Replaces the Ascending Link Text when using Custom Labels

								if( m_bUseCustomlabels ) {
									pResponse->strContent += "\">\n\n";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(linkA) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"A\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(linkA);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
								}
								else {
									pResponse->strContent += "\">A";
								}

								pResponse->strContent += "</a> <a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_DCOMPLETE;

								/* Original Source Code:
								pResponse->strContent += "\">A</a> <a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_DCOMPLETE;
								*/

								// ------------------------------------------------------------- END OF MOD								

							if( !strSearch.empty( ) )
								pResponse->strContent += "&search=" + strSearchResp;

							if( !strFilter.empty( ) )
								pResponse->strContent += "&filter=" + UTIL_StringToEscaped( strFilter );

								// The Trinity Edition - MOD BEGINS - Replaces the Descending Link Text when using Custom Labels

								if( m_bUseCustomlabels ) {
									pResponse->strContent += "\">";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(linkZ) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"Z\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(linkZ);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
									pResponse->strContent += "</a>";
								}
								else {
									pResponse->strContent += "\">Z</a>"; // <<< Original Source Code:
								}

								// ----------------------------------------------- END OF MOD
						}

						pResponse->strContent += "</th>";

						// <th> leechers
						
							// Trinity Edition - Modification Begins
							// The following adds the Custom Label Feature
							if( m_bUseCustomlabels ) {
								pResponse->strContent += "<th class=\"number\">\n";
								pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
								pResponse->strContent += "if (typeof(headerLeechers) == \"undefined\") {\n";
								pResponse->strContent += "   document.write(\"Leechers\");\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "else {\n";
								pResponse->strContent += "   document.write(headerLeechers);\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "</script>\n";
							}
							else { // Use Original Source Code:
								pResponse->strContent += "<th class=\"number\">Leechers";
							}
							// ------------------------------------------ END OF MODIFICATION
						
						if( m_bSort )
						{
							pResponse->strContent += "<br><a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_AINCOMPLETE;

							if( !strSearch.empty( ) )
								pResponse->strContent += "&search=" + strSearchResp;

							if( !strFilter.empty( ) )
								pResponse->strContent += "&filter=" + UTIL_StringToEscaped( strFilter );

								// The Trinity Edition - MOD BEGINS - Replaces the Ascending Link Text when using Custom Labels

								if( m_bUseCustomlabels ) {
									pResponse->strContent += "\">\n\n";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(linkA) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"A\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(linkA);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
								}
								else {
									pResponse->strContent += "\">A";
								}

								pResponse->strContent += "</a> <a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_DINCOMPLETE;

								/* Original Source Code:
								pResponse->strContent += "\">A</a> <a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_DINCOMPLETE;
								*/

								// ------------------------------------------------------------- END OF MOD	

							if( !strSearch.empty( ) )
								pResponse->strContent += "&search=" + strSearchResp;

							if( !strFilter.empty( ) )
								pResponse->strContent += "&filter=" + UTIL_StringToEscaped( strFilter );

								// The Trinity Edition - MOD BEGINS - Replaces the Descending Link Text when using Custom Labels

								if( m_bUseCustomlabels ) {
									pResponse->strContent += "\">";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(linkZ) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"Z\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(linkZ);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
									pResponse->strContent += "</a>";
								}
								else {
									pResponse->strContent += "\">Z</a>"; // <<< Original Source Code:
								}

								// ----------------------------------------------- END OF MOD
						}

						pResponse->strContent += "</th>";

						// <th> completed

						if( m_bShowCompleted )
						{
							// Trinity Edition - Modification Begins
							// The following adds the Custom Label Feature
							if( m_bUseCustomlabels ) {
								pResponse->strContent += "<th class=\"number\">\n";
								pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
								pResponse->strContent += "if (typeof(headerCompleted) == \"undefined\") {\n";
								pResponse->strContent += "   document.write(\"Completed\");\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "else {\n";
								pResponse->strContent += "   document.write(headerCompleted);\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "</script>\n";
							}
							else { // Use Original Source Code:
								pResponse->strContent += "<th class=\"number\">Completed";
							}
							// ------------------------------------------ END OF MODIFICATION

							if( m_bSort )
							{
								pResponse->strContent += "<br><a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_ACOMPLETED;

								if( !strSearch.empty( ) )
									pResponse->strContent += "&search=" + strSearchResp;

								if( !strFilter.empty( ) )
									pResponse->strContent += "&filter=" + UTIL_StringToEscaped( strFilter );

								// The Trinity Edition - MOD BEGINS - Replaces the Ascending Link Text when using Custom Labels

								if( m_bUseCustomlabels ) {
									pResponse->strContent += "\">\n\n";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(linkA) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"A\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(linkA);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
								}
								else {
									pResponse->strContent += "\">A";
								}

								pResponse->strContent += "</a> <a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_DCOMPLETED;

								/* Original Source Code:
								pResponse->strContent += "\">A</a> <a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_DCOMPLETED;
								*/

								// ------------------------------------------------------------- END OF MOD	

								if( !strSearch.empty( ) )
									pResponse->strContent += "&search=" + strSearchResp;

								if( !strFilter.empty( ) )
									pResponse->strContent += "&filter=" + UTIL_StringToEscaped( strFilter );

								// The Trinity Edition - MOD BEGINS - Replaces the Descending Link Text when using Custom Labels

								if( m_bUseCustomlabels ) {
									pResponse->strContent += "\">";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(linkZ) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"Z\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(linkZ);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
									pResponse->strContent += "</a>";
								}
								else {
									pResponse->strContent += "\">Z</a>"; // <<< Original Source Code:
								}

								// ----------------------------------------------- END OF MOD
							}

							pResponse->strContent += "</th>";
						}

						// <th> transferred

						if( m_pAllowed && m_bShowTransferred )
						{
							// Trinity Edition - Modification Begins
							// The following adds the Custom Label Feature
							if( m_bUseCustomlabels ) {
								pResponse->strContent += "<th class=\"bytes\">\n";
								pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
								pResponse->strContent += "if (typeof(headerTransferred) == \"undefined\") {\n";
								pResponse->strContent += "   document.write(\"Transferred\");\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "else {\n";
								pResponse->strContent += "   document.write(headerTransferred);\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "</script>\n";
							}
							else { // Use Original Source Code:
								pResponse->strContent += "<th class=\"bytes\">Transferred";
							}
							// ------------------------------------------ END OF MODIFICATION

							if( m_bSort )
							{
								pResponse->strContent += "<br><a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_ATRANSFERRED;

								if( !strSearch.empty( ) )
									pResponse->strContent += "&search=" + strSearchResp;

								if( !strFilter.empty( ) )
									pResponse->strContent += "&filter=" + UTIL_StringToEscaped( strFilter );

								// The Trinity Edition - MOD BEGINS - Replaces the Ascending Link Text when using Custom Labels

								if( m_bUseCustomlabels ) {
									pResponse->strContent += "\">\n\n";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(linkA) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"A\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(linkA);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
								}
								else {
									pResponse->strContent += "\">A";
								}

								pResponse->strContent += "</a> <a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_DTRANSFERRED;

								/* Original Source Code:
								pResponse->strContent += "\">A</a> <a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_DTRANSFERRED;
								*/

								// ------------------------------------------------------------- END OF MOD	

								if( !strSearch.empty( ) )
									pResponse->strContent += "&search=" + strSearchResp;

								if( !strFilter.empty( ) )
									pResponse->strContent += "&filter=" + UTIL_StringToEscaped( strFilter );

								// The Trinity Edition - MOD BEGINS - Replaces the Descending Link Text when using Custom Labels

								if( m_bUseCustomlabels ) {
									pResponse->strContent += "\">";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(linkZ) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"Z\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(linkZ);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
									pResponse->strContent += "</a>";
								}
								else {
									pResponse->strContent += "\">Z</a>"; // <<< Original Source Code:
								}

								// ----------------------------------------------- END OF MOD
							}

							pResponse->strContent += "</th>";
						}

						// <th> min left

						if( m_bShowMinLeft )
						{
							if( m_pAllowed && m_bShowLeftAsProgress )
								// Trinity Edition - Modification Begins
								// The following adds the Custom Label Feature
								if( m_bUseCustomlabels ) {
									pResponse->strContent += "<th class=\"percent\">\n";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(headerMinProgress) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"Min Progress\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(headerMinProgress);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
									pResponse->strContent += "</th>\n\n";
								}
								else { // Use Original Source Code:
									pResponse->strContent += "<th class=\"percent\">Min Progress</th>";
								}
							// ------------------------------------------ END OF MODIFICATION
							else
								// Trinity Edition - Modification Begins
								// The following adds the Custom Label Feature
								if( m_bUseCustomlabels ) {
									pResponse->strContent += "<th class=\"percent\">\n";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(headerMinLeft) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"Min Left\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(headerMinLeft);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
									pResponse->strContent += "</th>\n\n";
								}
								else { // Use Original Source Code:
									pResponse->strContent += "<th class=\"percent\">Min Left</th>";
								}
							// ------------------------------------------ END OF MODIFICATION
						}

						// <th> average left

						if( m_bShowAverageLeft )
						{
							if( m_pAllowed && m_bShowLeftAsProgress )
								// Trinity Edition - Modification Begins
								// The following adds the Custom Label Feature
								if( m_bUseCustomlabels ) {
									pResponse->strContent += "<th class=\"percent\">\n";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(headerAverageProgress) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"Average Progress\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(headerAverageProgress);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
								}
								else { // Use Original Source Code:
									pResponse->strContent += "<th class=\"percent\">Average Progress";
								}
							// ------------------------------------------ END OF MODIFICATION
							else
								// Trinity Edition - Modification Begins
								// The following adds the Custom Label Feature
								if( m_bUseCustomlabels ) {
									pResponse->strContent += "<th class=\"percent\">\n";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(headerAverageLeft) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"Average Left\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(headerAverageLeft);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
								}
								else { // Use Original Source Code:
									pResponse->strContent += "<th class=\"percent\">Average Left";
								}
							// ------------------------------------------ END OF MODIFICATION

							if( m_bSort )
							{
								pResponse->strContent += "<br><a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_AAVGLEFT;

								if( !strSearch.empty( ) )
									pResponse->strContent += "&search=" + strSearchResp;

								if( !strFilter.empty( ) )
									pResponse->strContent += "&filter=" + UTIL_StringToEscaped( strFilter );

								// The Trinity Edition - MOD BEGINS - Replaces the Ascending Link Text when using Custom Labels

								if( m_bUseCustomlabels ) {
									pResponse->strContent += "\">\n\n";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(linkA) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"A\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(linkA);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
								}
								else {
									pResponse->strContent += "\">A";
								}

								pResponse->strContent += "</a> <a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_DAVGLEFT;

								/* Original Source Code:
								pResponse->strContent += "\">A</a> <a class=\"sort\" href=\"/index.html?sort=" + SORTSTR_DAVGLEFT;
								*/

								// ------------------------------------------------------------- END OF MOD	

								if( !strSearch.empty( ) )
									pResponse->strContent += "&search=" + strSearchResp;

								if( !strFilter.empty( ) )
									pResponse->strContent += "&filter=" + UTIL_StringToEscaped( strFilter );

								// The Trinity Edition - MOD BEGINS - Replaces the Descending Link Text when using Custom Labels

								if( m_bUseCustomlabels ) {
									pResponse->strContent += "\">";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(linkZ) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"Z\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(linkZ);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
									pResponse->strContent += "</a>";
								}
								else {
									pResponse->strContent += "\">Z</a>"; // <<< Original Source Code:
								}

								// ----------------------------------------------- END OF MOD
							}

							pResponse->strContent += "</th>";
						}

						// <th> maxi left

						if( m_bShowMaxiLeft )
						{
							if( m_pAllowed && m_bShowLeftAsProgress )
								// Trinity Edition - Modification Begins
								// The following adds the Custom Label Feature
								if( m_bUseCustomlabels ) {
									pResponse->strContent += "<th class=\"percent\">\n";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(headerMaxProgress) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"Max Progress\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(headerMaxProgress);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
									pResponse->strContent += "</th>\n\n";
								}
								else { // Use Original Source Code:
									pResponse->strContent += "<th class=\"percent\">Max Progress</th>";
								}
							// ------------------------------------------ END OF MODIFICATION
							else
								// Trinity Edition - Modification Begins
								// The following adds the Custom Label Feature
								if( m_bUseCustomlabels ) {
									pResponse->strContent += "<th class=\"percent\">\n";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(headerMaxLeft) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"Max Left\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(headerMaxLeft);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
									pResponse->strContent += "</th>\n\n";
								}
								else { // Use Original Source Code:
									pResponse->strContent += "<th class=\"percent\">Max Left</th>";
								}
							// ------------------------------------------ END OF MODIFICATION
						}

						// <th> uploader

						if( m_bShowUploader )
							// Trinity Edition - Modification Begins
							// The following adds the Custom Label Feature
							if( m_bUseCustomlabels ) {
								pResponse->strContent += "<th class=\"name\">\n";
								pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
								pResponse->strContent += "if (typeof(headerUploader) == \"undefined\") {\n";
								pResponse->strContent += "   document.write(\"Uploader\");\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "else {\n";
								pResponse->strContent += "   document.write(headerUploader);\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "</script>\n";
								pResponse->strContent += "</th>\n\n";
							}
							else { // Use Original Source Code:
								pResponse->strContent += "<th class=\"name\">Uploader</th>";
							}
							// ------------------------------------------ END OF MODIFICATION

						// <th> info link

						if( m_bAllowInfoLink )
							// Trinity Edition - Modification Begins
							// The following adds the Custom Label Feature
							if( m_bUseCustomlabels ) {
								pResponse->strContent += "<th class=\"infolink\">\n";
								pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
								pResponse->strContent += "if (typeof(headerInfoLink) == \"undefined\") {\n";
								pResponse->strContent += "   document.write(\"Info Link\");\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "else {\n";
								pResponse->strContent += "   document.write(headerInfoLink);\n";
								pResponse->strContent += "}\n";
								pResponse->strContent += "</script>\n";
								pResponse->strContent += "</th>\n\n";
							}
							else { // Use Original Source Code:
								pResponse->strContent += "<th class=\"infolink\">Info Link</th>";
							}
							// ------------------------------------------ END OF MODIFICATION

						// <th> admin

						if( user.iAccess & ACCESS_EDIT )
						{
							if( m_pAllowed )
								// Trinity Edition - Modification Begins
								// The following adds the Custom Label Feature
								// Added th.admin class
								if( m_bUseCustomlabels ) {
									pResponse->strContent += "<th class=\"admin\">\n";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(headerAdmin) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"Admin\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(headerAdmin);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
									pResponse->strContent += "</th>\n\n";
								}
								else { // Use Original Source Code WITH ADDED CLASS:
									pResponse->strContent += "<th class=\"admin\">Admin</th>";
								}
							// ------------------------------------------ END OF MODIFICATION
						}

						pResponse->strContent += "</tr>\n";

						bFound = true;
					}

					if( iSkipped == (int)iStart )
					{
						// output table rows

						if( iAdded % 2 )
							pResponse->strContent += "<tr class=\"even\">";
						else
							pResponse->strContent += "<tr class=\"odd\">";

						// <td> tag

						if( !m_vecTags.empty( ) )
						{
							string strTemp = pTorrents[i].strTag;

							for( vector< pair<string, string> > :: iterator j = m_vecTags.begin( ); j != m_vecTags.end( ); j++ )
							{
								// The Trinity Edition - Modification Begins
									
								if( (*j).first == pTorrents[i].strTag && !(*j).second.empty( ) ) {

										/* The following removes the border around the tag image
										that normally displays when the image is hyperlinked...
										which it will be because of the new LINK CODE FOR TAG FILTERS below
										*/

										pTorrents[i].strTag = "<img border=\"0\" src=\"" + (*j).second + "\" title=\"" + (*j).first + "\">";
										
										/* Future OnMouseOver/Out Implimentation for the Tag Column Filters

										------ NOT YET IMPLIMENTED ------

										if( m_bUseMouseovers ) { // WILL MOST LIKELY CHANGE CONFIG KEY TO "trinity_use_mouseovers_ontags"
											pTorrents[i].strTag = "<img src=\"" + (*j).second + "\" border=\"0\" title=\"" + (*j).first + "\" name=\"" + (*j).first + "\" onMouseOver=\"hoverOn" + (*i).first + "()\" onMouseOut=\"hoverOff" + (*j).first + "()\"; return true;>";
										}
										else {
											pTorrents[i].strTag = "<img src=\"" + (*j).second + "\" border=\"0\" title=\"" + (*j).first + "\" name=\"" + (*j).first + "\">";
										}

										*/

								}

								/* Original Source Code:
								if( (*j).first == pTorrents[i].strTag && !(*j).second.empty( ) )
									pTorrents[i].strTag = "<img src=\"" + (*j).second + "\">";
								*/

								// ----------------------------------------------- END OF MODIFICATIONS
							}

							if( pTorrents[i].strTag == strTemp )
								pTorrents[i].strTag = UTIL_RemoveHTML( pTorrents[i].strTag );

							// Trinity Edition - Modification Begins

							/* The following allows the Tag/Category images in the far left column
							to act as an additional filter, functioning the same as the one at
							the top of the Torrent Table... SPECIAL THANKS to DreadWingKnight */
							
							pResponse->strContent += "<td class=\"tag\">";

							// Link code for tag Filters
							pResponse->strContent += "<a href=\"/index.html?filter=" + UTIL_StringToEscaped( strTemp );

							if( !strSort.empty( ) )
								pResponse->strContent += "&sort=" + strSort;

							if( !strSearch.empty( ) )
								pResponse->strContent += "&search=" + strSearchResp;

							pResponse->strContent += "\">";
							// End link code for tag Filters
							pResponse->strContent += pTorrents[i].strTag;
							pResponse->strContent += "</a>";
							pResponse->strContent += "</td>";

							/* Original Source Code:
							pResponse->strContent += "<td>" + pTorrents[i].strTag + "</td>";
							*/

							// ------------------------------------------------------ END OF MODIFICATION
						}

						// <td> info hash

						if( m_bShowInfoHash )
						{
							pResponse->strContent += "<td class=\"hash\">";

							if( m_bShowStats )
								pResponse->strContent += "<a class=\"hash\" href=\"/stats.html?info_hash=" + UTIL_HashToString( pTorrents[i].strInfoHash ) + "\">";

							pResponse->strContent += UTIL_HashToString( pTorrents[i].strInfoHash );

							if( m_bShowStats )
								pResponse->strContent += "</a>";

							pResponse->strContent += "</td>";
						}

						// <td> name

						if( m_pAllowed && m_bShowNames )
						{
							pResponse->strContent += "<td class=\"name\">";

							if( m_bShowStats )
								pResponse->strContent += "<a class=\"stats\" href=\"/stats.html?info_hash=" + UTIL_HashToString( pTorrents[i].strInfoHash ) + "\">";

							pResponse->strContent += UTIL_RemoveHTML( pTorrents[i].strName );

							if( m_bShowStats )
								pResponse->strContent += "</a>";

							pResponse->strContent += "</td>";
						}

						// <td> torrent

						if( m_pAllowed && m_bAllowTorrentDownloads && ( user.iAccess & ACCESS_DL ) )
						{
							// The Trinity Edition - Modification Begins

							// The following adds brackets around thepTorrents[i].strTag Torrent Download Link

							pResponse->strContent += "<td class=\"download\"><nobr>[<a class=\"download\" href=\"";
							
							/* Original Source Code:
							pResponse->strContent += "<td class=\"download\"><a class=\"download\" href=\"";
							*/

							if( m_strExternalTorrentDir.empty( ) )
								pResponse->strContent += "/torrent.html?info_hash=" + UTIL_HashToString( pTorrents[i].strInfoHash );
							else
								pResponse->strContent += m_strExternalTorrentDir + UTIL_StringToEscapedStrict( pTorrents[i].strFileName );

								// The following adds the Custom Label Feature

								if( m_bUseCustomlabels ) {
									pResponse->strContent += "\">\n";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(linkDownload) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"DL\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(linkDownload);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
									pResponse->strContent += "</a>]</nobr></td>\n\n";
								}
								else { // Use Original Source Code WITH ADDED BRACKET:
									pResponse->strContent += "\">DL</a>]</nobr></td>";
								}

							/* Original Source Code:
							pResponse->strContent += "\">DL</a></td>";
							*/

							// ------------------------------------------------------------- END OF MODIFICATION
						}

						// <td> comments

						if( m_pAllowed && m_bAllowComments )
						{
							if( m_pComments )
								pResponse->strContent += "<td class=\"number\"><a href=\"/comments.html?info_hash=" + UTIL_HashToString( pTorrents[i].strInfoHash ) + "\">" + CAtomInt( pTorrents[i].iComments ).toString( ) + "</a></td>";
						}

						// <td> added

						if( m_pAllowed && m_bShowAdded )
						{
							pResponse->strContent += "<td class=\"date\">";

							if( !pTorrents[i].strAdded.empty( ) )
							{
								// strip year and seconds from time

								pResponse->strContent += pTorrents[i].strAdded.substr( 5, pTorrents[i].strAdded.size( ) - 8 );
							}

							pResponse->strContent += "</td>";
						}

						// <td> size

						if( m_pAllowed && m_bShowSize )
							pResponse->strContent += "<td class=\"bytes\">" + UTIL_BytesToString( pTorrents[i].iSize ) + "</td>";

						// <td> files

						if( m_pAllowed && m_bShowNumFiles )
							pResponse->strContent += "<td class=\"number\">" + CAtomInt( pTorrents[i].iFiles ).toString( ) + "</td>";

						// <td> seeders

						pResponse->strContent += "<td class=\"number_";

						if( pTorrents[i].iComplete == 0 )
							pResponse->strContent += "red\">";
						else if( pTorrents[i].iComplete < 5 )
							pResponse->strContent += "yellow\">";
						else
							pResponse->strContent += "green\">";

						pResponse->strContent += CAtomInt( pTorrents[i].iComplete ).toString( ) + "</td>";

						// <td> leechers

						pResponse->strContent += "<td class=\"number_";

						if( pTorrents[i].iDL == 0 )
							pResponse->strContent += "red\">";
						else if( pTorrents[i].iDL < 5 )
							pResponse->strContent += "yellow\">";
						else
							pResponse->strContent += "green\">";

						pResponse->strContent += CAtomInt( pTorrents[i].iDL ).toString( ) + "</td>";

						// <td> completed

						if( m_bShowCompleted )
							pResponse->strContent += "<td class=\"number\">" + CAtomInt( pTorrents[i].iCompleted ).toString( ) + "</td>";

						// <td> transferred

						if( m_pAllowed && m_bShowTransferred )
							pResponse->strContent += "<td class=\"bytes\">" + UTIL_BytesToString( pTorrents[i].iTransferred ) + "</td>";

						// <td> min left

						if( m_bShowMinLeft )
						{
							pResponse->strContent += "<td class=\"percent\">";

							if( pTorrents[i].iDL == 0 )
								pResponse->strContent += "N/A";
							else
							{
								if( m_pAllowed )
								{
									int iPercent = 0;

									if( pTorrents[i].iSize > 0 )
									{
										if( m_bShowLeftAsProgress )
											iPercent = 100 - (int)( ( (float)pTorrents[i].iMaxiLeft / pTorrents[i].iSize ) * 100 );
										else
											iPercent = (int)( ( (float)pTorrents[i].iMinLeft / pTorrents[i].iSize ) * 100 );
									}

									pResponse->strContent += CAtomInt( iPercent ).toString( ) + "%</td>";
								}
								else
									pResponse->strContent += UTIL_BytesToString( pTorrents[i].iMinLeft );
							}
						}

						// <td> average left

						if( m_bShowAverageLeft )
						{
							pResponse->strContent += "<td class=\"percent\">";

							if( pTorrents[i].iDL == 0 )
								pResponse->strContent += "N/A";
							else
							{
								if( m_pAllowed )
								{
									int iPercent;

									if( m_bShowLeftAsProgress )
										iPercent = 100 - pTorrents[i].iAverageLeftPercent;
									else
										iPercent = pTorrents[i].iAverageLeftPercent;

									pResponse->strContent += CAtomInt( iPercent ).toString( ) + "%";

									if( !m_strImageBarFill.empty( ) && !m_strImageBarTrans.empty( ) )
									{
										pResponse->strContent += "<br>";

										if( iPercent > 0 )
											pResponse->strContent += "<img src=\"" + m_strImageBarFill + "\" width=" + CAtomInt( iPercent ).toString( ) + " height=8>";

										if( iPercent < 100 )
											pResponse->strContent += "<img src=\"" + m_strImageBarTrans + "\" width=" + CAtomInt( 100 - iPercent ).toString( ) + " height=8>";
									}
								}
								else
									pResponse->strContent += UTIL_BytesToString( pTorrents[i].iAverageLeft );

								pResponse->strContent += "</td>";
							}
						}

						// <td> maxi left

						if( m_bShowMaxiLeft )
						{
							pResponse->strContent += "<td class=\"percent\">";

							if( pTorrents[i].iDL == 0 )
								pResponse->strContent += "N/A";
							else
							{
								if( m_pAllowed )
								{
									int iPercent = 0;

									if( pTorrents[i].iSize > 0 )
									{
										if( m_bShowLeftAsProgress )
											iPercent = 100 - (int)( ( (float)pTorrents[i].iMinLeft / pTorrents[i].iSize ) * 100 );
										else
											iPercent = (int)( ( (float)pTorrents[i].iMaxiLeft / pTorrents[i].iSize ) * 100 );
									}

									pResponse->strContent += CAtomInt( iPercent ).toString( ) + "%</td>";
								}
								else
									pResponse->strContent += UTIL_BytesToString( pTorrents[i].iMaxiLeft );
							}
						}

						// <td> uploader

						if( m_bShowUploader )
							pResponse->strContent += "<td class=\"name\">" + UTIL_RemoveHTML( pTorrents[i].strUploader ) + "</td>";

						// <td> info link

						if( m_bAllowInfoLink )
						{
							pResponse->strContent += "<td class=\"infolink\">";

							// The Trinity Edition - Modification Begins

							/* The following adds the Custom Label Feature and also sets 
							the target for InfoLinks to a new browser window */

							if( !pTorrents[i].strInfoLink.empty( ) ) {
								
								if( m_bUseCustomlabels ) {
									pResponse->strContent += "[ <a target=\"_blank\" href=\"" + UTIL_RemoveHTML( pTorrents[i].strInfoLink ) + "\">\n\n";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(linkInfoLink) == \"undefined\") {\n";
									pResponse->strContent += "   document.write(\"Link\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "   document.write(linkInfoLink);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n";
									pResponse->strContent += "</a> ]";
								}
								else { // Use Original Source Code WITH ADDED TARGET:
									pResponse->strContent += "[ <a target=\"_blank\" href=\"" + UTIL_RemoveHTML( pTorrents[i].strInfoLink ) + "\">Link</a> ]";
								}

							}

							/* Original Source Code:
							if( !pTorrents[i].strInfoLink.empty( ) )
								pResponse->strContent += "<a href=\"" + UTIL_RemoveHTML( pTorrents[i].strInfoLink ) + "\">Link</a>";
							*/

							// ----------------------------------------------------------- END OF MODIFICATION

							pResponse->strContent += "</td>";
						}

						// <td> admin

						if( user.iAccess & ACCESS_EDIT )
						{
							if( m_pAllowed )
								// The Trinity Edition - Modification Begins
								// The following adds the Custom Label Feature to the DELETE LINK
								if ( m_bUseCustomlabels ) {
                                    pResponse->strContent += "<td><nobr>[<a href=\"/index.html?del=" + UTIL_HashToString( pTorrents[i].strInfoHash ) + "\">\n";
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
									pResponse->strContent += "if (typeof(linkDelete) == \"undefined\") {\n";
									pResponse->strContent += "    document.write(\"Delete\");\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "else {\n";
									pResponse->strContent += "    document.write(linkDelete);\n";
									pResponse->strContent += "}\n";
									pResponse->strContent += "</script>\n\n";
									pResponse->strContent += "</a>]</nobr></td>";
								}
								else { // Use Original Source Code WITH SPACES ADDED BETWEEN BRACKETS AND LINK TEXT:
									pResponse->strContent += "<td><nobr>[ <a href=\"/index.html?del=" + UTIL_HashToString( pTorrents[i].strInfoHash ) + "\">Delete</a> ]</nobr></td>";
								}

							/* Original Source Code:
							pResponse->strContent += "<td>[<a href=\"/index.html?del=" + UTIL_HashToString( pTorrents[i].strInfoHash ) + "\">Delete</a>]</td>";
							*/
						}

						pResponse->strContent += "</tr>\n";

						iAdded++;
					}
					else
						iSkipped++;
				}
			}

			delete [] pTorrents;

			// some finishing touches

			if( bFound )
				pResponse->strContent += "</table>\n";

            // The Trinity Edition - Additions Begin

			pResponse->strContent += "</div>\n\n"; // END THE TABLE DIV ELEMENT

			// The following adds an internal anchor for the PageNavJumpLink internal document link set earlier in the document

			pResponse->strContent += "<a name=\"search\"></a>\n";

			// ------------------------------------------------------------ END OF ADDITIONS

			if( m_pAllowed && m_bShowNames && m_bSearch )
			{
				// The Trinity Edition - Modification Begins:
				// The following specifies an inline display 
				// for the form to remove extra unwanted space on top and bottom.
				// Also added identical span class to form tag for redundancy

				pResponse->strContent += "<span class=\"search_index\"><form style=\"display:inline\" class=\"search_index\" method=\"get\" action=\"/index.html\">\n";

				/* Original Source Code:
				pResponse->strContent += "<span class=\"search_index\"><form method=\"get\" action=\"/index.html\">\n";
				*/

				// ---------------------------------------- END OF MODIFICATION

				if( !strSort.empty( ) )
					pResponse->strContent += "<input name=\"sort\" type=hidden value=\"" + strSort + "\">\n";

				if( !strFilter.empty( ) )
					pResponse->strContent += "<input name=\"filter\" type=hidden value=\"" + strFilter + "\">\n";

				// The Trinity Edition - Modifications Begin

				// The following adds the Custom Label Feature to the form field label "Search" and 
				// changes the "Clear Search" link into a button, whose label can also be customized.
				// <BR> tags are also inserted to further separate the Search form from the Torrent Table
				// Added Go button for Search Form

					if( m_bUseCustomlabels ) {
						pResponse->strContent += "<br><br>\n";
						pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
						pResponse->strContent += "if (typeof(labelSearch) == \"undefined\") {\n";
						pResponse->strContent += "   document.write(\"Search\");\n";
						pResponse->strContent += "}\n";
						pResponse->strContent += "else {\n";
						pResponse->strContent += "   document.write(labelSearch);\n";
						pResponse->strContent += "}\n";
						pResponse->strContent += "</script>\n";
						pResponse->strContent += "<input name=\"search\" type=text size=40>\n";
						pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
						pResponse->strContent += "if (typeof(buttonGo) == \"undefined\") {\n";
						pResponse->strContent += "   document.write(\"<input class=\\\"go\\\" type=\\\"submit\\\" value=\\\"Go\\\">\\n\");\n";
						pResponse->strContent += "}\n";
						pResponse->strContent += "else {\n";
						pResponse->strContent += "   document.write(\"<input class=\\\"go\\\" type=\\\"submit\\\" value=\\\"\" + buttonGo + \"\\\">\\n\");\n";
						pResponse->strContent += "}\n";
						pResponse->strContent += "if (typeof(buttonClearSearch) == \"undefined\") {\n";
						pResponse->strContent += "   document.write(\"<input class=\\\"clearsearch\\\" type=\\\"button\\\" value=\\\"Clear Search\\\" onClick=\\\"window.location=\'/index.html\'\\\">\");\n";
						pResponse->strContent += "}\n";
						pResponse->strContent += "else {\n";
						pResponse->strContent += "   document.write(\"<input class=\\\"clearsearch\\\" type=\\\"button\\\" value=\\\"\" + buttonClearSearch + \"\\\" onClick=\\\"window.location=\'/index.html\'\\\">\");\n";
						pResponse->strContent += "}\n";
						pResponse->strContent += "</script>\n\n";

					}
					else { // Use Original Source Code WITH <BR> TAGS AND BUTTON CODE:
						pResponse->strContent += "<br><br>Search <input name=\"search\" type=text size=40>\n";
						pResponse->strContent += "<input class=\"go\" type=\"submit\" value=\"Go\">\n";
						pResponse->strContent += "<input class=\"clearsearch\" type=\"button\" value=\"Clear Search\" onClick=\"window.location=\'/index.html\'\"></p>\n";
					}

				/* Original Source Code:
				pResponse->strContent += "Search <input name=\"search\" type=text size=40> <a href=\"/index.html\">Clear Search</a>\n";
				*/

				// -------------------------------------------------------------------- END OF MODIFICATIONS

				pResponse->strContent += "</form></span>\n";
			}

			// page numbers

			if( m_iPerPage > 0 )
			{
				pResponse->strContent += "<p class=\"pagenum_bottom\">";

				// The Trinity Edition - Additions Begin

				/* The following adds the ability to use the Custom Label Feature
				to add text to the beginning of the Page Navigation Links */

				if ( m_bUseCustomlabels ) {
					pResponse->strContent += "\n";
					pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
					pResponse->strContent += "if (typeof(labelPageNavPrefix) != \"undefined\") {\n";
					pResponse->strContent += "   document.write(labelPageNavPrefix);\n";
					pResponse->strContent += "}\n";
					pResponse->strContent += "</script>\n";
				}

				for( unsigned long i = 0; i < (unsigned int)iFound; i += m_iPerPage )
				{
					pResponse->strContent += " ";

					// don't link to current page

					if( i != iStart )
					{
						pResponse->strContent += "<a href=\"/index.html?page=" + CAtomInt( i / m_iPerPage ).toString( );

						if( !strSort.empty( ) )
							pResponse->strContent += "&sort=" + strSort;

						if( !strSearch.empty( ) )
							pResponse->strContent += "&search=" + strSearchResp;

						if( !strFilter.empty( ) )
							pResponse->strContent += "&filter=" + UTIL_StringToEscaped( strFilter );

						pResponse->strContent += "\">";
					}

					pResponse->strContent += CAtomInt( ( i / m_iPerPage ) + 1 ).toString( );

					if( i != iStart )
						pResponse->strContent += "</a>";

					pResponse->strContent += " ";

					// don't display a bar after the last page

					if( i + (unsigned int)m_iPerPage < (unsigned int)iFound )
						pResponse->strContent += "|";
				}

				pResponse->strContent += "</p>\n";
			}
		}

		// The Trinity Edition - Addition Begins

		// The following ends the <div class="index"> set earlier in the document

		pResponse->strContent += "</div>\n";

		// -------------------------------------------- END OF ADD

		pResponse->strContent += m_strStaticFooter;

		// don't even think about removing this :)

		// The Trinity Edition - Modification Begins
		// The following modifies the displayed Tracker Version String

		pResponse->strContent += "<hr>\n";
		// pResponse->strContent += "<p align=\"center\">BNBT EasyTracker</p>\n";
        pResponse->strContent += "<p align=\"center\">\n";
		pResponse->strContent += "POWERED BY <a href=\"http://bnbteasytracker.sourceforge.net\" target=\"_blank\" ";

		pResponse->strContent += "onMouseOver=\"window.status=\'The Trinity Edition of BNBT - Build 7.7r3.2004.10.27 - Click NOW to visit the SourceForge Project Homepage for this tracker software.\'; return true\" "; // BUILD INFORMATION

		pResponse->strContent += "onMouseOut=\"window.status=window.defaultStatus; return true\">\n";

		pResponse->strContent += "The Trinity Edition of BNBT " + string( BNBT_VER ) + "</a></p>\n";

		/* Original Source Code:
		pResponse->strContent += "<p align=\"center\">POWERED BY BNBT " + string( BNBT_VER ) + "</p>\n";
		*/

		// ------------------------------------------------- End of Modification
	}
	else
	{
		pResponse->strContent += "<p class=\"denied\">You are not authorized to view this page.";

		if( user.iAccess & ACCESS_SIGNUP )
			pResponse->strContent += " Click <a href=\"/signup.html\">here</a> to sign up for an account!";

		pResponse->strContent += "</p>\n";
	}

	if( m_bGen )
		// The Trinity Edition - Modification Begins
		// The following modifies the CSS class "gen" to "gen_index" 
		// so that differing styles (i.e. horizontal alignment) can
		// be applied to JUST the Generation Time that is displayed
		// on the tracker index

		pResponse->strContent += "<p class=\"gen_index\">Generated in " + UTIL_ElapsedTimeStr( btv, UTIL_CurrentTime( ) ) + " seconds.</p>\n";

		/* Original Source Code:
		pResponse->strContent += "<p class=\"gen\">Generated in " + UTIL_ElapsedTimeStr( btv, UTIL_CurrentTime( ) ) + " seconds.</p>\n";
		*/

		// ------------------------------------------------- END OF MODIFICATION

	pResponse->strContent += "</body>\n";
	pResponse->strContent += "</html>\n";
}
