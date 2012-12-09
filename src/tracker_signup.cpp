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

void CTracker :: serverResponseSignup( struct request_t *pRequest, struct response_t *pResponse, user_t user )
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
		pResponse->strContent += "<title>" + gstrTrackerTitle + " Signup</title>\n";
	}
	else {
        pResponse->strContent += "<title>BNBT Signup</title>\n";
	}

	// ------------------------------------------ END OF MOD

	if( !gstrStyle.empty( ) )
		pResponse->strContent += "<link rel=stylesheet type=\"text/css\" href=\"" + gstrStyle + "\">\n";

	// The Trinity Edition - Addition Begins
	// Adds a URL Redirect when "bnbt_alt_signup_url" is set

	if( !gstrAltSignupURL.empty( ) )
		pResponse->strContent += "<meta http-equiv=\"refresh\" content=\"0;" + gstrAltSignupURL + "\">\n\n";

	// ---------------------------------------- END OF ADDITION

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
		pResponse->strContent += "<p class=\"login1_signup\">You are not logged in. Click <a href=\"/login.html\">here</a> to login.</p>\n";
	else
		pResponse->strContent += "<p class=\"login2_signup\">You are logged in as <span class=\"username\">" + UTIL_RemoveHTML( user.strLogin ) + "</span>. Click <a href=\"/login.html?logout=1\">here</a> to logout.</p>\n";

}

	/* Original Source Code:
	if( user.strLogin.empty( ) )
		pResponse->strContent += "<p class=\"login1_signup\">You are not logged in. Click <a href=\"/login.html\">here</a> to login.</p>\n";
	else
		pResponse->strContent += "<p class=\"login2_signup\">You are logged in as <span class=\"username\">" + UTIL_RemoveHTML( user.strLogin ) + "</span>. Click <a href=\"/login.html?logout=1\">here</a> to logout.</p>\n";
	*/

// ------------------------------------------------- END OF MODIFICATION

// The Trinity Edition - Modification Begins
// Disables the Signup Page when an Alternate Signup URL is specified in the cofiguration...

if( !gstrAltSignupURL.empty( ) ) {
	pResponse->strContent += "<h3>You will now be redirected...</h3>\n";
}

else {

	// Static Signup Header Document (TRINCUSTSTATDOX)

	pResponse->strContent += m_strStaticHeaderSignup; // ADDED

	// Only Display <h3> header if Static Header is NOT being used

	if( m_strStaticHeaderSignupFile.empty( ) ) {
		if( !gstrTrackerTitle.empty( ) ) {
			pResponse->strContent += "<h3>" + gstrTrackerTitle + " Signup</h3>\n";
		}
		else {
			pResponse->strContent += "<h3>BNBT Signup</h3>\n";
		}
	}

	/* Original Source Code:
	pResponse->strContent += "<h3>BNBT Signup</h3>\n";
	*/

	// -------------------------- END OF MODS

	if( user.iAccess & ACCESS_SIGNUP )
	{
		if( pRequest->mapParams.find( "us_login" ) != pRequest->mapParams.end( ) &&
			pRequest->mapParams.find( "us_password" ) != pRequest->mapParams.end( ) &&
			pRequest->mapParams.find( "us_password_verify" ) != pRequest->mapParams.end( ) &&
			pRequest->mapParams.find( "us_email" ) != pRequest->mapParams.end( ) )
		{
			string strLogin = pRequest->mapParams["us_login"];
			string strPass = pRequest->mapParams["us_password"];
			string strPass2 = pRequest->mapParams["us_password_verify"];
			string strMail = pRequest->mapParams["us_email"];

			if( strLogin.empty( ) || strPass.empty( ) || strPass2.empty( ) || strMail.empty( ) )
			{
				pResponse->strContent += "<p>Unable to signup. You must fill in all the fields. Click <a href=\"/signup.html\">here</a> to return to the signup page.</p>\n";

				// The Trinity Edition - Addition Begins

				// The following presents a JS popup to notify and redirect

				pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
				pResponse->strContent += "  alert('Unable to signup. You must fill in all the fields. \\n\\n Press OK to Retry.');\n";
				pResponse->strContent += "  window.history.back();\n";
				pResponse->strContent += "</script>\n\n";

				// ------------------------------------------------- END OF ADDITION

				pResponse->strContent += "</body>\n";
				pResponse->strContent += "</html>\n";

				return;
			}
			else
			{
				if( strLogin[0] == ' ' || strLogin[strLogin.size( ) - 1] == ' ' || strLogin.size( ) > (unsigned int)m_iNameLength )
				{
					pResponse->strContent += "<p>Unable to signup. Your name must be less than " + CAtomInt( m_iNameLength ).toString( ) + " characters long and it must not start or end with spaces. Click <a href=\"/signup.html\">here</a> to return to the signup page.</p>\n";

					// The Trinity Edition - Addition Begins

					// The following presents a JS popup to notify and redirect

					pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
					pResponse->strContent += "  alert('Unable to signup. Your name must be less than " + CAtomInt( m_iNameLength ).toString( ) + " characters long and it must not start or end with spaces. \\n\\n Press OK to Retry.');\n";
					pResponse->strContent += "  window.history.back();\n";
					pResponse->strContent += "</script>\n\n";

					// ------------------------------------------------- END OF ADDITION

					pResponse->strContent += "</body>\n";
					pResponse->strContent += "</html>\n";

					return;
				}

				if( strMail.find( "@" ) == string :: npos || strMail.find( "." ) == string :: npos )
				{
					pResponse->strContent += "<p>Unable to signup. Your e-mail address is invalid. Click <a href=\"/signup.html\">here</a> to return to the signup page.</p>\n";

					// The Trinity Edition - Addition Begins

					// The following presents a JS popup to notify and redirect

					pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
					pResponse->strContent += "  alert('Unable to signup. Your e-mail address is invalid. \\n\\n Press OK to Retry.');\n";
					pResponse->strContent += "  window.history.back();\n";
					pResponse->strContent += "</script>\n\n";

					// ------------------------------------------------- END OF ADDITION
					pResponse->strContent += "</body>\n";
					pResponse->strContent += "</html>\n";

					return;
				}

				if( strPass == strPass2 )
				{
					if( m_pUsers->getItem( strLogin ) )
					{
						pResponse->strContent += "<p>Unable to signup. The user \"" + UTIL_RemoveHTML( strLogin ) + "\" already exists. Click <a href=\"/signup.html\">here</a> to return to the signup page.</p>\n";
					
						// The Trinity Edition - Addition Begins

						// The following presents a JS popup to notify and redirect

						pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
						pResponse->strContent += "  alert('Unable to signup. The user \\\"" + UTIL_RemoveHTML( strLogin ) + "\\\" already exists. \\n\\n Press OK to Retry.');\n";
						pResponse->strContent += "  window.history.back();\n";
						pResponse->strContent += "</script>\n\n";

						// ------------------------------------------------- END OF ADDITION

						pResponse->strContent += "</body>\n";
						pResponse->strContent += "</html>\n";

						return;
					}
					else
					{
						addUser( strLogin, strPass, m_iMemberAccess, strMail );

						pResponse->strContent += "<p>Thanks! You've successfully signed up! Click <a href=\"/login.html\">here</a> to login.</p>\n";

						// The Trinity Edition - Addition Begins

						// The following presents a JS popup to notify and redirect

						pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
						pResponse->strContent += "  alert('Thanks! Your account has been created. \\n\\n Press OK to Login.');\n";
						pResponse->strContent += "  document.location = \"/login.html\";\n";
						pResponse->strContent += "</script>\n\n";

						// ------------------------------------------------- END OF ADDITION

						pResponse->strContent += "</body>\n";
						pResponse->strContent += "</html>\n";

						return;
					}
				}
				else
				{
					pResponse->strContent += "<p>Unable to signup. The passwords did not match. Click <a href=\"/signup.html\">here</a> to return to the signup page.</p>\n";

					// The Trinity Edition - Addition Begins

					// The following presents a JS popup to notify and redirect

					pResponse->strContent += "<script language=\"javascript\" type=\"text/javascript\">\n\n";
					pResponse->strContent += "  alert('Unable to signup. The passwords did not match. \\n\\n Press OK to Retry.');\n";
					pResponse->strContent += "  window.history.back();\n";
					pResponse->strContent += "</script>\n\n";

					// ------------------------------------------------- END OF ADDITION

					pResponse->strContent += "</body>\n";
					pResponse->strContent += "</html>\n";

					return;
				}
			}
		}

		// The Trinity Edition - Modification Begins
		// Place the form into table cells for better visual flow
		// Assigns the Form a Name and Class; assigns the <ul> a class
		// Removes "Signup"

		pResponse->strContent += "<form method=\"get\" name=\"signup\" class=\"signup\"action=\"/signup.html\">\n";
		// pResponse->strContent += "<p><strong>Signup</strong></p>\n";
		pResponse->strContent += "<ul class=\"signup\">\n";
		pResponse->strContent += "<li>Names must be less than " + CAtomInt( m_iNameLength ).toString( ) + " characters long</li>\n";
		pResponse->strContent += "<li>Names are case sensitive</li>\n";
		pResponse->strContent += "<li>No HTML</li>\n";
		pResponse->strContent += "</ul>\n";

		pResponse->strContent += "<table>\n";
		pResponse->strContent += "<tr><td style=\"text-align:right\">\n";
		pResponse->strContent += "Login/Username</td>\n";
		pResponse->strContent += "<td><input name=\"us_login\" type=\"text\" size=\"40\" maxlength=" + CAtomInt( m_iNameLength ).toString( ) + ">\n";
		pResponse->strContent += "</td></tr>\n";
		pResponse->strContent += "<tr><td style=\"text-align:right\">\n";
		pResponse->strContent += "Password</td>\n";
		pResponse->strContent += "<td><input name=\"us_password\" type=password size=\"40\">\n";
		pResponse->strContent += "</td></tr>\n";
		pResponse->strContent += "<tr><td style=\"text-align:right\">\n";
		pResponse->strContent += "Password Again</td>\n";
		pResponse->strContent += "<td><input name=\"us_password_verify\" type=password size=\"40\">\n";
		pResponse->strContent += "</td></tr>\n";
		pResponse->strContent += "<tr><td style=\"text-align:right\">\n";
		pResponse->strContent += "Email Address</td>\n";
		pResponse->strContent += "<td><input name=\"us_email\" type=text size=\"40\">\n";
		pResponse->strContent += "</td></tr>\n";
		pResponse->strContent += "<tr><td colspan=\"2\" style=\"text-align:right\">\n";
		pResponse->strContent += "<input type=submit value=\"Signup\">\n";
		pResponse->strContent += "</td></tr>\n";
		pResponse->strContent += "</table>\n";

		/* Original Source Code:
		pResponse->strContent += "<form method=\"get\" action=\"/signup.html\">\n";
		pResponse->strContent += "<p><strong>Signup</strong></p>\n";
		pResponse->strContent += "<ul>\n";
		pResponse->strContent += "<li>Names must be less than " + CAtomInt( m_iNameLength ).toString( ) + " characters long</li>\n";
		pResponse->strContent += "<li>Names are case sensitive</li>\n";
		pResponse->strContent += "<li>No HTML</li>\n";
		pResponse->strContent += "</ul>\n";

		pResponse->strContent += "<input name=\"us_login\" type=text size=24 maxlength=" + CAtomInt( m_iNameLength ).toString( ) + "> Login<br><br>\n";
		pResponse->strContent += "<input name=\"us_password\" type=password size=20> Password<br>\n";
		pResponse->strContent += "<input name=\"us_password_verify\" type=password size=20> Verify Password<br><br>\n";
		pResponse->strContent += "<input name=\"us_email\" type=text size=40> E-Mail<br><br>\n";
		pResponse->strContent += "<input type=submit value=\"Signup\">\n";
		*/

		pResponse->strContent += "</form>\n";

		// The Trinity Edition - ADDITION BEGINS - Static Signup Footer Document (TRINCUSTSTATDOX)
		pResponse->strContent += m_strStaticFooterSignup;
		// -------------------------------- END OF ADD
	}
	else
		pResponse->strContent += "<p class=\"denied\">You are not authorized to view this page.</p>\n";

	pResponse->strContent += "</body>\n";
	pResponse->strContent += "</html>\n";

} // Added to End the Else statement for when the Alt_signup_page is NOT used
}
