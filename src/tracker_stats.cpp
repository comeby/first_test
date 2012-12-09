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
#include "bencode.h"
#include "sort.h"
#include "tracker.h"
#include "util.h"

void CTracker :: serverResponseStats( struct request_t *pRequest, struct response_t *pResponse, user_t user )
{
	struct bnbttv btv = UTIL_CurrentTime( );

	if( !m_bShowStats )
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

	// The Trinity Edition - MOD BEGINS - Uses Custom Tracker Title in place of BNBT

	if( !gstrTrackerTitle.empty( ) ) {
		pResponse->strContent += "<title>" + gstrTrackerTitle + " &mdash; Torrent File Info</title>\n";
	}
	else {
		pResponse->strContent += "<title>BNBT &mdash; Torrent File Info</title>\n";
	}

	/* Original Source Code:
	pResponse->strContent += "<title>BNBT File Info</title>\n";
	*/

	if( !gstrStyle.empty( ) )
		pResponse->strContent += "<link rel=stylesheet type=\"text/css\" href=\"" + gstrStyle + "\">\n";

	pResponse->strContent += "</head>\n";
	pResponse->strContent += "<body>\n";

	// The Trinity Edition - NAVBAR

if( m_bUseNavbar ) {

	string strHashString = pRequest->mapParams["info_hash"];
	string strHash = UTIL_StringToHash( strHashString );

	pResponse->strContent += "<table class=\"navbar\" width=\"100%\"><tr class=\"navbar\"><td class=\"navbar\">\n\n";

	pResponse->strContent += "<input class=\"rtt\" type=\"button\" value=\"Return To Tracker\" onClick=\"window.location=\'/index.html\'\">\n\n";

	if( m_pAllowed && m_bAllowTorrentDownloads && ( user.iAccess & ACCESS_DL ) && ( m_strExternalTorrentDir.empty( ) ) ) {
		pResponse->strContent += "| <input class=\"stats_download\" type=\"button\" value=\"Download This Torrent\" onClick=\"window.location=\'";
		pResponse->strContent += "/torrent.html?info_hash=" + strHashString;
		pResponse->strContent += "\'\">\n\n";
	}
	else {
		pResponse->strContent += "| <input disabled class=\"stats_download\" type=\"button\" value=\"Download This Torrent\">\n\n";
	}

	/* NEEDS EXTRA CODE TO WORK WITH EXTERNAL TORRENT DIRECTORY

	if( m_pAllowed && m_bAllowTorrentDownloads && ( user.iAccess & ACCESS_DL ) ) {
		pResponse->strContent += "| <input class=\"stats_download\" type=\"button\" value=\"Download This Torrent\" onClick=\"window.location=\'";

		if( m_strExternalTorrentDir.empty( ) ) {
			pResponse->strContent += "/torrent.html?info_hash=" + strHashString;
		}
		else {
			pResponse->strContent += m_strExternalTorrentDir + UTIL_StringToEscapedStrict( strFileName );
		}

		pResponse->strContent += "\'\">\n\n";
	}
	else {
		pResponse->strContent += "| <input disable class=\"stats_download\" type=\"button\" value=\"Download This Torrent\">\n\n";
	}
	*/

	if( user.iAccess & ACCESS_EDIT ) {
		pResponse->strContent += "<input type=\"button\" value=\"Delete This Torrent\" onClick=\"window.location=\'/index.html?del=" + strHashString + "\'\">\n\n";
    }
	else {
		pResponse->strContent += "<input disabled type=\"button\" value=\"Delete This Torrent\">\n\n";
	}

	pResponse->strContent += "</td></tr></table>\n\n";

}

	// -------------------------------- END OF ADDITION

// The Trinity Edition - Modification Begins
// Removes Login and Logged In Message when using the NAVBAR

if( !m_bUseNavbar ) {

	if( user.strLogin.empty( ) )
		pResponse->strContent += "<p class=\"login1_stats\">You are not logged in. Click <a href=\"/login.html\">here</a> to login.</p>\n";
	else
		pResponse->strContent += "<p class=\"login2_stats\">You are logged in as <span class=\"username\">" + UTIL_RemoveHTML( user.strLogin ) + "</span>. Click <a href=\"/login.html?logout=1\">here</a> to logout.</p>\n";

}

	/* Original Source Code:
	if( user.strLogin.empty( ) )
		pResponse->strContent += "<p class=\"login1_stats\">You are not logged in. Click <a href=\"/login.html\">here</a> to login.</p>\n";
	else
		pResponse->strContent += "<p class=\"login2_stats\">You are logged in as <span class=\"username\">" + UTIL_RemoveHTML( user.strLogin ) + "</span>. Click <a href=\"/login.html?logout=1\">here</a> to logout.</p>\n";
	*/

// ------------------------------------------------- END OF MODIFICATION

	// The Trinity Edition - Removes the following 
	/* Original Source Code:
	pResponse->strContent += "<h3>BNBT File Info</h3>\n";
	*/
	// ------------------------ END OF REMOVAL

	if( user.iAccess & ACCESS_VIEW )
	{
		string strHashString = pRequest->mapParams["info_hash"];
		string strHash = UTIL_StringToHash( strHashString );

		if( !strHash.empty( ) )
		{
			//
			// admin
			//

			if( user.iAccess & ACCESS_EDIT )
			{
				if( m_pTags )
				{
					string strName = pRequest->mapParams["name"];
					string strTag = pRequest->mapParams["tag"];
					string strUploader = pRequest->mapParams["uploader"];
					string strInfoLink = pRequest->mapParams["infolink"];
					string strOK = pRequest->mapParams["ok"];

					if( !strName.empty( ) || !strTag.empty( ) || !strUploader.empty( ) || !strInfoLink.empty( ) )
					{
						if( strOK == "1" )
						{
							addTag( strHash, strTag, strName, strUploader, strInfoLink );

							// The Trinity Edition - Modification Begins
							// Replaces standard response with a JavaScript Alert
							// Displays information based on changes
							// NEEDS WORK FOR INTUITIVENESS

							pResponse->strContent += "<p>Torrent Information Saved!</p>\n\n";

							pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
							
							pResponse->strContent += " var new_torrentname = \"" + UTIL_RemoveHTML( strName ) + "\";\n";
							pResponse->strContent += " var new_uploader = \"" + UTIL_RemoveHTML( strUploader ) + "\";\n";
							pResponse->strContent += " var new_infolink = \"" + UTIL_RemoveHTML( strInfoLink ) + "\";\n\n";
							pResponse->strContent += " var new_tag = \"" + UTIL_RemoveHTML( strTag ) + "\";\n\n";

							pResponse->strContent += " var tagstatus = \" The Torrent's Category/Type Tag is ( \" + new_tag + \" )\";\n\n";

							// pResponse->strContent += "function torrentname() {\n";
							// pResponse->strContent += " if ( new_torrentname != \"\" ) {\n";
							pResponse->strContent += "      var torrentnamestatus = \"The Torrent's Name has been changed to ( \" + new_torrentname + \" )\";\n";
							// pResponse->strContent += " }   \n";
							// pResponse->strContent += " else {\n";
							// pResponse->strContent += "      global torrentnamestatus = \"The Torrent's Name was not changed.\";\n";
							// pResponse->strContent += " }\n\n";
							// pResponse->strContent += "}\n";

							// pResponse->strContent += "function uploader() {\n";
							// pResponse->strContent += " if ( new_uploader == \"REMOVE\" ) {\n";
							// pResponse->strContent += "      var uploaderstatus = \"The Torrent Uploader's Name has been REMOVED.\";\n";
							// pResponse->strContent += "      return;\n";
							// pResponse->strContent += " }\n";
							// pResponse->strContent += " if ( new_uploader != \"\" ) {\n";
							pResponse->strContent += "      var uploaderstatus = \"The Torrent Uploader's Name has been changed to ( \" + new_uploader + \" )\";\n";
							// pResponse->strContent += " }\n";
							// pResponse->strContent += " else {\n";
							// pResponse->strContent += "      var uploaderstatus = \"The Torrent Uploader's Name was not changed.\";\n";
							// pResponse->strContent += " }\n\n";
							// pResponse->strContent += "}\n";

							// pResponse->strContent += "function infolink() {\n";
							// pResponse->strContent += " if ( new_infolink == \"REMOVE\" ) {\n";
							// pResponse->strContent += "      var infolinkstatus = \"The Torrent's InfoLink has been REMOVED.\";\n";
							// pResponse->strContent += "      return;\n";
							// pResponse->strContent += " }\n";
							// pResponse->strContent += " if ( new_infolink != \"\" ) {\n";
							pResponse->strContent += "      var infolinkstatus = \"The Torrent's InfoLink has been changed to ( \" + new_infolink + \" )\";\n";
							// pResponse->strContent += " }\n";
							// pResponse->strContent += " else {\n";
							// pResponse->strContent += "      var infolinkstatus = \"The Torrent's InfoLink was not changed.\";\n";
							// pResponse->strContent += " }\n\n";
							// pResponse->strContent += "}\n";

							// pResponse->strContent += "function postresults() {\n";
							pResponse->strContent += "if ( confirm (\"TORRENT INFORMATION SAVED: \\n\\n - \" + tagstatus + \"\\n\\n - \" + torrentnamestatus + \"\\n\\n - \" + uploaderstatus + \"\\n\\n - \" + infolinkstatus + \"\\n\\n PLEASE NOTE: Blank Values Mean No Change. \\n\\n\\n -------------------------------------- \\n\\n Press OK to return to the Torrent's Stats Page. \\n\\n Press CANCEL to return to the tracker.\") ) {\n";
							pResponse->strContent += "     document.location = \"/stats.html?info_hash=" + strHashString + "\";\n";
							pResponse->strContent += "}\n";
							pResponse->strContent += "else {\n";
							pResponse->strContent += "     document.location = \"/index.html\";\n";
							pResponse->strContent += "}\n\n";
							// pResponse->strContent += "}\n";

							// pResponse->strContent += " torrentname();\n";
							// pResponse->strContent += " uploader();\n";
							// pResponse->strContent += " infolink();\n";
							// pResponse->strContent += " postresults();\n";

							pResponse->strContent += "</script>\n\n";

							/* Original Source Code:
							pResponse->strContent += "<ul>\n";
							pResponse->strContent += "<li>Changed name to \"" + UTIL_RemoveHTML( strName ) + "\" (blank values mean no change).</li>\n";
							pResponse->strContent += "<li>Changed tag to \"" + UTIL_RemoveHTML( strTag ) + "\" (blank values mean no change).</li>\n";
							pResponse->strContent += "<li>Changed uploader to \"" + UTIL_RemoveHTML( strUploader ) + "\" (blank values mean no change).</li>\n";
							pResponse->strContent += "<li>Changed info link to \"" + UTIL_RemoveHTML( strInfoLink ) + "\" (blank values mean no change).</li>\n";
							pResponse->strContent += "</ul>\n";
							pResponse->strContent += "<p>Click <a href=\"/stats.html?info_hash=" + strHashString + "\">here</a> to return to the stats page.</p>\n";
							*/

							// -------------------------------------------- END OF MODIFICATION

							if( m_bGen )
								pResponse->strContent += "<p class=\"gen\">Generated in " + UTIL_ElapsedTimeStr( btv, UTIL_CurrentTime( ) ) + " seconds.</p>\n";

							pResponse->strContent += "</body>\n";
							pResponse->strContent += "</html>\n";

							return;
						}
						else
						{

							// The Trinity Edition - MOD BEGINS
							// JavaScript prompts for automation when Changing a Torrent's Information

							pResponse->strContent += "<p>Changing Torrent Information...</p>\n\n";

							pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
							pResponse->strContent += "if ( confirm (\"PLEASE CONFIRM! \\n\\n Are you sure you want to change this torrent's information?\") ) {\n";
							pResponse->strContent += "  window.location=\"/stats.html?info_hash=" + strHashString + "&name=" + UTIL_StringToEscaped( strName ) + "&uploader=" + UTIL_StringToEscaped( strUploader ) + "&infolink=" + UTIL_StringToEscaped( strInfoLink ) + "&tag=" + UTIL_StringToEscaped( strTag ) + "&ok=1\";\n";
							pResponse->strContent += "}\n";
							pResponse->strContent += "else {\n";
							pResponse->strContent += "  window.history.back();\n";
							pResponse->strContent += "}\n";
							pResponse->strContent += "</script>\n\n";

							/* Original Source Code:
							pResponse->strContent += "<p>Are you sure you want to change the torrent's info?</p>\n";
							pResponse->strContent += "<p><a href=\"/stats.html?info_hash=" + strHashString + "&name=" + UTIL_StringToEscaped( strName ) + "&uploader=" + UTIL_StringToEscaped( strUploader ) + "&infolink=" + UTIL_StringToEscaped( strInfoLink ) + "&tag=" + UTIL_StringToEscaped( strTag ) + "&ok=1\">OK</a></p>\n";
							*/

							// -------------------------------------------- END OF MOD

							if( m_bGen )
								pResponse->strContent += "<p class=\"gen\">Generated in " + UTIL_ElapsedTimeStr( btv, UTIL_CurrentTime( ) ) + " seconds.</p>\n";

							pResponse->strContent += "</body>\n";
							pResponse->strContent += "</html>\n";

							return;
						}
					}

					// The Trinity Edition - MOD BEGINS
					// Removes first <hr>
					// Alters Form Layout to Match Other Forms (Label / Field)
					// Assigns name/class to form

					pResponse->strContent += "<h3>Change Torrent Information</h3>\n";

					// The following script provides HELP on how to Change Torrent Information

					pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
					pResponse->strContent += "  function tipsChange() {\n";
					pResponse->strContent += "    alert('TIPS: Changing Torrent Information \\n\\n' +\n";
					pResponse->strContent += "      '- New Name: The Torrent Name displayed in the Name column. \\n\\n' +\n";
					pResponse->strContent += "      '- New Uploader: The name that is displayed in the Uploader column. \\n\\n' +\n";
					pResponse->strContent += "      '- New InfoLink: The hypertext URL in the InfoLink column. Must start with http://.\\n\\n' +\n";
					pResponse->strContent += "      '- Category/Type Tag: Defaults to the CURRENT VALUE. \\n\\n' +\n";        
					pResponse->strContent += "      '-------------------------------------- \\n\\n' +\n";
					pResponse->strContent += "      'SPECIAL NOTES: \\n\\n 1. To keep the current value for a particular detail, leave the field BLANK. \\n\\n' +\n";
					pResponse->strContent += "      '2. To remove the Uploader or InfoLink completely, type REMOVE in ALL CAPS and save changes. \\n\\n');\n";
					pResponse->strContent += "  }\n\n";
					
		                        pResponse->strContent += "function NewName() {\n";
		                        pResponse->strContent += "  if ( navigator.appName == \"Microsoft Internet Explorer\" ) {\n";
		                        pResponse->strContent += "    Pasted = document.changeinfo.name.createTextRange();\n";
		                        pResponse->strContent += "    Pasted.execCommand(\"Paste\");\n";
		                        pResponse->strContent += "  }\n";
		                        pResponse->strContent += "  else {\n";
		                        pResponse->strContent += "    alert(\"This feature only works with Microsoft Internet Explorer.\");\n";
		                        pResponse->strContent += "  }\n";
		                        pResponse->strContent += "}\n\n";
		                        
		                        pResponse->strContent += "function NewUploader() {\n";
		                        pResponse->strContent += "  if ( navigator.appName == \"Microsoft Internet Explorer\" ) {\n";
		                        pResponse->strContent += "    Pasted = document.changeinfo.uploader.createTextRange();\n";
		                        pResponse->strContent += "    Pasted.execCommand(\"Paste\");\n";
		                        pResponse->strContent += "  }\n";
		                        pResponse->strContent += "  else {\n";
		                        pResponse->strContent += "    alert(\"This feature only works with Microsoft Internet Explorer.\");\n";
		                        pResponse->strContent += "  }\n";
		                        pResponse->strContent += "}\n\n";
		                        
		                        pResponse->strContent += "function NewInfolink() {\n";
		                        pResponse->strContent += "  if ( navigator.appName == \"Microsoft Internet Explorer\" ) {\n";
		                        pResponse->strContent += "    Pasted = document.changeinfo.infolink.createTextRange();\n";
		                        pResponse->strContent += "    Pasted.execCommand(\"Paste\");\n";
		                        pResponse->strContent += "  }\n";
		                        pResponse->strContent += "  else {\n";
		                        pResponse->strContent += "    alert(\"This feature only works with Microsoft Internet Explorer.\");\n";
		                        pResponse->strContent += "  }\n";
		                        pResponse->strContent += "}\n\n";
		
					pResponse->strContent += "</script>\n\n";

					pResponse->strContent += "<p><input type=\"button\" class=\"help_changeinfo\" value=\"TIPS: Changing Torrent Information\" onClick=\"tipsChange()\"></p>\n\n";

					pResponse->strContent += "<form style=\"display:inline\" method=\"get\" name=\"changeinfo\" class=\"changeinfo\" action=\"/stats.html\">\n";
					pResponse->strContent += "<table class=\"changeinfo\">\n\n";
					pResponse->strContent += "<tr class=\"changeinfo\"><td class=\"changeinfo\" style=\"text-align:right\">\n\n";
					pResponse->strContent += "New Name</td><td>\n";
					pResponse->strContent += "<input name=\"info_hash\" type=hidden value=\"" + strHashString + "\">\n";
					pResponse->strContent += "<input name=\"name\" id=\"name\" type=text size=64 maxlength=" + CAtomInt( MAX_FILENAME_LEN ).toString( ) + ">\n";
                                        pResponse->strContent += "<input type=\"button\" name=\"pasteNewName\" value=\"Paste\" onClick=\"NewName()\"></td></tr>\n";
					pResponse->strContent += "<tr class=\"changeinfo\"><td class=\"changeinfo\" style=\"text-align:right\">\n\n";
					pResponse->strContent += "New Uploader</td><td>\n";
					pResponse->strContent += "<input name=\"uploader\" id=\"uploader\" type=text size=64 maxlength=" + CAtomInt( m_iNameLength ).toString( ) + ">\n";
                                        pResponse->strContent += "<input type=\"button\" name=\"pasteNewUploader\" value=\"Paste\" onClick=\"NewUploader()\"></td></tr>\n";
					pResponse->strContent += "<tr class=\"changeinfo\"><td class=\"changeinfo\" style=\"text-align:right\">\n\n";
					pResponse->strContent += "New InfoLink</td><td>\n";
					pResponse->strContent += "<input name=\"infolink\" id=\"infolink\" type=text size=64 maxlength=" + CAtomInt( MAX_INFOLINK_LEN ).toString( ) + ">\n";
                                        pResponse->strContent += "<input type=\"button\" name=\"pasteNewInfolink\" value=\"Paste\" onClick=\"NewInfolink()\"></td></tr>\n";

					/* Original Source Code:
					pResponse->strContent += "<hr width=\"80%\">\n";
					pResponse->strContent += "<p>Change Info</p>\n";
					pResponse->strContent += "<form method=\"get\" action=\"/stats.html\">\n";
					pResponse->strContent += "<input name=\"info_hash\" type=hidden value=\"" + strHashString + "\">\n";
					pResponse->strContent += "<input name=\"name\" type=text size=64 maxlength=" + CAtomInt( MAX_FILENAME_LEN ).toString( ) + "> New Name (blank values mean no change)<br><br>\n";
					pResponse->strContent += "<input name=\"uploader\" type=text size=64 maxlength=" + CAtomInt( m_iNameLength ).toString( ) + "> New Uploader (blank values mean no change)<br><br>\n";
					pResponse->strContent += "<input name=\"infolink\" type=text size=64 maxlength=" + CAtomInt( MAX_INFOLINK_LEN ).toString( ) + "> New Info Link (blank values mean no change)<br><br>\n";
					*/

					string strCur;

					CAtom *pTagInfo = m_pTags->getItem( strHash );

					if( pTagInfo && pTagInfo->isDicti( ) )
					{
						CAtom *pCur = ( (CAtomDicti *)pTagInfo )->getItem( "tag" );

						if( pCur )
							strCur = pCur->toString( );
					}

					// Modified by Trinity

					if( m_vecTags.size( ) > 0 ) {
						pResponse->strContent += "<tr class=\"changeinfo\"><td class=\"changeinfo\" style=\"text-align:right\">\n\n";
						pResponse->strContent += "Category/Type Tag</td><td>\n";
						pResponse->strContent += "<select name=\"tag\">\n";
					}

					/* Original Source Code:
					if( m_vecTags.size( ) > 0 )
						pResponse->strContent += "<select name=\"tag\">\n";
					*/

					for( vector< pair<string, string> > :: iterator i = m_vecTags.begin( ); i != m_vecTags.end( ); i++ )
					{
						pResponse->strContent += "<option";

						if( (*i).first == strCur )
							pResponse->strContent += " selected";

						pResponse->strContent += ">" + (*i).first + "\n";
					}

					// Modified by Trinity

					if( m_vecTags.size( ) > 0 )
						pResponse->strContent += "</select></td></tr>\n";

					/* Original Source Code:
					if( m_vecTags.size( ) > 0 )
						pResponse->strContent += "</select> New Tag<br><br>\n";
					*/

					// Modified by Trinity

					pResponse->strContent += "<tr class=\"changeinfo\"><td colspan=\"2\" class=\"changeinfo\" style=\"text-align:right\">\n";
					pResponse->strContent += "<input type=\"submit\" value=\"Save Changes\"> <input type=\"button\" value=\"Discard Changes\" onClick=\"window.history.back()\">\n";
					pResponse->strContent += "</td></tr></table>\n\n";
					pResponse->strContent += "</form>\n";

					/* Original Source Code:
					pResponse->strContent += "<input type=submit value=\"Change Info\">\n";
					pResponse->strContent += "</form>\n";
					*/

					// <hr> replaces <hr width=80%>
					pResponse->strContent += "<hr>\n";
					/* Original Source Code:
					pResponse->strContent += "<hr width=\"80%\">\n";
					*/

					// ----------------------------------------- END OF MODS
				}
			}

			// The Trinity Edition - ADD BEGINS - places a blank for <h3> tag when not an admin for navbar spacing reasons

			else {
				if( m_bUseNavbar ) {
				pResponse->strContent += "<h3 style=\"display:inline\">&nbsp;</h3>\n\n";
				}
			}

			// ------------------------------------ END OF ADD

			// display torrent information list

			string strFileName;

			int64 iSize = 0;

			if( m_pAllowed )
			{
				CAtom *pTorrent = m_pAllowed->getItem( strHash );

				if( pTorrent && pTorrent->isList( ) )
				{
					vector<CAtom *> vecTorrent = ( (CAtomList *)pTorrent )->getValue( );

					if( vecTorrent.size( ) == 6 )
					{
						CAtom *pFileName = vecTorrent[0];
						CAtom *pName = vecTorrent[1];
						CAtom *pAdded = vecTorrent[2];
						CAtom *pSize = vecTorrent[3];
						CAtom *pFiles = vecTorrent[4];
						CAtom *pComment = vecTorrent[5];

						// The Trinity Edition - MOD BEGINS - changes <p> tags to <h4>

						pResponse->strContent += "<h4>File Information</h4>\n";

						/* Original Source Code:
						pResponse->strContent += "<p>File Information</p>\n";
						*/

						// -------------------- END OF MOD

						pResponse->strContent += "<ul>\n";

						if( pFileName )
							strFileName = pFileName->toString( );

						if( pName )
							pResponse->strContent += "<li><strong>Name:</strong> " + UTIL_RemoveHTML( pName->toString( ) ) + "</li>\n";

						pResponse->strContent += "<li><strong>Info Hash:</strong> " + strHashString + "</li>\n";

						if( pAdded )
							pResponse->strContent += "<li><strong>Added:</strong> " + pAdded->toString( ) + "</li>\n";

						if( pSize && pSize->isLong( ) )
						{
							iSize = ( (CAtomLong *)pSize )->getValue( );

							pResponse->strContent += "<li><strong>Size:</strong> " + UTIL_BytesToString( iSize ) + "</li>\n";
						}

						if( pFiles && pFiles->isInt( ) )
						{
							pResponse->strContent += "<li><strong>Files:</strong> " + pFiles->toString( ) + "</li>\n";

							if( ( (CAtomInt *)pFiles )->getValue( ) == 1 )
								strFileName.erase( );
						}

						pResponse->strContent += "</ul>\n";

						if( pComment )
						{
							if( m_bShowFileComment )
							{
								// The Trinity Tracker - MOD BEGINS - changes STRING to "Torrent File Comment"

								pResponse->strContent += "<p>Torrent File Comment</p>\n";

								// disable block element spacing

								pResponse->strContent += "<table style=\"display:inline\" summary=\"file comment\">\n";
								
								/* Original Source Code:
								pResponse->strContent += "<p>File Comment</p>\n";
								pResponse->strContent += "<table summary=\"file comment\">\n";
								*/

								pResponse->strContent += "<tr><td class=\"com_body\"><code>" + UTIL_RemoveHTML( pComment->toString( ) ) + "</code></td></tr>\n";
								pResponse->strContent += "</table>\n";
							}
						}
					}
				}
			}

			if( m_pAllowed && m_bAllowTorrentDownloads && ( user.iAccess & ACCESS_DL ) )
			{
				// The Trinity Edition - Mod Begins - Disables Download Link when NAVBAR is being used

				if( !m_bUseNavbar ) {

					pResponse->strContent += "<p><a class=\"download\" href=\"";

					if( m_strExternalTorrentDir.empty( ) )
						pResponse->strContent += "/torrent.html?info_hash=" + strHashString;
					else
						pResponse->strContent += m_strExternalTorrentDir + UTIL_StringToEscapedStrict( strFileName );

					pResponse->strContent += "\">DOWNLOAD TORRENT</a></p>";

				}

				/* Original Source Code
				pResponse->strContent += "<p><a class=\"download\" href=\"";

				if( m_strExternalTorrentDir.empty( ) )
					pResponse->strContent += "/torrent.html?info_hash=" + strHashString;
				else
					pResponse->strContent += m_strExternalTorrentDir + UTIL_StringToEscapedStrict( strFileName );

				pResponse->strContent += "\">DOWNLOAD TORRENT</a></p>";
				*/

				// -------------------- END OF MOD
			}

			if( m_bShowFileContents && !strFileName.empty( ) )
			{
				CAtom *pDecoded = DecodeFile( ( m_strAllowedDir + strFileName ).c_str( ) );

				if( pDecoded && pDecoded->isDicti( ) )
				{
					CAtom *pInfo = ( (CAtomDicti *)pDecoded )->getItem( "info" );

					if( pInfo && pInfo->isDicti( ) )
					{
						CAtom *pFiles = ( (CAtomDicti *)pInfo )->getItem( "files" );

						if( pFiles && pFiles->isList( ) )
						{
							bool bFound = false;

							int iAdded = 0;

							vector<CAtom *> vecFiles = ( (CAtomList *)pFiles )->getValue( );

							for( vector<CAtom *> :: iterator i = vecFiles.begin( ); i != vecFiles.end( ); i++ )
							{
								if( (*i)->isDicti( ) )
								{
									CAtom *pPath = ( (CAtomDicti *)(*i) )->getItem( "path" );
									CAtom *pLen = ( (CAtomDicti *)(*i) )->getItem( "length" );

									if( pPath && pPath->isList( ) )
									{
										if( !bFound )
										{
											// The Trinity Tracker - MOD BEGINS
											// Changes "Contents" to Contents of Torrent File and uses <h4>
											// Adds <hr> before

											pResponse->strContent += "<hr>\n\n";
											pResponse->strContent += "<h4>Contents of Torrent File</h4>\n";

											pResponse->strContent += "<table style=\"display:inline\" summary=\"contents\">\n";

											/* Original Source Code:
											pResponse->strContent += "<p>Contents</p>\n";
											pResponse->strContent += "<table summary=\"contents\">\n";
											*/

											// ------------------------------------- END OF MOD

											bFound = true;
										}

										string strPath;

										vector<CAtom *> vecPath = ( (CAtomList *)pPath )->getValue( );

										for( vector<CAtom *> :: iterator j = vecPath.begin( ); j != vecPath.end( ); j++ )
										{
											if( j != vecPath.begin( ) )
												strPath += '/';

											strPath += (*j)->toString( );
										}

										if( !strPath.empty( ) )
										{
											if( iAdded % 2 )
												pResponse->strContent += "<tr class=\"even\">";
											else
												pResponse->strContent += "<tr class=\"odd\">";

											pResponse->strContent += "<td class=\"path\">" + UTIL_RemoveHTML( strPath ) + "</td>";

											if( pLen && pLen->isLong( ) )
												pResponse->strContent += "<td class=\"bytes\">" + UTIL_BytesToString( ( (CAtomLong *)pLen )->getValue( ) ) + "</td>";

											pResponse->strContent += "</tr>\n";

											iAdded++;
										}
									}
								}
							}

							if( bFound )
								pResponse->strContent += "</table>\n";
						}
					}
				}

				if( pDecoded )
					delete pDecoded;
			}

			if( m_pDFile )
			{
				CAtom *pPeers = m_pDFile->getItem( strHash );

				if( pPeers && pPeers->isDicti( ) )
				{
					map<string, CAtom *> mapPeersDicti = ( (CAtomDicti *)pPeers )->getValue( );

					// add the peers into this structure one by one and sort it afterwards

					struct peer_t *pPeersT = new struct peer_t[mapPeersDicti.size( )];

					unsigned long i = 0;

					for( map<string, CAtom *> :: iterator it = mapPeersDicti.begin( ); it != mapPeersDicti.end( ); it++ )
					{
						pPeersT[i].iUpped = 0;
						pPeersT[i].iDowned = 0;
						pPeersT[i].iLeft = 0;
						pPeersT[i].iConnected = 0;
						pPeersT[i].flShareRatio = 0.0;

						if( (*it).second->isDicti( ) )
						{
							CAtomDicti *pPeerDicti = (CAtomDicti *)(*it).second;

							CAtom *pIP = pPeerDicti->getItem( "ip" );
							CAtom *pUpped = pPeerDicti->getItem( "uploaded" );
							CAtom *pDowned = pPeerDicti->getItem( "downloaded" );
							CAtom *pLef = pPeerDicti->getItem( "left" );
							CAtom *pConn = pPeerDicti->getItem( "connected" );

							if( pIP )
							{
								pPeersT[i].strIP = pIP->toString( );

								// strip ip

								string :: size_type iStart = pPeersT[i].strIP.rfind( "." );

								if( iStart != string :: npos )
								{
									// don't strip ip for mods

									if( !( user.iAccess & ACCESS_EDIT ) )
										pPeersT[i].strIP = pPeersT[i].strIP.substr( 0, iStart + 1 ) + "xxx";
								}
							}

							if( pUpped && pUpped->isLong( ) )
								pPeersT[i].iUpped = ( (CAtomLong *)pUpped )->getValue( );

							if( pDowned && pDowned->isLong( ) )
							{
								pPeersT[i].iDowned = ( (CAtomLong *)pDowned )->getValue( );

								if( m_bShowShareRatios )
								{
									if( pPeersT[i].iDowned > 0 )
										pPeersT[i].flShareRatio = (float)pPeersT[i].iUpped / (float)pPeersT[i].iDowned;
									else if( pPeersT[i].iUpped == 0 )
										pPeersT[i].flShareRatio = 0.0;
									else
										pPeersT[i].flShareRatio = -1.0;
								}
							}

							if( pLef && pLef->isLong( ) )
								pPeersT[i].iLeft = ( (CAtomLong *)pLef )->getValue( );

							if( pConn && pConn->isLong( ) )
								pPeersT[i].iConnected = GetTime( ) - (unsigned long)( (CAtomLong *)pConn )->getValue( );
						}

						i++;
					}

					string strSort = pRequest->mapParams["sort"];

					if( m_bSort )
					{
						if( !strSort.empty( ) )
						{
							int iSort = atoi( strSort.c_str( ) );

							if( iSort == SORTP_AUPPED )
								qsort( pPeersT, mapPeersDicti.size( ), sizeof( struct peer_t ), asortpByUpped );
							else if( iSort == SORTP_ADOWNED )
								qsort( pPeersT, mapPeersDicti.size( ), sizeof( struct peer_t ), asortpByDowned );
							else if( iSort == SORTP_ALEFT )
								qsort( pPeersT, mapPeersDicti.size( ), sizeof( struct peer_t ), asortpByLeft );
							else if( iSort == SORTP_ACONNECTED )
								qsort( pPeersT, mapPeersDicti.size( ), sizeof( struct peer_t ), asortpByConnected );
							else if( iSort == SORTP_ASHARERATIO )
							{
								if( m_bShowShareRatios )
									qsort( pPeersT, mapPeersDicti.size( ), sizeof( struct peer_t ), asortpByShareRatio );
							}
							else if( iSort == SORTP_DUPPED )
								qsort( pPeersT, mapPeersDicti.size( ), sizeof( struct peer_t ), dsortpByUpped );
							else if( iSort == SORTP_DDOWNED )
								qsort( pPeersT, mapPeersDicti.size( ), sizeof( struct peer_t ), dsortpByDowned );
							else if( iSort == SORTP_DLEFT )
								qsort( pPeersT, mapPeersDicti.size( ), sizeof( struct peer_t ), dsortpByLeft );
							else if( iSort == SORTP_DCONNECTED )
								qsort( pPeersT, mapPeersDicti.size( ), sizeof( struct peer_t ), dsortpByConnected );
							else if( iSort == SORTP_DSHARERATIO )
							{
								if( m_bShowShareRatios )
									qsort( pPeersT, mapPeersDicti.size( ), sizeof( struct peer_t ), dsortpByShareRatio );
							}
							else
							{
								// default action is to sort by left

								qsort( pPeersT, mapPeersDicti.size( ), sizeof( struct peer_t ), asortpByLeft );
							}
						}
						else
						{
							// default action is to sort by left

							qsort( pPeersT, mapPeersDicti.size( ), sizeof( struct peer_t ), asortpByLeft );
						}
					}
					else
					{
						// sort is disabled, but default action is to sort by left

						qsort( pPeersT, mapPeersDicti.size( ), sizeof( struct peer_t ), asortpByLeft );
					}

					bool bFound = false;

					int iAdded = 0;

					//
					// seeders
					//

					string :: size_type iCountGoesHere = string :: npos;

					for( unsigned long i = 0; i < mapPeersDicti.size( ); i++ )
					{
						if( iAdded >= m_iMaxPeersDisplay )
							break;

						if( pPeersT[i].iLeft == 0 )
						{
							if( !bFound )
							{
								// output table headers

								pResponse->strContent += "<p>Seeders</p>\n";

								// to save on calculations, we're going to insert the number of seeders later, keep track of where

								iCountGoesHere = pResponse->strContent.size( ) - strlen( "</p>\n" );

								pResponse->strContent += "<table summary=\"seeders\">\n";
								pResponse->strContent += "<tr><th class=\"ip\">Peer IP</th><th class=\"bytes\">Uploaded";

								if( m_bSort )
								{
									pResponse->strContent += "<br><a class=\"sort\" href=\"/stats.html?info_hash=";
									pResponse->strContent += strHashString;
									pResponse->strContent += "&sort=";
									pResponse->strContent += SORTPSTR_AUPPED;
									pResponse->strContent += "\">A</a> <a class=\"sort\" href=\"/stats.html?info_hash=";
									pResponse->strContent += strHashString;
									pResponse->strContent += "&sort=";
									pResponse->strContent += SORTPSTR_DUPPED;
									pResponse->strContent += "\">Z</a>";
								}

								pResponse->strContent += "</th><th class=\"bytes\">Downloaded";

								if( m_bSort )
								{
									pResponse->strContent += "<br><a class=\"sort\" href=\"/stats.html?info_hash=";
									pResponse->strContent += strHashString;
									pResponse->strContent += "&sort=";
									pResponse->strContent += SORTPSTR_ADOWNED;
									pResponse->strContent += "\">A</a> <a class=\"sort\" href=\"/stats.html?info_hash=";
									pResponse->strContent += strHashString;
									pResponse->strContent += "&sort=";
									pResponse->strContent += SORTPSTR_DDOWNED;
									pResponse->strContent += "\">Z</a>";
								}

								pResponse->strContent += "</th><th class=\"connected\">Connected";

								if( m_bSort )
								{
									pResponse->strContent += "<br><a class=\"sort\" href=\"/stats.html?info_hash=";
									pResponse->strContent += strHashString;
									pResponse->strContent += "&sort=";
									pResponse->strContent += SORTPSTR_ACONNECTED;
									pResponse->strContent += "\">A</a> <a class=\"sort\" href=\"/stats.html?info_hash=";
									pResponse->strContent += strHashString;
									pResponse->strContent += "&sort=";
									pResponse->strContent += SORTPSTR_DCONNECTED;
									pResponse->strContent += "\">Z</a>";
								}

								pResponse->strContent += "</th>";

								if( m_bShowShareRatios )
								{
									pResponse->strContent += "<th class=\"number\">Share Ratio";

									if( m_bSort )
									{
										pResponse->strContent += "<br><a class=\"sort\" href=\"/stats.html?info_hash=";
										pResponse->strContent += strHashString;
										pResponse->strContent += "&sort=";
										pResponse->strContent += SORTPSTR_ASHARERATIO;
										pResponse->strContent += "\">A</a> <a class=\"sort\" href=\"/stats.html?info_hash=";
										pResponse->strContent += strHashString;
										pResponse->strContent += "&sort=";
										pResponse->strContent += SORTPSTR_DSHARERATIO;
										pResponse->strContent += "\">Z</a>";
									}

									pResponse->strContent += "</th>";
								}

								if( m_bShowAvgULRate )
									pResponse->strContent += "<th class=\"number\">Average Upload Rate</th>";

								pResponse->strContent += "</tr>\n";

								bFound = true;
							}

							// output table rows

							if( iAdded % 2 )
								pResponse->strContent += "<tr class=\"even\">";
							else
								pResponse->strContent += "<tr class=\"odd\">";

							pResponse->strContent += "<td class=\"ip\">" + pPeersT[i].strIP + "</td>";
							pResponse->strContent += "<td class=\"bytes\">" + UTIL_BytesToString( pPeersT[i].iUpped ) + "</td>";
							pResponse->strContent += "<td class=\"bytes\">" + UTIL_BytesToString( pPeersT[i].iDowned ) + "</td>";
							pResponse->strContent += "<td class=\"connected\">" + UTIL_SecondsToString( pPeersT[i].iConnected ) + "</td>";

							if( m_bShowShareRatios )
							{
								pResponse->strContent += "<td class=\"number_";

								if( pPeersT[i].flShareRatio == -1.0 )
									pResponse->strContent += "green\">";
								else if( pPeersT[i].flShareRatio < 0.8 )
									pResponse->strContent += "red\">";
								else if( pPeersT[i].flShareRatio < 1.2 )
									pResponse->strContent += "yellow\">";
								else
									pResponse->strContent += "green\">";

								// turn the share ratio into a string

								if( pPeersT[i].flShareRatio == -1.0 )
									pResponse->strContent += "Perfect";
								else
								{
									char szFloat[16];
									memset( szFloat, 0, sizeof( char ) * 16 );
									sprintf( szFloat, "%0.3f", pPeersT[i].flShareRatio );

									pResponse->strContent += szFloat;
								}

								pResponse->strContent += "</td>";
							}

							if( m_bShowAvgULRate )
							{
								pResponse->strContent += "<td class=\"number\">";

								if( pPeersT[i].iConnected > 0 )
									pResponse->strContent += UTIL_BytesToString( pPeersT[i].iUpped / pPeersT[i].iConnected ) + "/sec";
								else
									pResponse->strContent += "N/A";

								pResponse->strContent += "</td>";
							}

							pResponse->strContent += "</tr>\n";

							iAdded++;
						}
					}

					// insert the number of seeders

					string strTemp = " (" + CAtomInt( iAdded ).toString( ) + ")";

					if( iCountGoesHere != string :: npos )
						pResponse->strContent.insert( iCountGoesHere, strTemp );

					iCountGoesHere = string :: npos;

					if( bFound )
						pResponse->strContent += "</table>\n";

					bFound = false;

					iAdded = 0;

					//
					// leechers
					//

					for( unsigned long i = 0; i < mapPeersDicti.size( ); i++ )
					{
						if( iAdded >= m_iMaxPeersDisplay )
							break;

						if( pPeersT[i].iLeft != 0 )
						{
							if( !bFound )
							{
								// output table headers

								pResponse->strContent += "<p>Leechers</p>\n";

								// to save on calculations, we're going to insert the number of leechers later, keep track of where

								iCountGoesHere = pResponse->strContent.size( ) - strlen( "</p>\n" );

								pResponse->strContent += "<table summary=\"leechers\">\n";
								pResponse->strContent += "<tr><th class=\"ip\">Peer IP</th><th class=\"bytes\">Uploaded";

								if( m_bSort )
								{
									pResponse->strContent += "<br><a class=\"sort\" href=\"/stats.html?info_hash=";
									pResponse->strContent += strHashString;
									pResponse->strContent += "&sort=";
									pResponse->strContent += SORTPSTR_AUPPED;
									pResponse->strContent += "\">A</a> <a class=\"sort\" href=\"/stats.html?info_hash=";
									pResponse->strContent += strHashString;
									pResponse->strContent += "&sort=";
									pResponse->strContent += SORTPSTR_DUPPED;
									pResponse->strContent += "\">Z</a>";
								}

								pResponse->strContent += "</th><th class=\"bytes\">Downloaded";

								if( m_bSort )
								{
									pResponse->strContent += "<br><a class=\"sort\" href=\"/stats.html?info_hash=";
									pResponse->strContent += strHashString;
									pResponse->strContent += "&sort=";
									pResponse->strContent += SORTPSTR_ADOWNED;
									pResponse->strContent += "\">A</a> <a class=\"sort\" href=\"/stats.html?info_hash=";
									pResponse->strContent += strHashString;
									pResponse->strContent += "&sort=";
									pResponse->strContent += SORTPSTR_DDOWNED;
									pResponse->strContent += "\">Z</a>";
								}

								if( m_pAllowed && m_bShowLeftAsProgress )
									pResponse->strContent += "</th><th class=\"bytes\">Progress";
								else
									pResponse->strContent += "</th><th class=\"bytes\">Left";

								if( m_bSort )
								{
									pResponse->strContent += "<br><a class=\"sort\" href=\"/stats.html?info_hash=";
									pResponse->strContent += strHashString;
									pResponse->strContent += "&sort=";
									pResponse->strContent += SORTPSTR_ALEFT;
									pResponse->strContent += "\">A</a> <a class=\"sort\" href=\"/stats.html?info_hash=";
									pResponse->strContent += strHashString;
									pResponse->strContent += "&sort=";
									pResponse->strContent += SORTPSTR_DLEFT;
									pResponse->strContent += "\">Z</a>";
								}

								pResponse->strContent += "</th><th class=\"connected\">Connected";

								if( m_bSort )
								{
									pResponse->strContent += "<br><a class=\"sort\" href=\"/stats.html?info_hash=";
									pResponse->strContent += strHashString;
									pResponse->strContent += "&sort=";
									pResponse->strContent += SORTPSTR_ACONNECTED;
									pResponse->strContent += "\">A</a> <a class=\"sort\" href=\"/stats.html?info_hash=";
									pResponse->strContent += strHashString;
									pResponse->strContent += "&sort=";
									pResponse->strContent += SORTPSTR_DCONNECTED;
									pResponse->strContent += "\">Z</a>";
								}

								pResponse->strContent += "</th>";

								if( m_bShowShareRatios )
								{
									pResponse->strContent += "<th class=\"number\">Share Ratio";

									if( m_bSort )
									{
										pResponse->strContent += "<br><a class=\"sort\" href=\"/stats.html?info_hash=";
										pResponse->strContent += strHashString;
										pResponse->strContent += "&sort=";
										pResponse->strContent += SORTPSTR_ASHARERATIO;
										pResponse->strContent += "\">A</a> <a class=\"sort\" href=\"/stats.html?info_hash=";
										pResponse->strContent += strHashString;
										pResponse->strContent += "&sort=";
										pResponse->strContent += SORTPSTR_DSHARERATIO;
										pResponse->strContent += "\">Z</a>";
									}

									pResponse->strContent += "</th>";
								}

								if( m_bShowAvgULRate )
									pResponse->strContent += "<th class=\"number\">Average Upload Rate</th>";

								if( m_bShowAvgDLRate )
									pResponse->strContent += "<th class=\"number\">Average Download Rate</th>";

								pResponse->strContent += "</tr>\n";

								bFound = true;
							}

							// output table rows

							if( iAdded % 2 )
								pResponse->strContent += "<tr class=\"even\">";
							else
								pResponse->strContent += "<tr class=\"odd\">";

							pResponse->strContent += "<td class=\"ip\">" + pPeersT[i].strIP + "</td>";
							pResponse->strContent += "<td class=\"bytes\">" + UTIL_BytesToString( pPeersT[i].iUpped ) + "</td>";
							pResponse->strContent += "<td class=\"bytes\">" + UTIL_BytesToString( pPeersT[i].iDowned ) + "</td>";
							pResponse->strContent += "<td class=\"percent\">";

							if( m_pAllowed && m_bShowLeftAsProgress )
								pResponse->strContent += UTIL_BytesToString( iSize - pPeersT[i].iLeft );
							else
								pResponse->strContent += UTIL_BytesToString( pPeersT[i].iLeft );

							if( m_pAllowed )
							{
								pResponse->strContent += " (";

								int iPercent = 0;

								if( iSize > 0 )
								{
									if( m_bShowLeftAsProgress )
										iPercent = 100 - (int)( ( (float)pPeersT[i].iLeft / iSize ) * 100 );
									else
										iPercent = (int)( ( (float)pPeersT[i].iLeft / iSize ) * 100 );
								}

								pResponse->strContent += CAtomInt( iPercent ).toString( ) + "%)";

								if( !m_strImageBarFill.empty( ) && !m_strImageBarTrans.empty( ) )
								{
									pResponse->strContent += "<br>";

									if( iPercent > 0 )
										pResponse->strContent += "<img src=\"" + m_strImageBarFill + "\" width=" + CAtomInt( iPercent ).toString( ) + " height=8>";

									if( iPercent < 100 )
										pResponse->strContent += "<img src=\"" + m_strImageBarTrans + "\" width=" + CAtomInt( 100 - iPercent ).toString( ) + " height=8>";
								}
							}

							pResponse->strContent += "</td><td class=\"connected\">" + UTIL_SecondsToString( pPeersT[i].iConnected ) + "</td>";

							if( m_bShowShareRatios )
							{
								pResponse->strContent += "<td class=\"number_";

								if( pPeersT[i].flShareRatio == -1.0 )
									pResponse->strContent += "green\">";
								else if( pPeersT[i].flShareRatio < 0.8 )
									pResponse->strContent += "red\">";
								else if( pPeersT[i].flShareRatio < 1.2 )
									pResponse->strContent += "yellow\">";
								else
									pResponse->strContent += "green\">";

								// turn the share ratio into a string

								if( pPeersT[i].flShareRatio == -1.0 )
									pResponse->strContent += "Perfect";
								else
								{
									char szFloat[16];
									memset( szFloat, 0, sizeof( char ) * 16 );
									sprintf( szFloat, "%0.3f", pPeersT[i].flShareRatio );

									pResponse->strContent += szFloat;
								}

								pResponse->strContent += "</td>";
							}

							if( m_bShowAvgULRate )
							{
								pResponse->strContent += "<td class=\"number\">";

								if( pPeersT[i].iConnected > 0 )
									pResponse->strContent += UTIL_BytesToString( pPeersT[i].iUpped / pPeersT[i].iConnected ) + "/sec";
								else
									pResponse->strContent += "N/A";

								pResponse->strContent += "</td>";
							}

							if( m_bShowAvgDLRate )
							{
								pResponse->strContent += "<td class=\"number\">";

								if( pPeersT[i].iConnected > 0 )
									pResponse->strContent += UTIL_BytesToString( pPeersT[i].iDowned / pPeersT[i].iConnected ) + "/sec";
								else
									pResponse->strContent += "N/A";

								pResponse->strContent += "</td>";
							}

							pResponse->strContent += "</tr>\n";

							iAdded++;
						}
					}

					// insert the number of leechers

					strTemp = " (" + CAtomInt( iAdded ).toString( ) + ")";

					if( iCountGoesHere != string :: npos )
						pResponse->strContent.insert( iCountGoesHere, strTemp );

					delete [] pPeersT;

					if( bFound )
						pResponse->strContent += "</table>\n";
				}
			}
		}
	}
	else
		pResponse->strContent += "<p class=\"denied\">You are not authorized to view this page.</p>\n";

	if( m_bGen )
		pResponse->strContent += "<p class=\"gen\">Generated in " + UTIL_ElapsedTimeStr( btv, UTIL_CurrentTime( ) ) + " seconds.</p>\n";

	pResponse->strContent += "</body>\n";
	pResponse->strContent += "</html>\n";
}
