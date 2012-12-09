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

void CTracker :: serverResponseComments( struct request_t *pRequest, struct response_t *pResponse, user_t user )
{
	struct bnbttv btv = UTIL_CurrentTime( );

	if( !m_pAllowed || !m_bAllowComments )
	{
		pResponse->strCode = "403 Forbidden";

		return;
	}

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
		pResponse->strContent += "<title>" + gstrTrackerTitle + " &mdash; User and Torrent File Comments</title>\n";
	}
	else {
		pResponse->strContent += "<title>User and Torrent File Comments</title>\n";
	}
	
	/* Original Source Code:
	pResponse->strContent += "<title>BNBT Comments</title>\n";
	*/

	// --------------------------------- END OF MODIFICATION

	if( !gstrStyle.empty( ) )
		pResponse->strContent += "<link rel=stylesheet type=\"text/css\" href=\"" + gstrStyle + "\">\n";

	pResponse->strContent += "</head>\n";

	// assorted scripts (thanks SA)

	pResponse->strContent += "<script language=\"javascript\">\n";
	pResponse->strContent += "function validate( theform ) {\n";
	pResponse->strContent += "if( theform.comment.value == \"\" ) {\n";
	pResponse->strContent += "	alert( \"You must fill in all the fields.\" );\n";
	pResponse->strContent += "	return false; }\n";
	pResponse->strContent += "if( theform.comment.value.length > " + CAtomInt( m_iCommentLength ).toString( ) + " ) {\n";
	pResponse->strContent += "	alert( \"Your message is too long.\\nReduce your message to " + CAtomInt( m_iCommentLength ).toString( ) + " characters.\\nIt is currently \" + theform.comment.value.length + \" characters long.\" );\n";
	pResponse->strContent += "	return false; }\n";
	pResponse->strContent += "else { return true; }\n";
	pResponse->strContent += "}\n";
	pResponse->strContent += "function checklength( theform ) {\n";
	pResponse->strContent += "alert( \"Your message is \" + theform.comment.value.length + \" characters long.\" );\n";
	pResponse->strContent += "}\n";
	pResponse->strContent += "</script>\n";
	pResponse->strContent += "<body>\n";

	// The Trinity Edition - NAVBAR

if( m_bUseNavbar ) {

	pResponse->strContent += "<table class=\"navbar\" width=\"100%\"><tr class=\"navbar\"><td class=\"navbar\">\n\n";

	pResponse->strContent += "<input class=\"rtt\" type=\"button\" value=\"Return To Tracker\" onClick=\"window.location=\'/index.html\'\">\n";
	pResponse->strContent += "<input class=\"back\" type=\"button\" value=\"&laquo; Back\" onClick=\"window.history.back()\">\n\n";

	pResponse->strContent += "</td></tr></table>\n\n";

}

	// -------------------------------- END OF ADDITION

// The Trinity Edition - Modification Begins
// Removes Login and Logged In Message when using the NAVBAR

if( !m_bUseNavbar ) {

	if( user.strLogin.empty( ) )
		pResponse->strContent += "<p class=\"login1_comments\">You are not logged in. Click <a href=\"/login.html\">here</a> to login.</p>\n";
	else
		pResponse->strContent += "<p class=\"login2_comments\">You are logged in as <span class=\"username\">" + UTIL_RemoveHTML( user.strLogin ) + "</span>. Click <a href=\"/login.html?logout=1\">here</a> to logout.</p>\n";

}

	/* Original Source Code:
	if( user.strLogin.empty( ) )
		pResponse->strContent += "<p class=\"login1_comments\">You are not logged in. Click <a href=\"/login.html\">here</a> to login.</p>\n";
	else
		pResponse->strContent += "<p class=\"login2_comments\">You are logged in as <span class=\"username\">" + UTIL_RemoveHTML( user.strLogin ) + "</span>. Click <a href=\"/login.html?logout=1\">here</a> to logout.</p>\n";
	*/

// ------------------------------------------------- END OF MODIFICATION

	// The Trinity Edition - Modification Begins
	// Changed Header of Page
	
	pResponse->strContent += "<h3>Torrent Details</h3>\n";
	
	/* Original Source Code:
	pResponse->strContent += "<h3>BNBT Comments</h3>\n";
	*/

	// --------------------------------- END OF MODIFICATION

	if( user.iAccess & ACCESS_VIEW )
	{
		string strHashString = pRequest->mapParams["info_hash"];
		string strHash = UTIL_StringToHash( strHashString );

		if( !strHash.empty( ) )
		{
			//
			// delete comment
			//

			if( user.iAccess & ACCESS_EDIT )
			{
				string strDelAll = pRequest->mapParams["delall"];
				string strDel = pRequest->mapParams["del"];

				if( strDelAll == "1" )
				{
					m_pComments->delItem( strHash );

					saveComments( );

					pResponse->strContent += "<p>Deleted all comments. Click <a href=\"/comments.html?info_hash=" + strHashString + "\">here</a> to return to the comments page.</p>\n";

						// The Trinity Edition - Addition Begins

						// The following displays a JavaScript alert when all comments are deleted
						// and automatically returns the user to the comments display

						pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
						pResponse->strContent += "alert('All comments have been deleted.');\n";
						pResponse->strContent += "window.location='/comments.html?info_hash=" + strHashString + "';\n";
						pResponse->strContent += "</script>\n\n";

						// ---------------------------------------- END OF ADDITION

					if( m_bGen )
						pResponse->strContent += "<p class=\"gen\">Generated in " + UTIL_ElapsedTimeStr( btv, UTIL_CurrentTime( ) ) + " seconds.</p>\n";

					pResponse->strContent += "</body>\n";
					pResponse->strContent += "</html>\n";

					return;
				}
				else if( !strDel.empty( ) )
				{
					int iDel = atoi( strDel.c_str( ) ) - 1;

					CAtom *pComments = m_pComments->getItem( strHash );

					if( pComments && pComments->isList( ) )
					{
						vector<CAtom *> vecComments = ( (CAtomList *)pComments )->getValue( );

						if( iDel >= 0 && (unsigned int)iDel < vecComments.size( ) )
						{
							( (CAtomList *)pComments )->delItem( vecComments[iDel] );

							saveComments( );

							pResponse->strContent += "<p>Deleted comment " + strDel + ". Click <a href=\"/comments.html?info_hash=" + strHashString + "\">here</a> to return to the comments page.</p>\n";

						// The Trinity Edition - Addition Begins

						// The following displays a JavaScript alert when a comment is deleted
						// and automatically returns the user to the comments display

						pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
						pResponse->strContent += "alert('Comment #" + strDel + " has been deleted.');\n";
						pResponse->strContent += "window.location='/comments.html?info_hash=" + strHashString + "';\n";
						pResponse->strContent += "</script>\n\n";

						// ---------------------------------------- END OF ADDITION

							if( m_bGen )
								pResponse->strContent += "<p class=\"gen\">Generated in " + UTIL_ElapsedTimeStr( btv, UTIL_CurrentTime( ) ) + " seconds.</p>\n";

							pResponse->strContent += "</body>\n";
							pResponse->strContent += "</html>\n";

							return;
						}
						else
						{
							pResponse->strContent += "<p>Unable to delete comment " + strDel + ". The comment number is invalid. Click <a href=\"/comments.html?info_hash=" + strHashString + "\">here</a> to return to the comments page.</p>\n";

							if( m_bGen )
								pResponse->strContent += "<p class=\"gen\">Generated in " + UTIL_ElapsedTimeStr( btv, UTIL_CurrentTime( ) ) + " seconds.</p>\n";

							pResponse->strContent += "</body>\n";
							pResponse->strContent += "</html>\n";

							return;
						}
					}
				}
			}

			// display torrent information list

			if( m_pAllowed )
			{
				CAtom *pTorrent = m_pAllowed->getItem( strHash );

				if( pTorrent && pTorrent->isList( ) )
				{
					vector<CAtom *> vecTorrent = ( (CAtomList *)pTorrent )->getValue( );

					if( vecTorrent.size( ) == 6 )
					{
						CAtom *pName = vecTorrent[1];
						CAtom *pAdded = vecTorrent[2];
						CAtom *pSize = vecTorrent[3];
						CAtom *pFiles = vecTorrent[4];
						CAtom *pComment = vecTorrent[5];

						pResponse->strContent += "<p>File Information</p>\n";
						pResponse->strContent += "<ul>\n";

						if( pName )
							pResponse->strContent += "<li><strong>Name:</strong> " + UTIL_RemoveHTML( pName->toString( ) ) + "</li>\n";

						pResponse->strContent += "<li><strong>Info Hash:</strong> " + strHashString + "</li>\n";

						if( pAdded )
							pResponse->strContent += "<li><strong>Added:</strong> " + pAdded->toString( ) + "</li>\n";

						if( pSize && pSize->isLong( ) )
							pResponse->strContent += "<li><strong>Size:</strong> " + UTIL_BytesToString( ( (CAtomLong *)pSize )->getValue( ) ) + "</li>\n";

						if( pFiles && pFiles->isInt( ) )
							pResponse->strContent += "<li><strong>Files:</strong> " + pFiles->toString( ) + "</li>\n";

						pResponse->strContent += "</ul>\n";

						if( pComment )
						{
							if( m_bShowFileComment )
							{
								// The Trinity Edition - Modification Begins
								// Adds some text to "File Comment"

								pResponse->strContent += "<p>Torrent File Comment</p>\n";

								/* Original Source Code:
								pResponse->strContent += "<p>File Comment</p>\n";
								*/

								// ---------------------------------------------- END OF MODIFICATION

								pResponse->strContent += "<table summary=\"file comment\">\n";
								pResponse->strContent += "<tr><td class=\"com_body\"><code>" + UTIL_RemoveHTML( pComment->toString( ) ) + "</code></td></tr>\n";
								pResponse->strContent += "</table>\n";
							}
						}
					}
				}
			}

			if( !m_pComments->getItem( strHash ) )
				m_pComments->setItem( strHash, new CAtomList( ) );

			CAtom *pComments = m_pComments->getItem( strHash );

			if( pComments && pComments->isList( ) )
			{
				if( user.iAccess & ACCESS_COMMENTS )
				{
					if( pRequest->mapParams.find( "comment" ) != pRequest->mapParams.end( ) )
					{
						string strComment = pRequest->mapParams["comment"].substr( 0, m_iCommentLength );

						if( strComment.empty( ) )
						{
							pResponse->strContent += "<p>You must fill in all the fields. Click <a href=\"/comments.html?info_hash=" + strHashString + "\">here</a> to return to the comments page.</p>\n";

							if( m_bGen )
								pResponse->strContent += "<p class=\"gen\">Generated in " + UTIL_ElapsedTimeStr( btv, UTIL_CurrentTime( ) ) + " seconds.</p>\n";

							pResponse->strContent += "</body>\n";
							pResponse->strContent += "</html>\n";

							return;
						}

						CAtomDicti *pNew = new CAtomDicti( );

						pNew->setItem( "ip", new CAtomString( inet_ntoa( pRequest->sin.sin_addr ) ) );

						if( !user.strLogin.empty( ) )
							pNew->setItem( "name", new CAtomString( user.strLogin ) );

						pNew->setItem( "comment", new CAtomString( strComment ) );

						time_t tNow = time( NULL );
						char *szTime = asctime( localtime( &tNow ) );
						szTime[strlen( szTime ) - 1] = '\0';

						pNew->setItem( "time", new CAtomString( szTime ) );

						( (CAtomList *)pComments )->addItem( pNew );

						saveComments( );

						pResponse->strContent += "<p>Your comment has been posted. DO NOT REFRESH THIS PAGE OR YOUR COMMENT WILL BE POSTED TWICE. Click <a href=\"/comments.html?info_hash=" + strHashString + "\">here</a> to return to the comments page.</p>\n";

						// The Trinity Edition - Addition Begins

						// The following displays a JavaScript alert when a comment is posted
						// and automatically returns the user to the comments display

						pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
						pResponse->strContent += "alert('Your comment has been posted.');\n";
						pResponse->strContent += "window.location='/comments.html?info_hash=" + strHashString + "';\n";
						pResponse->strContent += "</script>\n\n";

						// ---------------------------------------- END OF ADDITION

						if( m_bGen )
							pResponse->strContent += "<p class=\"gen\">Generated in " + UTIL_ElapsedTimeStr( btv, UTIL_CurrentTime( ) ) + " seconds.</p>\n";

						pResponse->strContent += "</body>\n";
						pResponse->strContent += "</html>\n";

						return;
					}
				}

				vector<CAtom *> vecList = ( (CAtomList *)pComments )->getValue( );

				bool bFound = false;

				unsigned long i = 0;

				for( vector<CAtom *> :: iterator it = vecList.begin( ); it != vecList.end( ); it++ )
				{
					if( (*it)->isDicti( ) )
					{
						CAtomDicti *pCommentDicti = (CAtomDicti *)(*it);

						CAtom *pIP = pCommentDicti->getItem( "ip" );
						CAtom *pName = pCommentDicti->getItem( "name" );
						CAtom *pComText = pCommentDicti->getItem( "comment" );
						CAtom *pTime = pCommentDicti->getItem( "time" );

						if( pIP && pComText && pTime )
						{
							if( !bFound )
							{

								// The Trinity Edition - Modification Begins

								// Section Title changed from "Comments" to "User Comments"
								// Added horizontal ruled line
								// and removes the Delete All link... to be placed elsewhere as a button

								pResponse->strContent += "<hr>\n\n";

								// The following places a Delete All Comments button
								// on the top right of the table which presents a 
								// delete confirmation dialog  when the user has chosen to "Delete All Comments"

								if( user.iAccess & ACCESS_EDIT ) {
									pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
									pResponse->strContent += "  function deleteAll() {\n";
									pResponse->strContent += "    if ( confirm ('DELETE CONFIRMATION! \\n\\n Are you sure you want to delete ALL COMMENTS for this torrent? \\n\\n') ) {\n";
									pResponse->strContent += "         document.location = \"/comments.html?info_hash=" + strHashString + "&delall=1\";\n";
									pResponse->strContent += "    }\n";
									pResponse->strContent += "  }\n";
									pResponse->strContent += "</script>\n\n";
									pResponse->strContent += "<table summary=\"comments\">\n";
									pResponse->strContent += "<tr><td style=\"vertical-align:center\"><h4>User Comments</h4></td>\n";
									pResponse->strContent += "<td style=\"vertical-align:center\">\n";
									pResponse->strContent += "<input class=\"comments_deleteall\" type=\"button\" value=\"Delete All Comments\" onClick=\"deleteAll()\"></td></tr>\n";
									pResponse->strContent += "</table>\n\n";
								}

								/* Original Source Code:
								pResponse->strContent += "<p>Comments";

								if( user.iAccess & ACCESS_EDIT )
									pResponse->strContent += " [<a href=\"/comments.html?info_hash=" + strHashString + "&delall=1\">Delete All</a>]";

								pResponse->strContent += "</p>\n";
								*/

								// --------------------------------------------------- END OF MODIFICATION

								pResponse->strContent += "<table summary=\"comments\">\n";

								bFound = true;
							}

							string strIP = pIP->toString( );
							string strName;

							if( pName )
								strName = pName->toString( );

							string strComText = pComText->toString( );
							string strTime = pTime->toString( );

							if( strName.empty( ) )
							{
								// strip ip

								string :: size_type iStart = strIP.rfind( "." );

								if( iStart != string :: npos )
								{
									// don't strip ip for mods

									if( !( user.iAccess & ACCESS_EDIT ) )
										strIP = strIP.substr( 0, iStart + 1 ) + "xxx";
								}
							}
							else
							{
								if( !( user.iAccess & ACCESS_EDIT ) )
									strIP = "HIDDEN";
							}

							//
							// header
							//

							pResponse->strContent += "<tr class=\"com_header\"><td class=\"com_header\"><code>Comment " + CAtomInt( i + 1 ).toString( ) + " posted by ";

							if( !strName.empty( ) )
								pResponse->strContent += "<strong>" + UTIL_RemoveHTML( strName ) + "</strong> (";

							pResponse->strContent += strIP;

							if( !strName.empty( ) )
								pResponse->strContent += ")";

							pResponse->strContent += " on " + strTime;

							// The Trinity Edition - Modification Begins
							// changes delete link to a button with a confirmation popup

							if( user.iAccess & ACCESS_EDIT ) {
								pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
								pResponse->strContent += "  function deleteComment" + CAtomInt( i + 1 ).toString( ) + "() {\n";
								pResponse->strContent += "    if ( confirm ('DELETE CONFIRMATION! \\n\\n Are you sure you want to delete comment #" + CAtomInt( i + 1 ).toString( ) + "? \\n\\n') ) {\n";
								pResponse->strContent += "         var commentNumber = \"" + CAtomInt( i + 1 ).toString( ) + "\";\n";
								pResponse->strContent += "         var torrentHash = \"" + strHashString + "\";\n";
								pResponse->strContent += "         document.location = \"/comments.html?info_hash=\" + torrentHash + \"&del=\" + commentNumber;\n";
								pResponse->strContent += "    }\n";
								pResponse->strContent += "  }\n";
								pResponse->strContent += "</script>\n\n";
								pResponse->strContent += "| <input class=\"comments_delete\" type=\"button\" value=\"Delete This Comment\" onClick=\"deleteComment" + CAtomInt( i + 1 ).toString( ) + "()\">\n\n";
							}

							/* Original Source Code:
							if( user.iAccess & ACCESS_EDIT )
								pResponse->strContent += " [<a href=\"/comments.html?info_hash=" + strHashString + "&del=" + CAtomInt( i + 1 ).toString( ) + "\">Delete</a>]";
							*/

							pResponse->strContent += "</code></td></tr>\n";

							//
							// body
							//

							pResponse->strContent += "<tr class=\"com_body\"><td class=\"com_body\"><code>" + UTIL_RemoveHTML( strComText ) + "</code></td></tr>\n";
						}
					}

					i++;
				}

				// The Trinity Edition - Modification Begins
				
				// Add new Header Text and Horizontal Rule when no comments are posted.

				if( bFound ) {
					pResponse->strContent += "</table>\n";
				}
				else {
					pResponse->strContent += "<hr><h3>User Comments</h3>\n\n";
					pResponse->strContent += "<p>No User Comments Posted</p>\n";
				}

				/* Original Source Code:
				if( bFound )
					pResponse->strContent += "</table>\n";
				else
					pResponse->strContent += "<p>No Comments Posted</p>\n";
				*/
			}

			if( user.iAccess & ACCESS_COMMENTS )
			{
				pResponse->strContent += "<p>Post A Comment</p>\n";
				pResponse->strContent += "<form method=\"get\" action=\"/comments.html\" name=\"form\" onSubmit=\"return validate( this )\">\n";
				pResponse->strContent += "<ul>\n";
				pResponse->strContent += "<li>Comments must be less than " + CAtomInt( m_iCommentLength ).toString( ) + " characters long</li>\n";
				pResponse->strContent += "<li>No HTML</li>\n";
				pResponse->strContent += "</ul>\n";
				pResponse->strContent += "<input name=\"info_hash\" type=hidden value=\"" + strHashString + "\">\n";
				pResponse->strContent += "<textarea name=\"comment\" rows=8 cols=64></textarea><br><br>\n";

				// The Trinity Edition - Modification Begins
				// The following changes the "check message length" link to a button and puts it after the Submit button
				// Also adds a reset button

				pResponse->strContent += "<input type=\"submit\" value=\"Submit\">\n";
				pResponse->strContent += "<input type=\"reset\" value=\"Reset\">\n";
				pResponse->strContent += "<input type=\"button\" value=\"Check Message Length\" onClick=\"javascript:checklength( document.form );\">\n";

				/* Original Source Code:
				pResponse->strContent += "<a href=\"javascript:checklength( document.form );\">[check message length]</a><br><br>\n";
				pResponse->strContent += "<input type=submit value=\"Submit\">\n";
				*/

				// --------------------------------------------------- END OF MODIFICATION

				pResponse->strContent += "</form>\n";
			}
			else
				pResponse->strContent += "<p class=\"denied\">You are not authorized to post comments.</p>\n";
		}
	}
	else
		pResponse->strContent += "<p class=\"denied\">You are not authorized to view this page.</p>\n";

	if( m_bGen )
		pResponse->strContent += "<p class=\"gen\">Generated in " + UTIL_ElapsedTimeStr( btv, UTIL_CurrentTime( ) ) + " seconds.</p>\n";

	pResponse->strContent += "</body>\n";
	pResponse->strContent += "</html>\n";
}
