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
#include "md5.h"
#include "sort.h"
#include "tracker.h"
#include "util.h"

void CTracker :: serverResponseUsers( struct request_t *pRequest, struct response_t *pResponse, user_t user )
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

	// The Trinity Edition - Changed Page Title
	if( !gstrTrackerTitle.empty( ) ) {
        pResponse->strContent += "<title>" + gstrTrackerTitle + " User Control Panel</title>\n";
	}
	else {
		pResponse->strContent += "<title>BNBT User Control Panel</title>\n";
	}

	/* Original Source Code:
	pResponse->strContent += "<title>BNBT Users Info</title>\n";
	*/

	// --------------------------- END OF MODIFICATION

	if( !gstrStyle.empty( ) )
		pResponse->strContent += "<link rel=stylesheet type=\"text/css\" href=\"" + gstrStyle + "\">\n";

	pResponse->strContent += "</head>\n";
	pResponse->strContent += "<body>\n";

	// The Trinity Edition - RTT Button

if( m_bUseNavbar ) {

	pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n";
	pResponse->strContent += "  function userSearch() {\n";
	pResponse->strContent += "    var search = document.navbar.search.value;\n";
	pResponse->strContent += "    var newurl = \"/users.html?search=\" + search + \"#userlist\";\n";
	pResponse->strContent += "    document.location = newurl;\n";
	pResponse->strContent += "  }\n";
	pResponse->strContent += "</script>\n\n";

	pResponse->strContent += "<table class=\"navbar\" width=\"100%\"><tr class=\"navbar\"><td class=\"navbar\">\n\n";

	pResponse->strContent += "<form style=\"display:inline\" class=\"navbar_search\" name=\"navbar\" method=\"get\" action=\"/users.html\" onSubmit=\"userSearch()\">";

	pResponse->strContent += "<input class=\"rtt\" type=\"button\" value=\"Return To Tracker\" onClick=\"window.location=\'/index.html\'\">\n\n";
	
	if( user.iAccess & ACCESS_ADMIN ) {
		pResponse->strContent += "| <input class=\"users_admin\" type=\"button\" value=\"Admin CP\" onClick=\"window.location=\'/admin.html\'\">\n\n";
		pResponse->strContent += "| <input class=\"users_userlist\" type=\"button\" value=\"View Registered Users\" name=\"vru\" onClick=\"document.location='#userlist'\">\n\n";
		pResponse->strContent += "<input class=\"back\" type=\"button\" value=\"&laquo; Back\" onClick=\"window.history.back()\">\n\n";
		pResponse->strContent += "</td><td class=\"navbar_search\">\n\n";
		pResponse->strContent += "User Search <input class=\"navbar_search\" type=\"text\" name=\"search\">\n";
		pResponse->strContent += "<input class=\"go\" type=\"button\" value=\"Go\" onClick=\"userSearch()\">\n";
		pResponse->strContent += "<input class=\"users_clear\" type=\"button\" value=\"Clear Results\" onClick=\"window.location=\'/users.html\'\">\n";
		pResponse->strContent += "</td></tr>\n\n";
	}
	else {
		pResponse->strContent += "| <input disabled class=\"users_admin\" type=\"button\" value=\"Admin CP\">\n\n";
		pResponse->strContent += "| <input disabled class=\"users_userlist\" type=\"button\" value=\"View Registered Users\">\n\n";
		pResponse->strContent += "<input class=\"back\" type=\"button\" value=\"&laquo; Back\" onClick=\"window.history.back()\">\n\n";
		pResponse->strContent += "</td><td disabled class=\"navbar_search\">\n\n";
		pResponse->strContent += "User Search <input disabled class=\"navbar_search\" type=\"text\" name=\"search\">\n";
		pResponse->strContent += "<input disabled class=\"go\" type=\"button\" value=\"Go\">\n";
		pResponse->strContent += "<input disabled class=\"users_clear\" type=\"button\" value=\"Clear Results\">\n";
		pResponse->strContent += "</td></tr>\n\n";
	}

	pResponse->strContent += "</form></table>\n\n";

}

	// -------------------------------- END OF ADDITION

// The Trinity Edition - Modification Begins
// Removes Login and Logged In Message when using the NAVBAR

if( !m_bUseNavbar ) {

	if( user.strLogin.empty( ) )
		pResponse->strContent += "<p class=\"login1_users\">You are not logged in. Click <a href=\"/login.html\">here</a> to login.</p>\n";
	else
		pResponse->strContent += "<p class=\"login2_users\">You are logged in as <span class=\"username\">" + UTIL_RemoveHTML( user.strLogin ) + "</span>. Click <a href=\"/login.html?logout=1\">here</a> to logout.</p>\n";

}

	/* Original Source Code:
	if( user.strLogin.empty( ) )
		pResponse->strContent += "<p class=\"login1_users\">You are not logged in. Click <a href=\"/login.html\">here</a> to login.</p>\n";
	else
		pResponse->strContent += "<p class=\"login2_users\">You are logged in as <span class=\"username\">" + UTIL_RemoveHTML( user.strLogin ) + "</span>. Click <a href=\"/login.html?logout=1\">here</a> to logout.</p>\n";
	*/

// ------------------------------------------------- END OF MODIFICATION
	// The Trinity Edition - Modification Begins
	// Removes the <h3> header ... will be placed elsewhere

	/* Original Source Code:
	pResponse->strContent += "<h3>BNBT Users Info</h3>\n";
	*/

		// The Trinity Edition - Addition Begins
		// The following sets up functions for the Catch All buttons

		pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";

		pResponse->strContent += "function basic() {\n";
		pResponse->strContent += "  document.forms[1].us_access_view.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_dl.checked = false;\n";
		pResponse->strContent += "  document.forms[1].us_access_comments.checked = false;\n";
		pResponse->strContent += "  document.forms[1].us_access_upload.checked = false;\n";
		pResponse->strContent += "  document.forms[1].us_access_edit.checked = false;\n";
		pResponse->strContent += "  document.forms[1].us_access_admin.checked = false;\n";
		pResponse->strContent += "  document.forms[1].us_access_signup.checked = false;\n";
		pResponse->strContent += "}\n\n";

		pResponse->strContent += "function downloader() {\n";
		pResponse->strContent += "  document.forms[1].us_access_view.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_dl.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_comments.checked = false;\n";
		pResponse->strContent += "  document.forms[1].us_access_upload.checked = false;\n";
		pResponse->strContent += "  document.forms[1].us_access_edit.checked = false;\n";
		pResponse->strContent += "  document.forms[1].us_access_admin.checked = false;\n";
		pResponse->strContent += "  document.forms[1].us_access_signup.checked = false;\n";
		pResponse->strContent += "}\n\n";

		pResponse->strContent += "function poster() {\n";
		pResponse->strContent += "  document.forms[1].us_access_view.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_dl.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_comments.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_upload.checked = false;\n";
		pResponse->strContent += "  document.forms[1].us_access_edit.checked = false;\n";
		pResponse->strContent += "  document.forms[1].us_access_admin.checked = false;\n";
		pResponse->strContent += "  document.forms[1].us_access_signup.checked = false;\n";
		pResponse->strContent += "}\n\n";

		pResponse->strContent += "function uploader() {\n";
		pResponse->strContent += "  document.forms[1].us_access_view.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_dl.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_comments.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_upload.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_edit.checked = false;\n";
		pResponse->strContent += "  document.forms[1].us_access_admin.checked = false;\n";
		pResponse->strContent += "  document.forms[1].us_access_signup.checked = false;\n";
		pResponse->strContent += "}\n\n";

		pResponse->strContent += "function moderator() {\n";
		pResponse->strContent += "  document.forms[1].us_access_view.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_dl.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_comments.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_upload.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_edit.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_admin.checked = false;\n";
		pResponse->strContent += "  document.forms[1].us_access_signup.checked = false;\n";
		pResponse->strContent += "}\n\n";

		pResponse->strContent += "function admin() {\n";
		pResponse->strContent += "  document.forms[1].us_access_view.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_dl.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_comments.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_upload.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_edit.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_admin.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_signup.checked = false;\n";
		pResponse->strContent += "}\n\n";

		pResponse->strContent += "function signup() {\n";
		pResponse->strContent += "  document.forms[1].us_access_view.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_dl.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_comments.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_upload.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_edit.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_admin.checked = true;\n";
		pResponse->strContent += "  document.forms[1].us_access_signup.checked = true;\n";
		pResponse->strContent += "}\n\n";

		pResponse->strContent += "function clearAll() {\n";
		pResponse->strContent += "  document.forms[1].us_access_view.checked = false;\n";
		pResponse->strContent += "  document.forms[1].us_access_dl.checked = false;\n";
		pResponse->strContent += "  document.forms[1].us_access_comments.checked = false;\n";
		pResponse->strContent += "  document.forms[1].us_access_upload.checked = false;\n";
		pResponse->strContent += "  document.forms[1].us_access_edit.checked = false;\n";
		pResponse->strContent += "  document.forms[1].us_access_admin.checked = false;\n";
		pResponse->strContent += "  document.forms[1].us_access_signup.checked = false;\n";
		pResponse->strContent += "}\n\n";
		
		pResponse->strContent += "function pasteUsername() {\n";
		pResponse->strContent += "  if ( navigator.appName == \"Microsoft Internet Explorer\" ) {\n";
		pResponse->strContent += "    Pasted = document.createuser.us_login.createTextRange();\n";
		pResponse->strContent += "    Pasted.execCommand(\"Paste\");\n";
		pResponse->strContent += "  }\n";
		pResponse->strContent += "  else {\n";
		pResponse->strContent += "    alert(\"This feature only works with Microsoft Internet Explorer.\");\n";
		pResponse->strContent += "  }\n";
		pResponse->strContent += "}\n\n";

		pResponse->strContent += "function pastePasswords() {\n";
		pResponse->strContent += "  if ( navigator.appName == \"Microsoft Internet Explorer\" ) {\n";
		pResponse->strContent += "    Pasted = document.createuser.us_password.createTextRange();\n";
		pResponse->strContent += "    Pasted.execCommand(\"Paste\");\n";
		pResponse->strContent += "    Pasted = document.createuser.us_password_verify.createTextRange();\n";
		pResponse->strContent += "    Pasted.execCommand(\"Paste\");\n";
		pResponse->strContent += "  }\n";
		pResponse->strContent += "  else {\n";
		pResponse->strContent += "    alert(\"This feature only works with Microsoft Internet Explorer.\");\n";
		pResponse->strContent += "  }\n";
		pResponse->strContent += "}\n\n";

		pResponse->strContent += "function pasteEmail() {\n";
		pResponse->strContent += "  if ( navigator.appName == \"Microsoft Internet Explorer\" ) {\n";
		pResponse->strContent += "    Pasted = document.createuser.us_email.createTextRange();\n";
		pResponse->strContent += "    Pasted.execCommand(\"Paste\");\n";
		pResponse->strContent += "  }\n";
		pResponse->strContent += "  else {\n";
		pResponse->strContent += "    alert(\"This feature only works with Microsoft Internet Explorer.\");\n";
		pResponse->strContent += "  }\n";
		pResponse->strContent += "}\n\n";
		
		pResponse->strContent += "function pastePasswordsEdit() {\n";
		pResponse->strContent += "  if ( navigator.appName == \"Microsoft Internet Explorer\" ) {\n";
		pResponse->strContent += "    Pasted = document.edituser.us_password.createTextRange();\n";
		pResponse->strContent += "    Pasted.execCommand(\"Paste\");\n";
		pResponse->strContent += "    Pasted = document.edituser.us_password_verify.createTextRange();\n";
		pResponse->strContent += "    Pasted.execCommand(\"Paste\");\n";
		pResponse->strContent += "  }\n";
		pResponse->strContent += "  else {\n";
		pResponse->strContent += "    alert(\"This feature only works with Microsoft Internet Explorer.\");\n";
		pResponse->strContent += "  }\n";
		pResponse->strContent += "}\n\n";

		pResponse->strContent += "function pasteEmailEdit() {\n";
		pResponse->strContent += "  if ( navigator.appName == \"Microsoft Internet Explorer\" ) {\n";
		pResponse->strContent += "    Pasted = document.edituser.us_email.createTextRange();\n";
		pResponse->strContent += "    Pasted.execCommand(\"Paste\");\n";
		pResponse->strContent += "  }\n";
		pResponse->strContent += "  else {\n";
		pResponse->strContent += "    alert(\"This feature only works with Microsoft Internet Explorer.\");\n";
		pResponse->strContent += "  }\n";
		pResponse->strContent += "}\n\n";


		pResponse->strContent += "</script>\n\n";

	if( user.iAccess & ACCESS_ADMIN )
	{
		//
		// create user
		//

		if( pRequest->mapParams.find( "us_login" ) != pRequest->mapParams.end( ) &&
			pRequest->mapParams.find( "us_password" ) != pRequest->mapParams.end( ) &&
			pRequest->mapParams.find( "us_password_verify" ) != pRequest->mapParams.end( ) &&
			pRequest->mapParams.find( "us_email" ) != pRequest->mapParams.end( ) )
		{
			string strLogin = pRequest->mapParams["us_login"];
			string strPass = pRequest->mapParams["us_password"];
			string strPass2 = pRequest->mapParams["us_password_verify"];
			string strMail = pRequest->mapParams["us_email"];
			string strAccView = pRequest->mapParams["us_access_view"];
			string strAccDL = pRequest->mapParams["us_access_dl"];
			string strAccComments = pRequest->mapParams["us_access_comments"];
			string strAccUpload = pRequest->mapParams["us_access_upload"];
			string strAccEdit = pRequest->mapParams["us_access_edit"];
			string strAccAdmin = pRequest->mapParams["us_access_admin"];
			string strAccSignup = pRequest->mapParams["us_access_signup"];

			if( strLogin.empty( ) || strPass.empty( ) || strPass2.empty( ) || strMail.empty( ) )
			{
				pResponse->strContent += "<p>Unable to create user. You must fill in all the fields. Click <a href=\"/users.html\">here</a> to return to the users page.</p>\n";

				// The Trinity Edition - Addition Begins
				// The following presents JavaScript popups 
				// to automate navigation during user create failure

				pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
				pResponse->strContent += "  if ( confirm (\"FAILURE! \\n\\n You must complete all form fields. \\n\\n Press OK to Make Corrections. \\n Press CANCEL to Start Over.\") ) {\n";
				pResponse->strContent += "       window.history.back();\n";
				pResponse->strContent += "  }\n";
				pResponse->strContent += "  else {\n";
				pResponse->strContent += "       document.location = \"/users.html\";\n";
				pResponse->strContent += "  }\n";
				pResponse->strContent += "</script>\n\n";

				// ---------------------------------------------------- END OF ADDITION

				pResponse->strContent += "</body>\n";
				pResponse->strContent += "</html>\n";

				return;
			}
			else
			{
				if( strPass == strPass2 )
				{
					if( m_pUsers->getItem( strLogin ) )
					{
						pResponse->strContent += "<p>Unable to create user. The user \"" + UTIL_RemoveHTML( strLogin ) + "\" already exists. Click <a href=\"/users.html\">here</a> to return to the users page.</p>\n";

						// The Trinity Edition - Addition Begins
						// The following presents JavaScript popups 
						// to automate navigation during user create failure

						pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
						pResponse->strContent += "  if ( confirm (\"FAILURE! \\n\\n The user \'" + UTIL_RemoveHTML( strLogin ) + "\' already exists. \\n\\n Press OK to Make Corrections. \\n Press CANCEL to Start Over.\") ) {\n";
						pResponse->strContent += "       window.history.back();\n";
						pResponse->strContent += "  }\n";
						pResponse->strContent += "  else {\n";
						pResponse->strContent += "       document.location = \"/users.html\";\n";
						pResponse->strContent += "  }\n";
						pResponse->strContent += "</script>\n\n";

						// ---------------------------------------------------- END OF ADDITION

						pResponse->strContent += "</body>\n";
						pResponse->strContent += "</html>\n";

						return;
					}
					else
					{
						int iAccess = 0;

						if( strAccView == "on" )
							iAccess += ACCESS_VIEW;
						if( strAccDL == "on" )
							iAccess += ACCESS_DL;
						if( strAccComments == "on" )
							iAccess += ACCESS_COMMENTS;
						if( strAccUpload == "on" )
							iAccess += ACCESS_UPLOAD;
						if( strAccEdit == "on" )
							iAccess += ACCESS_EDIT;
						if( strAccAdmin == "on" )
							iAccess += ACCESS_ADMIN;
						if( strAccSignup == "on" )
							iAccess += ACCESS_SIGNUP;

						addUser( strLogin, strPass, iAccess, strMail );

						pResponse->strContent += "<p>Created user \"" + UTIL_RemoveHTML( strLogin ) + "\". Click <a href=\"/users.html\">here</a> to return to the users page.</p>\n";

						// The Trinity Edition - Addition Begins
						// The following presents JavaScript popups 
						// to automate navigation after creating a new user

						pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
						pResponse->strContent += "  alert(\"SUCCESS! \\n\\n Created New User: \\\"" + UTIL_RemoveHTML( strLogin ) + "\\\".\");\n";
						pResponse->strContent += "  document.location = \"/users.html\";\n";
						pResponse->strContent += "</script>\n\n";

						// ---------------------------------------------------- END OF ADDITION


						pResponse->strContent += "</body>\n";
						pResponse->strContent += "</html>\n";

						return;
					}
				}
				else
				{
					pResponse->strContent += "<p>Unable to create user. The passwords did not match. Click <a href=\"/users.html\">here</a> to return to the users page.</p>\n";

					// The Trinity Edition - Addition Begins
					// The following presents JavaScript popups 
					// to automate navigation during user create failure

					pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
					pResponse->strContent += "  if ( confirm (\"FAILURE! \\n\\n The passwords did not match. \\n\\n Press OK to Make Corrections. \\n Press CANCEL to Start Over.\") ) {\n";
					pResponse->strContent += "       window.history.back();\n";
					pResponse->strContent += "  }\n";
					pResponse->strContent += "  else {\n";
					pResponse->strContent += "       document.location = \"/users.html\";\n";
					pResponse->strContent += "  }\n";
					pResponse->strContent += "</script>\n\n";

					// ---------------------------------------------------- END OF ADDITION

					pResponse->strContent += "</body>\n";
					pResponse->strContent += "</html>\n";

					return;
				}
			}
		}

		//
		// edit user
		//

		string strPass = pRequest->mapParams["us_password"];
		string strPass2 = pRequest->mapParams["us_password_verify"];
		string strMail = pRequest->mapParams["us_email"];
		string strAccView = pRequest->mapParams["us_access_view"];
		string strAccDL = pRequest->mapParams["us_access_dl"];
		string strAccComments = pRequest->mapParams["us_access_comments"];
		string strAccUpload = pRequest->mapParams["us_access_upload"];
		string strAccEdit = pRequest->mapParams["us_access_edit"];
		string strAccAdmin = pRequest->mapParams["us_access_admin"];
		string strAccSignup = pRequest->mapParams["us_access_signup"];

		string strUser = pRequest->mapParams["user"];
		string strAction = pRequest->mapParams["action"];
		string strOK = pRequest->mapParams["ok"];

		if( strAction == "edit" )
		{
			CAtom *pUserToEdit = m_pUsers->getItem( strUser );

			if( pUserToEdit && pUserToEdit->isDicti( ) )
			{
				if( strOK == "1" )
				{
					// edit password

					if( !strPass.empty( ) && !strPass2.empty( ) )
					{
						if( strPass == strPass2 )
						{
							string strA1 = strUser + ":" + gstrRealm + ":" + strPass;

							unsigned char szMD5[16];

							MD5_CTX md5;

							MD5Init( &md5 );
							MD5Update( &md5, (unsigned char *)strA1.c_str( ), strA1.size( ) );
							MD5Final( szMD5, &md5 );

							( (CAtomDicti *)pUserToEdit )->setItem( "md5", new CAtomString( string( (char *)szMD5, 16 ) ) );
						}
						else
						{
							pResponse->strContent += "<p>Unable to edit user. The passwords did not match. Click <a href=\"/users.html\">here</a> to return to the users page.</p>\n";

							// The Trinity Edition - Addition Begins
							// The following presents JavaScript popups 
							// to automate navigation during user edit failure

							pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
							pResponse->strContent += "  if ( confirm (\"FAILURE! \\n\\n The passwords did not match. \\n\\n Press OK to Make Corrections. \\n Press CANCEL to Start Over.\") ) {\n";
							pResponse->strContent += "       window.history.back();\n";
							pResponse->strContent += "  }\n";
							pResponse->strContent += "  else {\n";
							pResponse->strContent += "       document.location = \"/users.html?user=" + UTIL_RemoveHTML( strUser ) + "&action=edit\";\n";
							pResponse->strContent += "  }\n";
							pResponse->strContent += "</script>\n\n";

							// ---------------------------------------------------- END OF ADDITION

							pResponse->strContent += "</body>\n";
							pResponse->strContent += "</html>\n";

							return;
						}
					}

					// edit mail

					( (CAtomDicti *)pUserToEdit )->setItem( "email", new CAtomString( strMail ) );

					// edit access

					int iAccess = 0;

					if( strAccView == "on" )
						iAccess += ACCESS_VIEW;
					if( strAccDL == "on" )
						iAccess += ACCESS_DL;
					if( strAccComments == "on" )
						iAccess += ACCESS_COMMENTS;
					if( strAccUpload == "on" )
						iAccess += ACCESS_UPLOAD;
					if( strAccEdit == "on" )
						iAccess += ACCESS_EDIT;
					if( strAccAdmin == "on" )
						iAccess += ACCESS_ADMIN;
					if( strAccSignup == "on" )
						iAccess += ACCESS_SIGNUP;

					( (CAtomDicti *)pUserToEdit )->setItem( "access", new CAtomLong( iAccess ) );

					saveUsers( );

					pResponse->strContent += "<p>Edited user \"" + UTIL_RemoveHTML( strUser ) + "\". Click <a href=\"/users.html\">here</a> to return to the users page.</p>\n";

					// The Trinity Edition - Addition Begins
					// The following presents JavaScript popups 
					// to automate navigation during user edit

					pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
					pResponse->strContent += "  alert (\"User \\\"" + UTIL_RemoveHTML( strUser ) + "\\\" has been SUCCESSFULLY edited.\");\n";
					pResponse->strContent += "  document.location = \"/users.html\";\n";
					pResponse->strContent += "</script>\n\n";

					// ---------------------------------------------------- END OF ADDITION

					pResponse->strContent += "</body>\n";
					pResponse->strContent += "</html>\n";

					return;
				}
				else
				{
					int iAccess = 0;

					CAtom *pAccessToEdit = ( (CAtomDicti *)pUserToEdit )->getItem( "access" );

					if( pAccessToEdit && pAccessToEdit->isLong( ) )
						iAccess = (int)( (CAtomLong *)pAccessToEdit )->getValue( );

					// The Trinity Edition - Modification Begins

					// Added Header Text
					
					pResponse->strContent += "<h3>Edit Existing User</h3>\n";

					pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
					pResponse->strContent += "  document.navbar.vru.disabled = true;\n\n";

					pResponse->strContent += "  function us_loginHelp() {\n";
					pResponse->strContent += "    alert(\"NOTE: \\n\\n You cannot change a user's login. \\n\\n You can only delete the current user and create a new one. \\n\\n\");\n";
					pResponse->strContent += "  }\n";
					pResponse->strContent += "</script>\n\n";

					pResponse->strContent += "<form name=\"edituser\" class=\"edituser\" method=\"get\" action=\"/users.html\">\n";


					pResponse->strContent += "<input name=\"user\" type=hidden value=\"" + UTIL_StringToEscaped( strUser ) + "\">\n";
					pResponse->strContent += "<input name=\"action\" type=hidden value=\"edit\">\n";
					pResponse->strContent += "<input name=\"ok\" type=hidden value=1>\n";

					pResponse->strContent += "<table style=\"display:inline\" class=\"edituser\">\n";
					pResponse->strContent += "<tr class=\"edituser\">\n";
					pResponse->strContent += "  <td class=\"edituser\" style=\"text-align:right\">Username/Login</td>\n";
					pResponse->strContent += "  <td class=\"edituser\"><input disabled name=\"us_login\" type=text size=24 value=\" " + UTIL_StringToEscaped( strUser ) + "\"> <input type=\"button\" value=\"Help\" onClick=\"us_loginHelp()\"></td></tr>\n";
					pResponse->strContent += "<tr class=\"edituser\">\n";
					pResponse->strContent += "  <td class=\"edituser\" style=\"text-align:right\">Password (leave blank for no change)</td>\n";
					pResponse->strContent += "  <td class=\"edituser\"><input name=\"us_password\" type=password size=20>  <input type=\"button\" name=\"pastepasswordsedit\" value=\"Paste\" onClick=\"pastePasswordsEdit()\"></td></tr>\n";
					pResponse->strContent += "<tr class=\"edituser\">\n";
					pResponse->strContent += "  <td class=\"edituser\" style=\"text-align:right\">Password Again (leave blank for no change)</td>\n";
					pResponse->strContent += "  <td class=\"edituser\"><input name=\"us_password_verify\" type=password size=20></td></tr>\n";
					pResponse->strContent += "<tr class=\"edituser\">\n";
					pResponse->strContent += "  <td class=\"edituser\" style=\"text-align:right\">Email Address</td>\n";
					pResponse->strContent += "  <td class=\"edituser\"><input name=\"us_email\" type=text size=40";

					CAtom *pMailToEdit = ( (CAtomDicti *)pUserToEdit )->getItem( "email" );

					if( pMailToEdit )
						pResponse->strContent += " value=\"" + pMailToEdit->toString( ) + "\"";

					pResponse->strContent += ">  <input type=\"button\" name=\"pasteemailedit\" value=\"Paste\" onClick=\"pasteEmailEdit()\"></td></tr>\n";

					pResponse->strContent += "</table>\n\n";

					pResponse->strContent += "<table class=\"edituser\"><tr class=\"edituser\"><td class=\"edituser\" style=\"vertical-align:top\">\n";
					pResponse->strContent += "<input name=\"us_access_view\" type=checkbox";

					if( iAccess & ACCESS_VIEW )
						pResponse->strContent += " checked";

					pResponse->strContent += "> View Access (Basic)<hr>\n";

					pResponse->strContent += "<input name=\"us_access_dl\" type=checkbox";

					if( iAccess & ACCESS_DL )
						pResponse->strContent += " checked";

					pResponse->strContent += "> DL Access (Downloader)<hr>\n";

					pResponse->strContent += "<input name=\"us_access_comments\" type=checkbox";

					if( iAccess & ACCESS_COMMENTS )
						pResponse->strContent += " checked";

					pResponse->strContent += "> Comments Access (Poster)<hr>\n";

					pResponse->strContent += "<input name=\"us_access_upload\" type=checkbox";

					if( iAccess & ACCESS_UPLOAD )
						pResponse->strContent += " checked";

					pResponse->strContent += "> Upload Access (Uploader)<hr>\n";

					pResponse->strContent += "<input name=\"us_access_edit\" type=checkbox";

					if( iAccess & ACCESS_EDIT )
						pResponse->strContent += " checked";

					pResponse->strContent += "> Edit Access (Moderator)<hr>\n";

					pResponse->strContent += "<input name=\"us_access_admin\" type=checkbox";

					if( iAccess & ACCESS_ADMIN )
						pResponse->strContent += " checked";

					pResponse->strContent += "> Admin Access (Admin)<hr>\n";

					pResponse->strContent += "<input name=\"us_access_signup\" type=checkbox";

					if( iAccess & ACCESS_SIGNUP )
						pResponse->strContent += " checked";

					pResponse->strContent += "> Signup Access<hr><br>\n";

					pResponse->strContent += "<input type=submit value=\"Save Changes\">\n";
					pResponse->strContent += "<input type=\"button\" value=\"Discard Changes\" onClick=\"window.history.back()\">\n";

					pResponse->strContent += "</td><td class=\"edituser\" style=\"text-align:right;vertical-align:top\">\n";
					pResponse->strContent += "<input type=\"button\" value=\"Basic (1)\" onClick=\"basic()\"><hr>\n";
					pResponse->strContent += "<input type=\"button\" value=\"Downloader ( 2 + 1 )\" onClick=\"downloader()\"><hr>\n";
					pResponse->strContent += "<input type=\"button\" value=\"Poster ( 4 + 2 + 1 )\" onClick=\"poster()\"><hr>\n";
					pResponse->strContent += "<input type=\"button\" value=\"Uploader ( 8 + 4 + 2 + 1 )\" onClick=\"uploader()\"><hr>\n";
					pResponse->strContent += "<input type=\"button\" value=\"Moderator ( 16 + 8 + 4 + 2 + 1 )\" onClick=\"moderator()\"><hr>\n";
					pResponse->strContent += "<input type=\"button\" value=\"Admin ( 32 + 16 + 8 + 4 + 2 + 1 )\" onClick=\"admin()\"><hr>\n";
					pResponse->strContent += "<input type=\"button\" value=\"Signup ( 64 + 32 + 16 + 8 + 4 + 2 + 1 )\" onClick=\"signup()\"><hr><br>\n";
					pResponse->strContent += "<input type=\"button\" value=\"Delete User\" onClick=\"document.location='/users.html?user=" + UTIL_StringToEscaped( strUser ) + "&action=delete'\">\n";
					pResponse->strContent += "<input type=\"button\" value=\"Clear All Checkboxes\" onClick=\"clearAll()\">\n";
					pResponse->strContent += "</td></tr></table>\n\n";
					pResponse->strContent += "</form>\n";		



					/* Original Source Code:
					pResponse->strContent += "<form method=\"get\" action=\"/users.html\">\n";
					pResponse->strContent += "<p><strong>Edit User \"" + UTIL_RemoveHTML( strUser ) + "\"</strong></p>\n";
					pResponse->strContent += "<input name=\"user\" type=hidden value=\"" + UTIL_StringToEscaped( strUser ) + "\">\n";
					pResponse->strContent += "<input name=\"action\" type=hidden value=\"edit\">\n";
					pResponse->strContent += "<input name=\"ok\" type=hidden value=1>\n";
					pResponse->strContent += "<input name=\"us_password\" type=password size=20> Password (optional)<br>\n";
					pResponse->strContent += "<input name=\"us_password_verify\" type=password size=20> Verify Password (optional)<br><br>\n";
					pResponse->strContent += "<input name=\"us_email\" type=text size=40";

					CAtom *pMailToEdit = ( (CAtomDicti *)pUserToEdit )->getItem( "email" );

					if( pMailToEdit )
						pResponse->strContent += " value=\"" + pMailToEdit->toString( ) + "\"";

					pResponse->strContent += "> E-Mail<br><br>\n";
					pResponse->strContent += "<input name=\"us_access_view\" type=checkbox";

					if( iAccess & ACCESS_VIEW )
						pResponse->strContent += " checked";

					pResponse->strContent += "> View Access (Basic)<br>\n";
					pResponse->strContent += "<input name=\"us_access_dl\" type=checkbox";

					if( iAccess & ACCESS_DL )
						pResponse->strContent += " checked";

					pResponse->strContent += "> DL Access (Downloader)<br>\n";
					pResponse->strContent += "<input name=\"us_access_comments\" type=checkbox";

					if( iAccess & ACCESS_COMMENTS )
						pResponse->strContent += " checked";

					pResponse->strContent += "> Comments Access (Poster)<br>\n";
					pResponse->strContent += "<input name=\"us_access_upload\" type=checkbox";

					if( iAccess & ACCESS_UPLOAD )
						pResponse->strContent += " checked";

					pResponse->strContent += "> Upload Access (Uploader)<br>\n";
					pResponse->strContent += "<input name=\"us_access_edit\" type=checkbox";

					if( iAccess & ACCESS_EDIT )
						pResponse->strContent += " checked";

					pResponse->strContent += "> Edit Access (Moderator)<br>\n";
					pResponse->strContent += "<input name=\"us_access_admin\" type=checkbox";

					if( iAccess & ACCESS_ADMIN )
						pResponse->strContent += " checked";

					pResponse->strContent += "> Admin Access (Admin)<br>\n";
					pResponse->strContent += "<input name=\"us_access_signup\" type=checkbox";

					if( iAccess & ACCESS_SIGNUP )
						pResponse->strContent += " checked";

					pResponse->strContent += "> Signup Access<br><br>\n";
					pResponse->strContent += "<input type=submit value=\"Edit User\">\n";
					*/

					pResponse->strContent += "</form>\n";
					pResponse->strContent += "</body>\n";
					pResponse->strContent += "</html>\n";

					return;
				}
			}
			else
			{
				pResponse->strContent += "<p>Unable to edit user. The user \"" + UTIL_RemoveHTML( strUser ) + "\" does not exist. Click <a href=\"/users.html\">here</a> to return to the users page.</p>\n";
		
				// The Trinity Edition - Addition Begins
				// The following presents JavaScript popups 
				// to automate navigation during user edit failure

				pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
				pResponse->strContent += "  alert(\"FAILURE! \\n\\n The user \"" + UTIL_RemoveHTML( strUser ) + "\" does not exist.\");\n";
				pResponse->strContent += "  document.location = \"/users.html\";\n";
				pResponse->strContent += "</script>\n\n";

				// ---------------------------------------------------- END OF ADDITION

				pResponse->strContent += "</body>\n";
				pResponse->strContent += "</html>\n";

				return;
			}
		}

		//
		// delete user
		//

		else if( strAction == "delete" )
		{
			if( strOK == "1" )
			{
				deleteUser( strUser );

				pResponse->strContent += "<p>Deleted user \"" + UTIL_RemoveHTML( strUser ) + "\". Click <a href=\"/users.html\">here</a> to return to the users page.</p>\n";

				// The Trinity Edition - Addition Begins
				// The following presents JavaScript popups 
				// to automate navigation after deleting user

				pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
				pResponse->strContent += "  alert (\"User \\\"" + UTIL_RemoveHTML( strUser ) + "\\\" has been deleted.\");\n";
				pResponse->strContent += "  document.location = \"/users.html\";\n";
				pResponse->strContent += "</script>\n\n";

				// ---------------------------------------------------- END OF ADDITION

				pResponse->strContent += "</body>\n";
				pResponse->strContent += "</html>\n";

				return;
			}
			else
			{
				pResponse->strContent += "<p>Are you sure you want to delete the user \"" + UTIL_RemoveHTML( strUser ) + "\"? WARNING! If there are no admin users, you won't be able to administrate your tracker!</p>\n";
				pResponse->strContent += "<p><a href=\"/users.html?user=" + UTIL_StringToEscaped( strUser ) + "&action=delete&ok=1\">OK</a></p>\n";

				// The Trinity Edition - Addition Begins
				// The following presents JavaScript popups 
				// to automate navigation when deleting user

				pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
				pResponse->strContent += "  if ( confirm (\"DELETE USER CONFIRMATION! \\n\\n WARNING: If there are no admin users, you will not \\n be able to administrate your tracker. \\n\\n Are you sure you want to delete the user \\\"" + UTIL_RemoveHTML( strUser ) + "\\\"? \\n\\n \") ) {\n";
				pResponse->strContent += "       document.location = \"/users.html?user=" + UTIL_StringToEscaped( strUser ) + "&action=delete&ok=1\";\n";
				pResponse->strContent += "  }\n";
				pResponse->strContent += "  else {\n";
				pResponse->strContent += "       window.history.back();\n";
				pResponse->strContent += "  }\n";
				pResponse->strContent += "</script>\n\n";

				// ---------------------------------------------------- END OF ADDITION

				pResponse->strContent += "</body>\n";
				pResponse->strContent += "</html>\n";

				return;
			}
		}

		//
		// create user
		//

		// The Trinty Edition - Modification Begins
		// Reformats the form into table cells for better visual flow
		// Adds a name value to the form for JavaScripting reference
		// Adds "CatchAll" buttons.
		// Removes the "Create User" which has been replaced

		pResponse->strContent += "<h3>Create New User</h3>\n";

		pResponse->strContent += "<form name=\"createuser\" class=\"createuser\" method=\"get\" action=\"/users.html\">\n";
		pResponse->strContent += "<table style=\"display:inline\" class=\"createuser\">\n";
		pResponse->strContent += "<tr class=\"createuser\">\n";
		pResponse->strContent += "  <td class=\"createuser\" style=\"text-align:right\">Username/Login</td>\n";
		pResponse->strContent += "  <td class=\"createuser\"><input name=\"us_login\" type=text size=24> <input type=\"button\" name=\"pasteusername\" value=\"Paste\" onClick=\"pasteUsername()\"></td></tr>\n";
		pResponse->strContent += "<tr class=\"createuser\">\n";
		pResponse->strContent += "  <td class=\"createuser\" style=\"text-align:right\">Password</td>\n";
		pResponse->strContent += "  <td class=\"createuser\"><input name=\"us_password\" type=password size=20> <input type=\"button\" name=\"pastepasswords\" value=\"Paste\" onClick=\"pastePasswords()\"></td></tr>\n";
		pResponse->strContent += "<tr class=\"createuser\">\n";
		pResponse->strContent += "  <td class=\"createuser\" style=\"text-align:right\">Password Again</td>\n";
		pResponse->strContent += "  <td class=\"createuser\"><input name=\"us_password_verify\" type=password size=20></td></tr>\n";
		pResponse->strContent += "<tr class=\"createuser\">\n";
		pResponse->strContent += "  <td class=\"createuser\" style=\"text-align:right\">Email Address</td>\n";
		pResponse->strContent += "  <td class=\"createuser\"><input name=\"us_email\" type=text size=40> <input type=\"button\" name=\"pasteemail\" value=\"Paste\" onClick=\"pasteEmail()\"></td></tr>\n";
		pResponse->strContent += "</table>\n\n";

		pResponse->strContent += "<table class=\"createuser\"><tr class=\"createuser\"><td class=\"createuser\" style=\"vertical-align:top\">\n";
		pResponse->strContent += "<input name=\"us_access_view\" type=checkbox> View Access (Basic)<hr>\n";
		pResponse->strContent += "<input name=\"us_access_dl\" type=checkbox> DL Access (Downloader)<hr>\n";
		pResponse->strContent += "<input name=\"us_access_comments\" type=checkbox> Comments Access (Poster)<hr>\n";
		pResponse->strContent += "<input name=\"us_access_upload\" type=checkbox> Upload Access (Uploader)<hr>\n";
		pResponse->strContent += "<input name=\"us_access_edit\" type=checkbox> Edit Access (Moderator)<hr>\n";
		pResponse->strContent += "<input name=\"us_access_admin\" type=checkbox> Admin Access (Admin)<hr>\n";
		pResponse->strContent += "<input name=\"us_access_signup\" type=checkbox> Signup Access<hr><br>\n";
		pResponse->strContent += "<input type=submit value=\"Create User\">\n";
		pResponse->strContent += "</td><td class=\"createuser\" style=\"text-align:right;vertical-align:top\">\n";
		pResponse->strContent += "<input type=\"button\" value=\"Basic (1)\" onClick=\"basic()\"><hr>\n";
		pResponse->strContent += "<input type=\"button\" value=\"Downloader ( 2 + 1 )\" onClick=\"downloader()\"><hr>\n";
		pResponse->strContent += "<input type=\"button\" value=\"Poster ( 4 + 2 + 1 )\" onClick=\"poster()\"><hr>\n";
		pResponse->strContent += "<input type=\"button\" value=\"Uploader ( 8 + 4 + 2 + 1 )\" onClick=\"uploader()\"><hr>\n";
		pResponse->strContent += "<input type=\"button\" value=\"Moderator ( 16 + 8 + 4 + 2 + 1 )\" onClick=\"moderator()\"><hr>\n";
		pResponse->strContent += "<input type=\"button\" value=\"Admin ( 32 + 16 + 8 + 4 + 2 + 1 )\" onClick=\"admin()\"><hr>\n";
		pResponse->strContent += "<input type=\"button\" value=\"Signup ( 64 + 32 + 16 + 8 + 4 + 2 + 1 )\" onClick=\"signup()\"><hr><br>\n";
		pResponse->strContent += "<input type=\"button\" value=\"Clear All Checkboxes\" onClick=\"clearAll()\">\n";
		pResponse->strContent += "</td></tr></table>\n\n";
		pResponse->strContent += "</form>\n";

		// Sets an internal document anchor

		pResponse->strContent += "<a name=\"userlist\"></a><hr>\n\n";

		/* Original Source Code:
		pResponse->strContent += "<form method=\"get\" action=\"/users.html\">\n";
		pResponse->strContent += "<p><strong>Create User</strong></p>\n";
		pResponse->strContent += "<input name=\"us_login\" type=text size=24> Login<br><br>\n";
		pResponse->strContent += "<input name=\"us_password\" type=password size=20> Password<br>\n";
		pResponse->strContent += "<input name=\"us_password_verify\" type=password size=20> Verify Password<br><br>\n";
		pResponse->strContent += "<input name=\"us_email\" type=text size=40> E-Mail<br><br>\n";
		pResponse->strContent += "<input name=\"us_access_view\" type=checkbox> View Access (Basic)<br>\n";
		pResponse->strContent += "<input name=\"us_access_dl\" type=checkbox> DL Access (Downloader)<br>\n";
		pResponse->strContent += "<input name=\"us_access_comments\" type=checkbox> Comments Access (Poster)<br>\n";
		pResponse->strContent += "<input name=\"us_access_upload\" type=checkbox> Upload Access (Uploader)<br>\n";
		pResponse->strContent += "<input name=\"us_access_edit\" type=checkbox> Edit Access (Moderator)<br>\n";
		pResponse->strContent += "<input name=\"us_access_admin\" type=checkbox> Admin Access (Admin)<br>\n";
		pResponse->strContent += "<input name=\"us_access_signup\" type=checkbox> Signup Access<br><br>\n";
		pResponse->strContent += "<input type=submit value=\"Create User\">\n";
		pResponse->strContent += "</form>\n";
		*/

		// ---------------------------------------------------------------- END OF MODIFICATION

		//
		// user table
		//

		if( m_pUsers )
		{
			if( m_pUsers->isEmpty( ) )
			{
				pResponse->strContent += "<p><strong>WARNING! Your tracker does not have any users. Guests will have full access until someone creates the first user.</strong></p>\n";
				pResponse->strContent += "</body>\n";
				pResponse->strContent += "</html>\n";

				return;
			}

			map<string, CAtom *> mapDicti = m_pUsers->getValue( );

			unsigned long iKeySize = mapDicti.size( );

			// add the users into this structure one by one and sort it afterwards

			struct user_t *pUsersT = new struct user_t[iKeySize];

			unsigned long i = 0;

			for( map<string, CAtom *> :: iterator it = mapDicti.begin( ); it != mapDicti.end( ); it++ )
			{
				pUsersT[i].strLogin = (*it).first;
				pUsersT[i].strLowerLogin = UTIL_ToLower( pUsersT[i].strLogin );
				pUsersT[i].iAccess = m_iGuestAccess;

				if( (*it).second->isDicti( ) )
				{
					CAtom *pMD5 = ( (CAtomDicti *)(*it).second )->getItem( "md5" );
					CAtom *pAccess = ( (CAtomDicti *)(*it).second )->getItem( "access" );
					CAtom *pMail = ( (CAtomDicti *)(*it).second )->getItem( "email" );
					CAtom *pCreated = ( (CAtomDicti *)(*it).second )->getItem( "created" );

					if( pMD5 )
						pUsersT[i].strMD5 = pMD5->toString( );

					if( pMail )
					{
						pUsersT[i].strMail = pMail->toString( );
						pUsersT[i].strLowerMail = UTIL_ToLower( pUsersT[i].strMail );
					}

					if( pAccess && pAccess->isLong( ) )
						pUsersT[i].iAccess = (int)( (CAtomLong *)pAccess )->getValue( );

					if( pCreated )
						pUsersT[i].strCreated = pCreated->toString( );
				}

				i++;
			}

			string strSort = pRequest->mapParams["sort"];

			if( !strSort.empty( ) )
			{
				int iSort = atoi( strSort.c_str( ) );

				if( iSort == SORTU_ALOGIN )
					qsort( pUsersT, iKeySize, sizeof( struct user_t ), asortuByLogin );
				else if( iSort == SORTU_AACCESS )
					qsort( pUsersT, iKeySize, sizeof( struct user_t ), asortuByAccess );
				else if( iSort == SORTU_AEMAIL )
					qsort( pUsersT, iKeySize, sizeof( struct user_t ), asortuByMail );
				else if( iSort == SORTU_ACREATED )
					qsort( pUsersT, iKeySize, sizeof( struct user_t ), asortuByCreated );
				else if( iSort == SORTU_DLOGIN )
					qsort( pUsersT, iKeySize, sizeof( struct user_t ), dsortuByLogin );
				else if( iSort == SORTU_DACCESS )
					qsort( pUsersT, iKeySize, sizeof( struct user_t ), dsortuByAccess );
				else if( iSort == SORTU_DEMAIL )
					qsort( pUsersT, iKeySize, sizeof( struct user_t ), dsortuByMail );
				else if( iSort == SORTU_DCREATED )
					qsort( pUsersT, iKeySize, sizeof( struct user_t ), dsortuByCreated );
				else
				{
					// default action is to sort by created

					qsort( pUsersT, iKeySize, sizeof( struct user_t ), dsortuByCreated );
				}
			}
			else
			{
				// default action is to sort by created

				qsort( pUsersT, iKeySize, sizeof( struct user_t ), dsortuByCreated );
			}

			// some preliminary search crap

			string strSearch = pRequest->mapParams["search"];
			string strLowerSearch = UTIL_ToLower( strSearch );
			string strSearchResp = UTIL_StringToEscaped( strSearch );

			if( !strSearch.empty( ) )
				pResponse->strContent += "<p class=\"search_results\">Search results for \"" + UTIL_RemoveHTML( strSearch ) + "\".</p>\n";

			// which page are we viewing

			unsigned int iStart = 0;

			if( m_iUsersPerPage > 0 )
			{
				string strPage = pRequest->mapParams["page"];

				if( !strPage.empty( ) )
					iStart = atoi( strPage.c_str( ) ) * m_iUsersPerPage;

				// The Trinity Edition - MOD BEGINS - changed element class to "pagenum_top_users"
				pResponse->strContent += "<p class=\"pagenum_top_users\">Page " + CAtomInt( ( iStart / m_iUsersPerPage ) + 1 ).toString( ) + "</p>\n";

				/* Original Source Code:
				pResponse->strContent += "<p class=\"pagenum_top\">Page " + CAtomInt( ( iStart / m_iUsersPerPage ) + 1 ).toString( ) + "</p>\n";
				*/
			}

			bool bFound = false;

			int iAdded = 0;
			int iSkipped = 0;

			// for correct page numbers after searching

			int iFound = 0;

			for( unsigned long i = 0; i < iKeySize; i++ )
			{
				if( !strSearch.empty( ) )
				{
					// only display entries that match the search

					if( pUsersT[i].strLowerLogin.find( strLowerSearch ) == string :: npos )
						continue;
				}

				iFound++;

				if( m_iUsersPerPage == 0 || iAdded < m_iUsersPerPage )
				{
					if( !bFound )
					{
						// output table headers

						// The Trinity Edition - Modification Begins
						// Changes "Users" to Header called "Registered Users"

						pResponse->strContent += "<h4>Registered Users</h4>\n";

						/* Original Source Code:
						pResponse->strContent += "<p>Users</p>\n";
						*/

						// ------------------------------------------- END OF MODIFICATION

						pResponse->strContent += "<table summary=\"users\">\n";
						pResponse->strContent += "<tr><th class=\"name\">Login";
						pResponse->strContent += "<br><a class=\"sort\" href=\"/users.html?sort=" + SORTUSTR_ALOGIN;

						if( !strSearch.empty( ) )
							pResponse->strContent += "&search=" + strSearchResp;

						pResponse->strContent += "\">A</a> <a class=\"sort\" href=\"/users.html?sort=" + SORTUSTR_DLOGIN;

						if( !strSearch.empty( ) )
							pResponse->strContent += "&search=" + strSearchResp;

						pResponse->strContent += "\">Z</a>";
						pResponse->strContent += "</th><th>Access";
						pResponse->strContent += "<br><a class=\"sort\" href=\"/users.html?sort=" + SORTUSTR_AACCESS;

						if( !strSearch.empty( ) )
							pResponse->strContent += "&search=" + strSearchResp;

						pResponse->strContent += "\">A</a> <a class=\"sort\" href=\"/users.html?sort=" + SORTUSTR_DACCESS;

						if( !strSearch.empty( ) )
							pResponse->strContent += "&search=" + strSearchResp;

						pResponse->strContent += "\">Z</a>";
						pResponse->strContent += "</th><th>E-Mail";
						pResponse->strContent += "<br><a class=\"sort\" href=\"/users.html?sort=" + SORTUSTR_AEMAIL;

						if( !strSearch.empty( ) )
							pResponse->strContent += "&search=" + strSearchResp;

						pResponse->strContent += "\">A</a> <a class=\"sort\" href=\"/users.html?sort=" + SORTUSTR_DEMAIL;

						if( !strSearch.empty( ) )
							pResponse->strContent += "&search=" + strSearchResp;

						pResponse->strContent += "\">Z</a>";
						pResponse->strContent += "</th><th>Created";
						pResponse->strContent += "<br><a class=\"sort\" href=\"/users.html?sort=" + SORTUSTR_ACREATED;

						if( !strSearch.empty( ) )
							pResponse->strContent += "&search=" + strSearchResp;

						pResponse->strContent += "\">A</a> <a class=\"sort\" href=\"/users.html?sort=" + SORTUSTR_DCREATED;

						if( !strSearch.empty( ) )
							pResponse->strContent += "&search=" + strSearchResp;

						pResponse->strContent += "\">Z</a>";
						pResponse->strContent += "</th><th>Admin</th></tr>\n";

						bFound = true;
					}

					if( iSkipped == (int)iStart )
					{
						// output table rows

						if( iAdded % 2 )
							pResponse->strContent += "<tr class=\"even\">";
						else
							pResponse->strContent += "<tr class=\"odd\">";

						pResponse->strContent += "<td class=\"name\">";
						pResponse->strContent += UTIL_RemoveHTML( pUsersT[i].strLogin );
						pResponse->strContent += "</td><td>";
						pResponse->strContent += UTIL_AccessToString( pUsersT[i].iAccess );
						pResponse->strContent += "</td><td>";
						pResponse->strContent += UTIL_RemoveHTML( pUsersT[i].strMail );
						pResponse->strContent += "</td><td>";

						if( !pUsersT[i].strCreated.empty( ) )
						{
							// strip year and seconds from time

							pResponse->strContent += pUsersT[i].strCreated.substr( 5, pUsersT[i].strCreated.size( ) - 8 );
						}

						// The Trinity Edition - Modification Begins
						// Adds a Send Email button to the Admin column
						// Turns the other admin links into buttons

						pResponse->strContent += "</td><td>\n";
						pResponse->strContent += "<input type=\"button\" value=\"Send Email\" onClick=\"document.location='mailto:" + UTIL_RemoveHTML( pUsersT[i].strMail ) + "'\">\n";
						pResponse->strContent += "<input type=\"button\" value=\"Edit\" onClick=\"document.location='/users.html?user=" + UTIL_StringToEscaped( pUsersT[i].strLogin ) + "&action=edit'\">\n";
						pResponse->strContent += "<input type=\"button\" value=\"Delete\" onClick=\"document.location='/users.html?user=" + UTIL_StringToEscaped( pUsersT[i].strLogin ) + "&action=delete'\">\n";
						pResponse->strContent += "</td></tr>\n";

						/* Original Source Code:
						pResponse->strContent += "</td><td>[<a href=\"/users.html?user=" + UTIL_StringToEscaped( pUsersT[i].strLogin ) + "&action=edit\">Edit</a>] [<a href=\"/users.html?user=" + UTIL_StringToEscaped( pUsersT[i].strLogin ) + "&action=delete\">Delete</a>]</td></tr>\n";
						*/

						iAdded++;
					}
					else
						iSkipped++;
				}
			}

			delete [] pUsersT;

			// some finishing touches

			if( bFound )
				pResponse->strContent += "</table>\n";

			pResponse->strContent += "<span class=\"search_users\"><form method=\"get\" action=\"/users.html\">\n";

			if( !strSort.empty( ) )
				pResponse->strContent += "<input name=\"sort\" type=hidden value=\"" + strSort + "\">\n";

			pResponse->strContent += "Search <input name=\"search\" type=text size=40> <a href=\"/users.html\">Clear Search</a>\n";
			pResponse->strContent += "</form></span>\n";

			// page numbers

			if( m_iUsersPerPage > 0 )
			{
				// The Trinity Edition - MOD BEGINS - Added "Go To Page:" and changed element class to "pagenum_bottom_users"
				pResponse->strContent += "<p class=\"pagenum_bottom_users\">Go To Page: ";

				/* Original Source Code:
				pResponse->strContent += "<p class=\"pagenum_bottom\">";
				*/

				for( unsigned long i = 0; i < (unsigned int)iFound; i += m_iUsersPerPage )
				{
					pResponse->strContent += " ";

					// don't link to current page

					if( i != iStart )
					{
						pResponse->strContent += "<a href=\"/users.html?page=" + CAtomInt( i / m_iUsersPerPage ).toString( );

						if( !strSort.empty( ) )
							pResponse->strContent += "&sort=" + strSort;

						if( !strSearch.empty( ) )
							pResponse->strContent += "&search=" + strSearchResp;

						pResponse->strContent += "\">";
					}

					pResponse->strContent += CAtomInt( ( i / m_iUsersPerPage ) + 1 ).toString( );

					if( i != iStart )
						pResponse->strContent += "</a>";

					pResponse->strContent += " ";

					// don't display a bar after the last page

					if( i + (unsigned int)m_iUsersPerPage < (unsigned int)iFound )
						pResponse->strContent += "|";
				}

				pResponse->strContent += "</p>\n";
			}
		}
	}
	else
		pResponse->strContent += "<p class=\"denied\">You are not authorized to view this page.</p>\n";

	pResponse->strContent += "</body>\n";
	pResponse->strContent += "</html>\n";
}
