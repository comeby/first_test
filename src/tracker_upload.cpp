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
#include "client.h"
#include "tracker.h"
#include "util.h"

void CTracker :: serverResponseUploadGET( struct request_t *pRequest, struct response_t *pResponse, user_t user )
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
        
	// The Trinity Edition - Addition Begins
	// Javascript Functions for Paste
	
	pResponse->strContent += "<script language=\"JavaScript\" type=\"text/javascript\">\n\n";
	
        pResponse->strContent += "function namePaste() {\n";
        pResponse->strContent += "  if ( navigator.appName == \"Microsoft Internet Explorer\" ) {\n";
        pResponse->strContent += "    Pasted = document.fileuploader.name.createTextRange();\n";
	pResponse->strContent += "    Pasted.execCommand(\"Paste\");\n";
	pResponse->strContent += "  }\n";
	pResponse->strContent += "  else {\n";
	pResponse->strContent += "    alert(\"This feature only works with Microsoft Internet Explorer.\");\n";
	pResponse->strContent += "  }\n";
	pResponse->strContent += "}\n\n";
	
        pResponse->strContent += "function infolinkPaste() {\n";
        pResponse->strContent += "  if ( navigator.appName == \"Microsoft Internet Explorer\" ) {\n";
        pResponse->strContent += "    Pasted = document.fileuploader.infolink.createTextRange();\n";
	pResponse->strContent += "    Pasted.execCommand(\"Paste\");\n";
	pResponse->strContent += "  }\n";
	pResponse->strContent += "  else {\n";
	pResponse->strContent += "    alert(\"This feature only works with Microsoft Internet Explorer.\");\n";
	pResponse->strContent += "  }\n";
	pResponse->strContent += "}\n\n";
	
	pResponse->strContent += "</script>\n\n";
	
	// ----------------------------------------- End of Addition

	// The Trinity Edition - MOD BEGINS - 
	// Uses a Custom Tracker Title instead of BNBT

	if( !gstrTrackerTitle.empty( ) ) {
		pResponse->strContent += "<title>" + gstrTrackerTitle + " File Uploader</title>\n";
	}
	else {
        pResponse->strContent += "<title>BNBT File Uploader</title>\n"; // << Original Source Code
	}

	if( !gstrStyle.empty( ) )
		pResponse->strContent += "<link rel=stylesheet type=\"text/css\" href=\"" + gstrStyle + "\">\n";

	pResponse->strContent += "</head>\n";
	pResponse->strContent += "<body>\n";

	// The Trinity Edition - RTT and My Torrents Button

if( m_bUseNavbar ) {

	pResponse->strContent += "<table class=\"navbar\" width=\"100%\"><tr class=\"navbar\"><td class=\"navbar\">\n\n";

	pResponse->strContent += "<input class=\"rtt\" type=\"button\" value=\"Return To Tracker\" onClick=\"window.location=\'/index.html\'\">\n";
	
		if( ( user.iAccess & ACCESS_UPLOAD ) && ( !user.strLogin.empty( ) ) ) {
			pResponse->strContent += "<input class=\"mytorrents\" type=\"button\" value=\"My Torrents\" onClick=\"window.location=\'/login.html\'\">\n";
		}
		else {
			pResponse->strContent += "<input disabled class=\"mytorrents\" type=\"button\" value=\"My Torrents\">\n";
		}

	pResponse->strContent += "</td></tr></table>\n\n";

}

	// -------------------------------- END OF ADDITION

// The Trinity Edition - Modification Begins
// Removes Login and Logged In Message when using the NAVBAR

if( !m_bUseNavbar ) {

	if( user.strLogin.empty( ) )
		pResponse->strContent += "<p class=\"login1_upload\">You are not logged in. Click <a href=\"/login.html\">here</a> to login.</p>\n";
	else
		pResponse->strContent += "<p class=\"login2_upload\">You are logged in as <span class=\"username\">" + UTIL_RemoveHTML( user.strLogin ) + "</span>. Click <a href=\"/login.html?logout=1\">here</a> to logout.</p>\n";

}

	/* Original Source Code:
	if( user.strLogin.empty( ) )
		pResponse->strContent += "<p class=\"login1_upload\">You are not logged in. Click <a href=\"/login.html\">here</a> to login.</p>\n";
	else
		pResponse->strContent += "<p class=\"login2_upload\">You are logged in as <span class=\"username\">" + UTIL_RemoveHTML( user.strLogin ) + "</span>. Click <a href=\"/login.html?logout=1\">here</a> to logout.</p>\n";
	*/

// ------------------------------------------------- END OF MODIFICATION

	// Static UPLOAD Header Document (TRINCUSTSTATDOX)

	pResponse->strContent += m_strStaticHeaderUpload; // ADDED

	// Only Display <h3> header if Static Header is NOT being used

	if( m_strStaticHeaderUploadFile.empty( ) ) {
		if( !gstrTrackerTitle.empty( ) ) {
            pResponse->strContent += "<h3>" + gstrTrackerTitle + " File Uploader</h3>\n";
		}
		else {
			pResponse->strContent += "<h3>BNBT File Uploader</h3>\n";
		}
	}

	/* Original Source Code:
	pResponse->strContent += "<h3>BNBT File Uploader</h3>\n";
	*/

	// -------------------------- END OF MODS

	if( user.iAccess & ACCESS_UPLOAD )
	{
		if( m_strUploadDir.empty( ) )
		{
			pResponse->strContent += "<p class=\"denied\">This tracker does not allow file uploads. Click <a href=\"/index.html\">here</a> to return to the tracker.</p>\n";

			if( m_bGen )
				pResponse->strContent += "<p class=\"gen\">Generated in " + UTIL_ElapsedTimeStr( btv, UTIL_CurrentTime( ) ) + " seconds.</p>\n";

			pResponse->strContent += "</body>\n";
			pResponse->strContent += "</html>\n";

			return;
		}
		else if( m_iMaxTorrents > 0 )
		{
			if( m_pAllowed )
			{
				if( m_pAllowed->getValue( ).size( ) >= (unsigned int)m_iMaxTorrents )
				{
					pResponse->strContent += "<p class=\"denied\">This tracker has reached its torrent limit. Click <a href=\"/index.html\">here</a> to return to the tracker.</p>\n";

					if( m_bGen )
						pResponse->strContent += "<p class=\"gen\">Generated in " + UTIL_ElapsedTimeStr( btv, UTIL_CurrentTime( ) ) + " seconds.</p>\n";

					pResponse->strContent += "</body>\n";
					pResponse->strContent += "</html>\n";

					return;
				}
			}
		}
		
		// The Trinity Edition - Addition Begins
		// The following script provides HELP on how to use the BNBT File Uploader

		pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
		pResponse->strContent += "  function helpUpload() {\n";
		pResponse->strContent += "    alert('Help Topic: HOW TO USE THE FILE UPLOADER \\n\\n' +\n";
		pResponse->strContent += "      '- Torrent File: Press the Browse button to select the torrent file on your computer. \\n\\n' +\n";
		pResponse->strContent += "      '- Torrent Name: OPTIONAL - Will be taken from the filename if not specified. \\n\\n' +\n";

		if( m_bAllowInfoLink ) {
			pResponse->strContent += "      '- Information Link: OPTIONAL - MUST BEGIN WITH \"http://\". \\n\\n' +\n";
		}
		if( m_vecTags.size( ) > 0 ) {
            pResponse->strContent += "      '- Category/Type Tag: Used to specify the type of files you are torrenting. \\n\\n' +\n";
		}

		pResponse->strContent += "      '- Upload Torrent Button: Uploads your torrent to the tracker. \\n\\n' +\n";        
		pResponse->strContent += "      'SPECIAL NOTE: Please be sure that you have used the correct Announce URL for this tracker, when you created your torrent. \\n\\n')\n";

		pResponse->strContent += "  }\n\n";

		// The following script verifies that the category/tag selected is not blank

		pResponse->strContent += "  function verifyTag() {\n";
		pResponse->strContent += "    if ( document.fileuploader.tag.selectedIndex == 0 ) {\n";
		pResponse->strContent += "         alert('Please Specify a Category/Type Tag!');\n";
		pResponse->strContent += "         return;\n";
		pResponse->strContent += "    }\n";
		pResponse->strContent += "    else {\n";
		pResponse->strContent += "         document.fileuploader.submit();\n";
		pResponse->strContent += "    }\n";
		pResponse->strContent += "  }\n";
		pResponse->strContent += "</script>\n\n";

		pResponse->strContent += "<input type=\"button\" class=\"help_upload\" value=\"Help: How to use the File Uploader\" onClick=\"helpUpload()\">\n\n";

		// The Trinity Edition - Modification Begins
		// Places form fields and labels in a table for a better visual flow
		// Also assigns a name and css class to the form for future implementation
		// Places Paste buttons for use with IE

		pResponse->strContent += "<form class=\"fileuploader\" name=\"fileuploader\" method=\"post\" action=\"/upload.html\" enctype=\"multipart/form-data\">\n";

		pResponse->strContent += "<table class=\"fileuploader\">\n";
		pResponse->strContent += "  <tr class=\"fileuploader\">\n";
		pResponse->strContent += "    <td class=\"fileuploader_leftcell\" style=\"text-align:right\">\n";
		pResponse->strContent += "      Torrent File: </td>\n";
		pResponse->strContent += "    <td class=\"fileuploader_rightcell\">\n";
		pResponse->strContent += "      <input name=\"torrent\" type=\"file\" size=\"37\"> </td></tr>\n";
		pResponse->strContent += "  <tr class=\"fileuploader\">\n";
		pResponse->strContent += "    <td class=\"fileuploader_leftcell\" style=\"text-align:right\">\n";
		pResponse->strContent += "      Torrent Name: </td>\n";
		pResponse->strContent += "    <td class=\"fileuploader_rightcell\">\n";
		pResponse->strContent += "      <input name=\"name\" type=\"text\" size=\"50\" maxlength=\"" + CAtomInt( MAX_FILENAME_LEN ).toString( ) + "\">\n";
                pResponse->strContent += "      <input name=\"namepaste\" type=\"button\" value=\"Paste\" onClick=\"namePaste()\"></td></tr>\n";

		if( m_bAllowInfoLink ) { // Show InfoLink if Allow InfoLink is set
            pResponse->strContent += "  <tr class=\"fileuploader\">\n";
			pResponse->strContent += "    <td class=\"fileuploader_leftcell\" style=\"text-align:right\">\n";
			pResponse->strContent += "      Information Link: </td>\n";
			pResponse->strContent += "    <td class=\"fileuploader_rightcell\">\n";
			pResponse->strContent += "      <input name=\"infolink\" type=\"text\" size=\"50\" maxlength=\"" + CAtomInt( MAX_INFOLINK_LEN ).toString( ) + "\">\n";
			pResponse->strContent += "      <input name=\"infolinkpaste\" type=\"button\" value=\"Paste\" onClick=\"infolinkPaste()\"></td></tr>\n";
		}

		if( m_vecTags.size( ) > 0 ) { // Show Category/Type Tag Selector when at least 1 exists
			pResponse->strContent += "  <tr class=\"fileuploader\">\n";
			pResponse->strContent += "    <td class=\"fileuploader_leftcell\" style=\"text-align:right\">\n";
			pResponse->strContent += "      Category/Type Tag: </td>\n";
			pResponse->strContent += "    <td class=\"fileuploader_rightcell\">\n";
			pResponse->strContent += "      <select name=\"tag\">\n";
			pResponse->strContent += "        <option selected value=\"none specified\"></option>\n";
		}

		for( vector< pair<string, string> > :: iterator i = m_vecTags.begin( ); i != m_vecTags.end( ); i++ )
			pResponse->strContent += "<option>" + (*i).first + "\n";

		if( m_vecTags.size( ) > 0 ) {
			pResponse->strContent += "      </select> </td></tr>\n";
		}

		pResponse->strContent += "  <tr class=\"fileuploader\">\n";
		pResponse->strContent += "    <td class=\"fileuploader_submit\">\n";

		if( m_vecTags.size( ) > 0 ) {
            pResponse->strContent += "      <input class=\"uploadtorrent\" type=\"button\" value=\"Upload Torrent\" onClick=\"verifyTag()\"> </td>\n\n";		
		}
		else {
			pResponse->strContent += "      <input class=\"uploadtorrent\" type=\"button\" value=\"Upload Torrent\" onClick=\"document.fileuploader.submit()\"> </td>\n\n";		
		}

		pResponse->strContent += "    <td class=\"fileuploader_blist\">\n";

		pResponse->strContent += "<ul>\n";		
		pResponse->strContent += "<li>No HTML</li>\n";
		pResponse->strContent += "<li><strong>Max. File Size:</strong> " + UTIL_BytesToString( MAX_RECV_SIZE ) + "</li>\n";
		pResponse->strContent += "<li>Names must be less than " + CAtomInt( MAX_FILENAME_LEN ).toString( ) + " characters long</li>\n";

		// The following JavaScript will display the "ANNOUNCE URL TO USE"
		// when the "Force Announce URL" is not specified in the tracker's configuration
		// This item can be disabled using CSS:

		/* ----------	.announce_url{display:none}   ------------ */

		if( !m_strForceAnnounceURL.empty( ) ) {
			pResponse->strContent += "<li class=\"announce_url\"><strong>Auto Announce URL:</strong> " + UTIL_RemoveHTML( m_strForceAnnounceURL ) + "</li>\n";
		}
		else {
			pResponse->strContent += "<li class=\"announce_url\"><strong>Announce URL:</strong> \n";
			pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
			pResponse->strContent += "  document.write( location.protocol + \"//\" + location.host + \"/announce\" );\n";
			pResponse->strContent += "</script>\n\n";
			pResponse->strContent += "</li>\n\n";
		}

		pResponse->strContent += "</ul> </td></tr>\n";
		pResponse->strContent += "</table>\n";
		pResponse->strContent += "</form>\n";
		
		// The Trinity Edition - ADDITION BEGINS - Static UPLOAD Footer Document (TRINCUSTSTATDOX)
		pResponse->strContent += m_strStaticFooterUpload;
		// -------------------------------- END OF ADD

		/* Original Source Code:
		pResponse->strContent += "<form method=\"post\" action=\"/upload.html\" enctype=\"multipart/form-data\">\n";
		pResponse->strContent += "<input name=\"torrent\" type=file size=24> Torrent<br><br>\n";
		pResponse->strContent += "<input name=\"name\" type=text size=64 maxlength=" + CAtomInt( MAX_FILENAME_LEN ).toString( ) + "> Name (optional - if blank, the name will be taken from the filename)<br><br>\n";

		if( m_bAllowInfoLink )
			pResponse->strContent += "<input name=\"infolink\" type=text size=64 maxlength=" + CAtomInt( MAX_INFOLINK_LEN ).toString( ) + "> Info Link (optional)<br><br>\n";

		if( m_vecTags.size( ) > 0 )
			pResponse->strContent += "<select name=\"tag\">\n";

		for( vector< pair<string, string> > :: iterator i = m_vecTags.begin( ); i != m_vecTags.end( ); i++ )
			pResponse->strContent += "<option>" + (*i).first + "\n";

		if( m_vecTags.size( ) > 0 )
			pResponse->strContent += "</select> Tag\n";

		pResponse->strContent += "<ul>\n";
		pResponse->strContent += "<li>Names must be less than " + CAtomInt( MAX_FILENAME_LEN ).toString( ) + " characters long</li>\n";
		pResponse->strContent += "<li>No HTML</li>\n";
		pResponse->strContent += "<li><strong>Max. File Size:</strong> " + UTIL_BytesToString( MAX_RECV_SIZE ) + "</li>\n";

		if( !m_strForceAnnounceURL.empty( ) )
			pResponse->strContent += "<li><strong>Auto Announce URL:</strong> " + UTIL_RemoveHTML( m_strForceAnnounceURL ) + "</li>\n";

		pResponse->strContent += "</ul>\n";
		pResponse->strContent += "<input type=submit value=\"Upload\">\n";
		pResponse->strContent += "</form>\n";
		*/
	}
	else
		pResponse->strContent += "<p class=\"denied\">You are not authorized to view this page.</p>\n";

	if( m_bGen )
		pResponse->strContent += "<p class=\"gen\">Generated in " + UTIL_ElapsedTimeStr( btv, UTIL_CurrentTime( ) ) + " seconds.</p>\n";

	pResponse->strContent += "</body>\n";
	pResponse->strContent += "</html>\n";
}

void CTracker :: serverResponseUploadPOST( struct request_t *pRequest, struct response_t *pResponse, CAtomList *pPost, user_t user )
{
	struct bnbttv btv = UTIL_CurrentTime( );

	if( m_strUploadDir.empty( ) || !( user.iAccess & ACCESS_UPLOAD ) )
	{
		pResponse->strCode = "403 Forbidden";

		return;
	}

	if( m_iMaxTorrents > 0 )
	{
		if( m_pAllowed )
		{
			if( m_pAllowed->getValue( ).size( ) >= (unsigned int)m_iMaxTorrents )
			{
				pResponse->strCode = "403 Forbidden";

				return;
			}
		}
	}

	string strFile;
	string strTorrent;
	string strTag;
	string strPostedName;
	string strPostedInfoLink;

	if( pPost )
	{
		vector<CAtom *> vecSegs = pPost->getValue( );

		for( unsigned long i = 0; i < vecSegs.size( ); i++ )
		{
			if( vecSegs[i]->isDicti( ) )
			{
				CAtomDicti *pSeg = (CAtomDicti *)vecSegs[i];

				CAtom *pDisp = ( (CAtomDicti *)vecSegs[i] )->getItem( "disposition" );
				CAtom *pDat = ( (CAtomDicti *)vecSegs[i] )->getItem( "data" );

				if( pDisp && pDisp->isDicti( ) && pDat )
				{
					CAtom *pName = ( (CAtomDicti *)pDisp )->getItem( "name" );

					if( pName )
					{
						string strName = pName->toString( );

						if( strName == "torrent" )
						{
							CAtom *pFile = ( (CAtomDicti *)pDisp )->getItem( "filename" );

							if( pFile )
							{
								// the path is local to the peer

								strFile = UTIL_StripPath( pFile->toString( ) );

								strTorrent = pDat->toString( );
							}
							else
							{
								pResponse->strCode = "400 Bad Request";

								return;
							}
						}
						else if( strName == "tag" )
							strTag = pDat->toString( );
						else if( strName == "name" )
							strPostedName = pDat->toString( ).substr( 0, MAX_FILENAME_LEN );
						else if( strName == "infolink" )
							strPostedInfoLink = pDat->toString( ).substr( 0, MAX_INFOLINK_LEN );
					}
					else
					{
						pResponse->strCode = "400 Bad Request";

						return;
					}
				}
			}
		}
	}
	else
	{
		pResponse->strCode = "400 Bad Request";

		return;
	}

	pResponse->strCode = "200 OK";

	pResponse->mapHeaders.insert( pair<string, string>( "Content-Type", string( "text/html; charset=" ) + gstrCharSet ) );

	pResponse->strContent += "<html>\n";
	pResponse->strContent += "<head>\n";
	pResponse->strContent += "<title>BNBT File Uploader</title>\n";

	if( !gstrStyle.empty( ) )
		pResponse->strContent += "<link rel=stylesheet type=\"text/css\" href=\"" + gstrStyle + "\">\n";

	pResponse->strContent += "</head>\n";
	pResponse->strContent += "<body>\n";

	// The Trinity Edition - Addition Begins
	// The following scripts specify variables for the numerous messages
	// to be displayed in popups when an upload fails:

	pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";

	pResponse->strContent += " var failed_1 = \"UPLOAD FAILED! \\n\\n REASON: The uploaded file is corrupt or invalid. \\n\\n Press OK to go back and make changes. \\n Press CANCEL to start over from scratch. \\n\\n\";\n";
	pResponse->strContent += " var failed_2 = \"UPLOAD FAILED! \\n\\n REASON: The uploaded file is not a .torrent file. \\n\\n Press OK to go back and make changes. \\n Press CANCEL to start over from scratch. \\n\\n\";\n";
	pResponse->strContent += " var failed_3 = \"UPLOAD FAILED! \\n\\n REASON: The file category/tag is invalid. \\n\\n Press OK to go back and make changes. \\n Press CANCEL to start over from scratch. \\n\\n\";\n";
	pResponse->strContent += " var failed_4 = \"UPLOAD FAILED! \\n\\n REASON: The uploaded file already exists. \\n\\n Press OK to go back and make changes. \\n Press CANCEL to start over from scratch. \\n\\n\";\n";
	pResponse->strContent += " var failed_5 = \"UPLOAD FAILED! \\n\\n REASON: A file with the uploaded file's info hash already exists. \\n\\n Press OK to go back and make changes. \\n Press CANCEL to start over from scratch. \\n\\n\";\n";
	
	pResponse->strContent += "</script>\n\n";

	// -------------------------------------------------------------------------- END OF ADDITION

	// The Trinity Edition - RTT, My Torrents, Upload Another, and Back Buttons

if( m_bUseNavbar ) {

	pResponse->strContent += "<table class=\"navbar\" width=\"100%\"><tr class=\"navbar\"><td class=\"navbar\">\n\n";

	pResponse->strContent += "<input class=\"rtt\" type=\"button\" value=\"Return To Tracker\" onClick=\"window.location=\'/index.html\'\">\n";

		if( user.iAccess & ACCESS_UPLOAD ) {
			pResponse->strContent += "<input class=\"mytorrents\" type=\"button\" value=\"My Torrents\" onClick=\"window.location=\'/login.html\'\">\n";
			pResponse->strContent += "<input class=\"uploadanother\" type=\"button\" value=\"Upload Another Torrent\" onClick=\"window.location=\'/upload.html\'\">\n";
		}
		else {
			pResponse->strContent += "<input disabled class=\"mytorrents\" type=\"button\" value=\"My Torrents\">\n";
			pResponse->strContent += "<input disabled class=\"uploadanother\" type=\"button\" value=\"Upload Another Torrent\">\n";
		}

	pResponse->strContent += "<input class=\"back\" type=\"button\" value=\"&laquo; Back\" onClick=\"window.history.back()\">\n\n";

	pResponse->strContent += "</td></tr></table>\n\n";

}

	// -------------------------------- END OF ADDITION

// The Trinity Edition - Modification Begins
// Removes Login and Logged In Message when using the NAVBAR

if( !m_bUseNavbar ) {

	if( user.strLogin.empty( ) )
		pResponse->strContent += "<p class=\"login1_upload\">You are not logged in. Click <a href=\"/login.html\">here</a> to login.</p>\n";
	else
		pResponse->strContent += "<p class=\"login2_upload\">You are logged in as <span class=\"username\">" + UTIL_RemoveHTML( user.strLogin ) + "</span>. Click <a href=\"/login.html?logout=1\">here</a> to logout.</p>\n";

}

	/* Original Source Code:
	if( user.strLogin.empty( ) )
		pResponse->strContent += "<p class=\"login1_upload\">You are not logged in. Click <a href=\"/login.html\">here</a> to login.</p>\n";
	else
		pResponse->strContent += "<p class=\"login2_upload\">You are logged in as <span class=\"username\">" + UTIL_RemoveHTML( user.strLogin ) + "</span>. Click <a href=\"/login.html?logout=1\">here</a> to logout.</p>\n";
	*/

// ------------------------------------------------- END OF MODIFICATION

	string strPath = m_strUploadDir + strFile;

	string :: size_type iExt = strFile.rfind( "." );

	string strExt;

	if( iExt != string :: npos )
		strExt = strFile.substr( iExt );

	if( strTorrent.empty( ) )
	{
		pResponse->strContent += "<h3>Upload Failed</h3>\n";
		pResponse->strContent += "<p>The uploaded file is corrupt or invalid. Click <a href=\"/upload.html\">here</a> to return to the upload page.</p>\n";

		// The Trinity Edition - Addition Begins
		// The following script alerts the user that the upload has failed
		// and allows him/her to retry or cancel

		pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
		pResponse->strContent += "  if ( confirm ( failed_1 ) ) {\n";
		pResponse->strContent += "       window.history.back();\n";
		pResponse->strContent += "  }\n";
		pResponse->strContent += "  else {\n";
		pResponse->strContent += "       document.location = \"/upload.html\";\n";
		pResponse->strContent += "  }\n";
		pResponse->strContent += "</script>\n\n";

		// -------------------------------------------- END OF ADDITION

	}
	else if( strExt != ".torrent" )
	{
		pResponse->strContent += "<h3>Upload Failed</h3>\n";
		pResponse->strContent += "<p>The uploaded file is not a .torrent file. Click <a href=\"/upload.html\">here</a> to return to the upload page.</p>\n";

		// The Trinity Edition - Addition Begins
		// The following script alerts the user that the upload has failed
		// and allows him/her to retry or cancel

		pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
		pResponse->strContent += "  if ( confirm ( failed_2 ) ) {\n";
		pResponse->strContent += "       window.history.back();\n";
		pResponse->strContent += "  }\n";
		pResponse->strContent += "  else {\n";
		pResponse->strContent += "       document.location = \"/upload.html\";\n";
		pResponse->strContent += "  }\n";
		pResponse->strContent += "</script>\n\n";

		// -------------------------------------------- END OF ADDITION

	}
	else if( !checkTag( strTag ) )
	{
		pResponse->strContent += "<h3>Upload Failed</h3>\n";
		pResponse->strContent += "<p>The file tag is invalid. Click <a href=\"/upload.html\">here</a> to return to the upload page.</p>\n";

		// The Trinity Edition - Addition Begins
		// The following script alerts the user that the upload has failed
		// and allows him/her to retry or cancel

		pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
		pResponse->strContent += "  if ( confirm ( failed_3 ) ) {\n";
		pResponse->strContent += "       window.history.back();\n";
		pResponse->strContent += "  }\n";
		pResponse->strContent += "  else {\n";
		pResponse->strContent += "       document.location = \"/upload.html\";\n";
		pResponse->strContent += "  }\n";
		pResponse->strContent += "</script>\n\n";

		// -------------------------------------------- END OF ADDITION

	}
	else if( UTIL_CheckFile( strPath.c_str( ) ) )
	{
		pResponse->strContent += "<h3>Upload Failed</h3>\n";
		pResponse->strContent += "<p>The uploaded file already exists. Click <a href=\"/upload.html\">here</a> to return to the upload page.</p>\n";

		// The Trinity Edition - Addition Begins
		// The following script alerts the user that the upload has failed
		// and allows him/her to retry or cancel

		pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
		pResponse->strContent += "  if ( confirm ( failed_4 ) ) {\n";
		pResponse->strContent += "       window.history.back();\n";
		pResponse->strContent += "  }\n";
		pResponse->strContent += "  else {\n";
		pResponse->strContent += "       document.location = \"/upload.html\";\n";
		pResponse->strContent += "  }\n";
		pResponse->strContent += "</script>\n\n";

		// -------------------------------------------- END OF ADDITION

	}
	else
	{
		CAtom *pTorrent = Decode( strTorrent );

		if( pTorrent && pTorrent->isDicti( ) )
		{
			string strInfoHash = UTIL_InfoHash( pTorrent );

			if( !strInfoHash.empty( ) )
			{
				if( m_pDFile->getItem( strInfoHash ) )
				{
					pResponse->strContent += "<h3>Upload Failed</h3>\n";
					pResponse->strContent += "<p>A file with the uploaded file's info hash already exists. Click <a href=\"/upload.html\">here</a> to return to the upload page.</p>\n";

					// The Trinity Edition - Addition Begins
					// The following script alerts the user that the upload has failed
					// and allows him/her to retry or cancel

					pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
					pResponse->strContent += "  if ( confirm ( failed_5 ) ) {\n";
					pResponse->strContent += "       window.history.back();\n";
					pResponse->strContent += "  }\n";
					pResponse->strContent += "  else {\n";
					pResponse->strContent += "       document.location = \"/upload.html\";\n";
					pResponse->strContent += "  }\n";
					pResponse->strContent += "</script>\n\n";

					// -------------------------------------------- END OF ADDITION

				}
				else
				{
					if( !m_strForceAnnounceURL.empty( ) )
						( (CAtomDicti *)pTorrent )->setItem( "announce", new CAtomString( m_strForceAnnounceURL ) );

					UTIL_MakeFile( strPath.c_str( ), Encode( pTorrent ) );

					addTag( strInfoHash, strTag, strPostedName, user.strLogin, strPostedInfoLink );

					pResponse->strContent += "<h3>Upload Successful</h3>\n";

					if( m_bParseOnUpload )
					{
						if( m_pAllowed )
							parseTorrent( strPath.c_str( ) );

						pResponse->strContent += "<p>The uploaded file is ready. You should start seeding it now. Click <a href=\"/index.html\">here</a> to return to the tracker.</p>\n";

						// The Trinity Edition - Addition Begins
						// The following scripts sets variables for the Seeding Instructions:

						pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
						pResponse->strContent += "  var method = 1;\n";
						pResponse->strContent += "  var message = \" is ready. You should start seeding it now.\";\n";
						pResponse->strContent += "</script>\n\n";

						// ------------------------------------------------ END OF ADDITION
					}
					else
					{
						pResponse->strContent += "<p>The uploaded file will be ready in " + CAtomInt( m_iParseAllowedInterval ).toString( );

						// The Trinity Edition - Modification Begins
						// The following scripts sets variables for the Seeding Instructions:

						if( m_iParseAllowedInterval == 1 ) {

							pResponse->strContent += " minute. You should start seeding it as soon as possible. Click <a href=\"/index.html\">here</a> to return to the tracker.</p>\n";

							pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
							pResponse->strContent += "  var method = 2;\n";
							pResponse->strContent += "  var message = \" will be ready in 1 minute. You should start seeding it as soon as possible.\";\n";
							pResponse->strContent += "</script>\n\n";

						}
						else {

							pResponse->strContent += " minutes. You should start seeding it as soon as possible. Click <a href=\"/index.html\">here</a> to return to the tracker.</p>\n";

							pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
							pResponse->strContent += "  var method = 3;\n";
							pResponse->strContent += "  var message = \" will be ready in " + CAtomInt( m_iParseAllowedInterval ).toString( ) + " minutes. You should start seeding it as soon as possible.\";\n";
							pResponse->strContent += "</script>\n\n";
						}

						/* Original Source Code:
						if( m_iParseAllowedInterval == 1 )
							pResponse->strContent += " minute. You should start seeding it as soon as possible. Click <a href=\"/index.html\">here</a> to return to the tracker.</p>\n";
						else
							pResponse->strContent += " minutes. You should start seeding it as soon as possible. Click <a href=\"/index.html\">here</a> to return to the tracker.</p>\n";
						*/

						// ------------------------------------------------ END OF ADDITION
					}

					UTIL_LogPrint( "%s uploaded %s\n", inet_ntoa( pRequest->sin.sin_addr ), strFile.c_str( ) );

					// The Trinity Edition - Addition Begins
					// The following scripts handle the JavaScript 
					// popups presented to the user after a successful torrent upload

					pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
					pResponse->strContent += "  alert('UPLOAD SUCCESSFUL!');\n";

					if( m_pAllowed && m_bAllowTorrentDownloads && ( user.iAccess & ACCESS_DL ) ) {
						pResponse->strContent += "  var download = \"yes\";\n";
					}
					else {
						pResponse->strContent += "  var download = \"no\";\n";
					}

					pResponse->strContent += "  if ( method > 1 ) {\n";
					pResponse->strContent += "       var startmsg = \"To begin seeding your torrent, open the torrent file on your harddrive in your BitTorrent client.\";\n";
					pResponse->strContent += "  }\n";
					pResponse->strContent += "  if ( method == 1 ) {\n";
					pResponse->strContent += "       if ( download == \"yes\" ) {\n";
					pResponse->strContent += "       var startmsg = \"To begin seeding your torrent, press the OK button below to Return to the Tracker and click the Torrent Download link.\";\n";
					pResponse->strContent += "       }\n";
					pResponse->strContent += "       else {\n";
					pResponse->strContent += "       var startmsg = \"To begin seeding your torrent, open the torrent file on your harddrive in your BitTorrent client.\";\n";
					pResponse->strContent += "       }\n";
					pResponse->strContent += "  }\n";

					pResponse->strContent += "  if ( confirm ('The uploaded file' + message + '\\n\\n' + startmsg + '\\n\\n' + 'Press OK to Return to the Tracker, or press CANCEL if you would like to upload another torrent. \\n\\n') ) {\n";
					pResponse->strContent += "       document.location = \"/index.html\";\n";
					pResponse->strContent += "  }\n";
					pResponse->strContent += "  else {\n";
					pResponse->strContent += "       document.location = \"/upload.html\";\n";
					pResponse->strContent += "  }\n";
					pResponse->strContent += "</script>\n\n";

				}
			}
			else
			{
				pResponse->strContent += "<h3>Upload Failed</h3>\n";
				pResponse->strContent += "<p>The uploaded file is corrupt or invalid. Click <a href=\"/upload.html\">here</a> to return to the upload page.</p>\n";

				// The Trinity Edition - Addition Begins
				// The following script alerts the user that the upload has failed
				// and allows him/her to retry or cancel

				pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
				pResponse->strContent += "  if ( confirm ( failed_1 ) ) {\n";
				pResponse->strContent += "       window.history.back();\n";
				pResponse->strContent += "  }\n";
				pResponse->strContent += "  else {\n";
				pResponse->strContent += "       document.location = \"/upload.html\";\n";
				pResponse->strContent += "  }\n";
				pResponse->strContent += "</script>\n\n";

				// -------------------------------------------- END OF ADDITION

			}
		}
		else
		{
			pResponse->strContent += "<h3>Upload Failed</h3>\n";
			pResponse->strContent += "<p>The uploaded file is corrupt or invalid. Click <a href=\"/upload.html\">here</a> to return to the upload page.</p>\n";

			// The Trinity Edition - Addition Begins
			// The following script alerts the user that the upload has failed
			// and allows him/her to retry or cancel

			pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
			pResponse->strContent += "  if ( confirm ( failed_1 ) ) {\n";
			pResponse->strContent += "       window.history.back();\n";
			pResponse->strContent += "  }\n";
			pResponse->strContent += "  else {\n";
			pResponse->strContent += "       document.location = \"/upload.html\";\n";
			pResponse->strContent += "  }\n";
			pResponse->strContent += "</script>\n\n";

			// -------------------------------------------- END OF ADDITION

		}

		if( pTorrent )
			delete pTorrent;
	}

	if( m_bGen )
		pResponse->strContent += "<p class=\"gen\">Generated in " + UTIL_ElapsedTimeStr( btv, UTIL_CurrentTime( ) ) + " seconds.</p>\n";

	pResponse->strContent += "</body>\n";
	pResponse->strContent += "</html>\n";
}
