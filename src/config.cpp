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

#include <fstream>

#include "bnbt.h"
#include "atom.h"
#include "config.h"
#include "tracker.h"
#include "util.h"

map<string, string> gmapCFG;

void CFG_Open( const char *szFile )
{
	gmapCFG.clear( );

	ifstream in;

	in.open( szFile );

	if( in.fail( ) )
	{
		UTIL_LogPrint( "config warning - unable to open %s for reading\n", szFile );

		return;
	}

	while( !in.eof( ) )
	{
		char pBuf[1024];

		in.getline( pBuf, 1023 );

		string strTemp = pBuf;

		// ignore blank lines and comments

		if( strTemp.empty( ) || strTemp[0] == '#' )
			continue;

		string :: size_type iSplit = strTemp.find( "=" );

		if( iSplit == string :: npos )
			continue;

		string :: size_type iKeyStart = strTemp.find_first_not_of( " " );
		string :: size_type iKeyEnd = strTemp.find( " ", iKeyStart );
		string :: size_type iValueStart = strTemp.find_first_not_of( " ", iSplit + 1 );
		string :: size_type iValueEnd = strTemp.size( );

		if( iValueStart != string :: npos )
			gmapCFG[strTemp.substr( iKeyStart, iKeyEnd - iKeyStart )] = strTemp.substr( iValueStart, iValueEnd - iValueStart );
	}

	in.close( );
}

void CFG_SetInt( string strKey, int x )
{
	gmapCFG[strKey] = CAtomInt( x ).toString( );
}

void CFG_SetString( string strKey, string x )
{
	gmapCFG[strKey] = x;
}

int CFG_GetInt( string strKey, int x )
{
	if( gmapCFG.find( strKey ) == gmapCFG.end( ) )
		return x;
	else
		return atoi( gmapCFG[strKey].c_str( ) );
}

string CFG_GetString( string strKey, string x )
{
	if( gmapCFG.find( strKey ) == gmapCFG.end( ) )
		return x;
	else
		return gmapCFG[strKey];
}

void CFG_Delete( string strKey )
{
	gmapCFG.erase( strKey );
}

void CFG_Close( const char *szFile )
{
	ofstream out;

	out.open( szFile );

	if( out.fail( ) )
	{
		UTIL_LogPrint( "config warning - unable to open %s for writing\n", szFile );

		return;
	}

	for( map<string, string> :: iterator i = gmapCFG.begin( ); i != gmapCFG.end( ); i++ )
		out << (*i).first.c_str( ) << " = " << (*i).second.c_str( ) << endl;

	out.close( );
}

void CFG_SetDefaults( )
{
	if( gbDebug )
		UTIL_LogPrint( "config - setting defaults\n" );

	// bnbt.cpp

	if( CFG_GetInt( "bnbt_debug", -1 ) < 0 )
		CFG_SetInt( "bnbt_debug", 0 );

	if( CFG_GetInt( "bnbt_max_conns", 0 ) < 1 )
		CFG_SetInt( "bnbt_max_conns", 64 );

	if( CFG_GetString( "bnbt_style_sheet", string( ) ).empty( ) )
		CFG_SetString( "bnbt_style_sheet", string( ) );

	if( CFG_GetString( "bnbt_charset", string( ) ).empty( ) )
		CFG_SetString( "bnbt_charset", "iso-8859-1" );

	if( CFG_GetString( "bnbt_realm", string( ) ).empty( ) )
		CFG_SetString( "bnbt_realm", "BNBT" );

	if( CFG_GetString( "bnbt_error_log_dir", string( ) ).empty( ) )
		CFG_SetString( "bnbt_error_log_dir", string( ) );

	if( CFG_GetString( "bnbt_access_log_dir", string( ) ).empty( ) )
		CFG_SetString( "bnbt_access_log_dir", string( ) );

	if( CFG_GetInt( "bnbt_flush_interval", -1 ) < 0 )
		CFG_SetInt( "bnbt_flush_interval", 100 );

	// link.cpp

	if( CFG_GetInt( "bnbt_tlink_server", -1 ) < 0 )
		CFG_SetInt( "bnbt_tlink_server", 0 );

	if( CFG_GetString( "bnbt_tlink_connect", string( ) ).empty( ) )
		CFG_SetString( "bnbt_tlink_connect", string( ) );

	if( CFG_GetString( "bnbt_tlink_password", string( ) ).empty( ) )
		CFG_SetString( "bnbt_tlink_password", string( ) );

	if( CFG_GetString( "bnbt_tlink_bind", string( ) ).empty( ) )
		CFG_SetString( "bnbt_tlink_bind", string( ) );

	if( CFG_GetInt( "bnbt_tlink_port", 0 ) < 1 )
		CFG_SetInt( "bnbt_tlink_port", 5204 );

	// server.cpp

	if( CFG_GetInt( "socket_timeout", -1 ) < 0 )
		CFG_SetInt( "socket_timeout", 15 );

	if( CFG_GetInt( "bnbt_compression_level", -1 ) < 0 )
		CFG_SetInt( "bnbt_compression_level", 6 );

	if( CFG_GetString( "bind", string( ) ).empty( ) )
		CFG_SetString( "bind", string( ) );

	if( CFG_GetInt( "port", 0 ) < 1 )
		CFG_SetInt( "port", 6969 );

	// tracker.cpp

	if( CFG_GetString( "allowed_dir", string( ) ).empty( ) )
		CFG_SetString( "allowed_dir", string( ) );

	if( CFG_GetString( "bnbt_upload_dir", string( ) ).empty( ) )
		CFG_SetString( "bnbt_upload_dir", string( ) );

	if( CFG_GetString( "bnbt_external_torrent_dir", string( ) ).empty( ) )
		CFG_SetString( "bnbt_external_torrent_dir", string( ) );

	if( CFG_GetString( "bnbt_archive_dir", string( ) ).empty( ) )
		CFG_SetString( "bnbt_archive_dir", string( ) );

	if( CFG_GetString( "bnbt_file_dir", string( ) ).empty( ) )
		CFG_SetString( "bnbt_file_dir", string( ) );

	if( CFG_GetString( "dfile", string( ) ).empty( ) )
		CFG_SetString( "dfile", "dstate.bnbt" );

	if( CFG_GetString( "bnbt_comments_file", string( ) ).empty( ) )
		CFG_SetString( "bnbt_comments_file", string( ) );

	if( CFG_GetString( "bnbt_tag_file", string( ) ).empty( ) )
		CFG_SetString( "bnbt_tag_file", "tags.bnbt" );

	if( CFG_GetString( "bnbt_users_file", string( ) ).empty( ) )
		CFG_SetString( "bnbt_users_file", "users.bnbt" );

	if( CFG_GetString( "bnbt_static_header", string( ) ).empty( ) )
		CFG_SetString( "bnbt_static_header", string( ) );

	if( CFG_GetString( "bnbt_static_footer", string( ) ).empty( ) )
		CFG_SetString( "bnbt_static_footer", string( ) );

	// The Trinity Edition - ADD BEGINS (TRINCUSTSTATDOX)
	// Creates the Static SIGNUP Header and Footer Documents configuration keys (TRINCUSTSTATDOX)

	if( CFG_GetString( "bnbt_static_signup_header", string( ) ).empty( ) )
		CFG_SetString( "bnbt_static_signup_header", string( ) );

	if( CFG_GetString( "bnbt_static_signup_footer", string( ) ).empty( ) )
		CFG_SetString( "bnbt_static_signup_footer", string( ) );

	// Creates the Static UPLOAD Header and Footer Documents configuration keys (TRINCUSTSTATDOX)

	if( CFG_GetString( "bnbt_static_upload_header", string( ) ).empty( ) )
		CFG_SetString( "bnbt_static_upload_header", string( ) );

	if( CFG_GetString( "bnbt_static_upload_footer", string( ) ).empty( ) )
		CFG_SetString( "bnbt_static_upload_footer", string( ) );
		
	// --------------------------------------- END OF ADD

	if( CFG_GetString( "bnbt_dump_xml_file", string( ) ).empty( ) )
		CFG_SetString( "bnbt_dump_xml_file", string( ) );

	if( CFG_GetString( "image_bar_fill", string( ) ).empty( ) )
		CFG_SetString( "image_bar_fill", string( ) );

	if( CFG_GetString( "image_bar_trans", string( ) ).empty( ) )
		CFG_SetString( "image_bar_trans", string( ) );

	if( CFG_GetString( "bnbt_force_announce_url", string( ) ).empty( ) )
		CFG_SetString( "bnbt_force_announce_url", string( ) );

	if( CFG_GetInt( "bnbt_force_announce_on_download", -1 ) < 0 )
		CFG_SetInt( "bnbt_force_announce_on_download", 0 );

	if( CFG_GetInt( "parse_allowed_interval", -1 ) < 0 )
		CFG_SetInt( "parse_allowed_interval", 0 );

	if( CFG_GetInt( "save_dfile_interval", -1 ) < 0 )
		CFG_SetInt( "save_dfile_interval", 300 );

	if( CFG_GetInt( "downloader_timeout_interval", -1 ) < 0 )
		CFG_SetInt( "downloader_timeout_interval", 2700 );

	if( CFG_GetInt( "bnbt_refresh_static_interval", -1 ) < 0 )
		CFG_SetInt( "bnbt_refresh_static_interval", 10 );

	if( CFG_GetInt( "bnbt_dump_xml_interval", -1 ) < 0 )
		CFG_SetInt( "bnbt_dump_xml_interval", 600 );

	if( CFG_GetInt( "announce_interval", -1 ) < 0 )
		CFG_SetInt( "announce_interval", 1800 );

	if( CFG_GetInt( "min_request_interval", -1 ) < 0 )
		CFG_SetInt( "min_request_interval", 18000 );

	if( CFG_GetInt( "response_size", -1 ) < 0 )
		CFG_SetInt( "response_size", 50 );

	if( CFG_GetInt( "max_give", -1 ) < 0 )
		CFG_SetInt( "max_give", 200 );

	if( CFG_GetInt( "keep_dead", -1 ) < 0 )
		// The Trinity Edition - Modification Begins
		// Changes the default keep_dead value to 1
		CFG_SetInt( "keep_dead", 1 );
		/* Original Source Code:
		CFG_SetInt( "keep_dead", 0 );
		*/

	if( CFG_GetInt( "bnbt_allow_scrape", -1 ) < 0 )
		CFG_SetInt( "bnbt_allow_scrape", 1 );

	if( CFG_GetInt( "bnbt_count_unique_peers", -1 ) < 0 )
		CFG_SetInt( "bnbt_count_unique_peers", 1 );

	if( CFG_GetInt( "bnbt_delete_invalid", -1 ) < 0 )
		CFG_SetInt( "bnbt_delete_invalid", 0 );

	if( CFG_GetInt( "bnbt_parse_on_upload", -1 ) < 0 )
		CFG_SetInt( "bnbt_parse_on_upload", 1 );

	if( CFG_GetInt( "bnbt_max_torrents", -1 ) < 0 )
		CFG_SetInt( "bnbt_max_torrents", 0 );

	if( CFG_GetInt( "bnbt_show_info_hash", -1 ) < 0 )
		CFG_SetInt( "bnbt_show_info_hash", 0 );

	if( CFG_GetInt( "show_names", -1 ) < 0 )
		CFG_SetInt( "show_names", 1 );

	if( CFG_GetInt( "bnbt_show_stats", -1 ) < 0 )
		CFG_SetInt( "bnbt_show_stats", 1 );

	if( CFG_GetInt( "bnbt_allow_torrent_downloads", -1 ) < 0 )
		CFG_SetInt( "bnbt_allow_torrent_downloads", 1 );

	if( CFG_GetInt( "bnbt_allow_comments", -1 ) < 0 )
		CFG_SetInt( "bnbt_allow_comments", 0 );

	if( CFG_GetInt( "bnbt_show_added", -1 ) < 0 )
		CFG_SetInt( "bnbt_show_added", 1 );

	if( CFG_GetInt( "bnbt_show_size", -1 ) < 0 )
		CFG_SetInt( "bnbt_show_size", 1 );

	if( CFG_GetInt( "bnbt_show_num_files", -1 ) < 0 )
		CFG_SetInt( "bnbt_show_num_files", 1 );

	if( CFG_GetInt( "bnbt_show_completed", -1 ) < 0 )
		CFG_SetInt( "bnbt_show_completed", 0 );

	if( CFG_GetInt( "bnbt_show_transferred", -1 ) < 0 )
		CFG_SetInt( "bnbt_show_transferred", 0 );

	if( CFG_GetInt( "bnbt_show_min_left", -1 ) < 0 )
		CFG_SetInt( "bnbt_show_min_left", 0 );

	if( CFG_GetInt( "bnbt_show_average_left", -1 ) < 0 )
		CFG_SetInt( "bnbt_show_average_left", 0 );

	if( CFG_GetInt( "bnbt_show_max_left", -1 ) < 0 )
		CFG_SetInt( "bnbt_show_max_left", 0 );

	if( CFG_GetInt( "bnbt_show_left_as_progress", -1 ) < 0 )
		CFG_SetInt( "bnbt_show_left_as_progress", 1 );

	if( CFG_GetInt( "bnbt_show_uploader", -1 ) < 0 )
		CFG_SetInt( "bnbt_show_uploader", 0 );

	if( CFG_GetInt( "bnbt_allow_info_link", -1 ) < 0 )
		CFG_SetInt( "bnbt_allow_info_link", 0 );

	if( CFG_GetInt( "bnbt_allow_search", -1 ) < 0 )
		CFG_SetInt( "bnbt_allow_search", 1 );

	if( CFG_GetInt( "bnbt_allow_sort", -1 ) < 0 )
		CFG_SetInt( "bnbt_allow_sort", 1 );

	if( CFG_GetInt( "bnbt_show_file_comment", -1 ) < 0 )
		CFG_SetInt( "bnbt_show_file_comment", 1 );

	if( CFG_GetInt( "bnbt_show_file_contents", -1 ) < 0 )
		CFG_SetInt( "bnbt_show_file_contents", 0 );

	if( CFG_GetInt( "bnbt_show_share_ratios", -1 ) < 0 )
		CFG_SetInt( "bnbt_show_share_ratios", 1 );

	if( CFG_GetInt( "bnbt_show_average_dl_rate", -1 ) < 0 )
		CFG_SetInt( "bnbt_show_average_dl_rate", 0 );

	if( CFG_GetInt( "bnbt_show_average_ul_rate", -1 ) < 0 )
		CFG_SetInt( "bnbt_show_average_ul_rate", 0 );

	if( CFG_GetInt( "bnbt_delete_own_torrents", -1 ) < 0 )
		CFG_SetInt( "bnbt_delete_own_torrents", 1 );

	if( CFG_GetInt( "bnbt_show_gen_time", -1 ) < 0 )
		CFG_SetInt( "bnbt_show_gen_time", 1 );

	if( CFG_GetInt( "bnbt_per_page", -1 ) < 0 )
		CFG_SetInt( "bnbt_per_page", 20 );

	if( CFG_GetInt( "bnbt_users_per_page", -1 ) < 0 )
		CFG_SetInt( "bnbt_users_per_page", 50 );

	if( CFG_GetInt( "bnbt_max_peers_display", -1 ) < 0 )
		CFG_SetInt( "bnbt_max_peers_display", 500 );

	if( CFG_GetInt( "bnbt_guest_access", -1 ) < 0 )
		CFG_SetInt( "bnbt_guest_access", ACCESS_VIEW | ACCESS_DL | ACCESS_SIGNUP );

	if( CFG_GetInt( "bnbt_member_access", -1 ) < 0 )
		CFG_SetInt( "bnbt_member_access", ACCESS_VIEW | ACCESS_DL | ACCESS_COMMENTS | ACCESS_UPLOAD | ACCESS_SIGNUP );

	if( CFG_GetInt( "bnbt_file_expires", -1 ) < 0 )
		CFG_SetInt( "bnbt_file_expires", 180 );

	if( CFG_GetInt( "bnbt_name_length", -1 ) < 0 )
		CFG_SetInt( "bnbt_name_length", 32 );

	if( CFG_GetInt( "bnbt_comment_length", -1 ) < 0 )
		CFG_SetInt( "bnbt_comment_length", 800 );

	// The Trinity Edition - Addition Begins
	// Sets the default value for "trinity_nt_service_name" to "BNBT Service"

	if( CFG_GetString( "trinity_nt_service_name", string( ) ).empty( ) )
		CFG_SetString( "trinity_nt_service_name", "BNBT Service" );

	// Sets the default value for "trinity_use_customlabels" to 1 (TRUE)

	if( CFG_GetInt( "bnbt_custom_labels", -1 ) < 0 )
		CFG_SetInt( "bnbt_custom_labels", 1 );

	// Sets the default value for "bnbt_navigation_bar" to 1 (TRUE)

	if( CFG_GetInt( "bnbt_navigation_bar", -1 ) < 0 )
		CFG_SetInt( "bnbt_navigation_bar", 1 );

	// Sets the default value for "trinity_use_mouseovers" to 0 (FALSE)

	if( CFG_GetInt( "trinity_use_mouseovers", -1 ) < 0 )
		CFG_SetInt( "trinity_use_mouseovers", 0 );

	// Creates the Alternate Signup URL configuration key

	if( CFG_GetString( "bnbt_alt_signup_url", string( ) ).empty( ) )
		CFG_SetString( "bnbt_alt_signup_url", string( ) );

	// Creates the Tracker Title configuration key

	if( CFG_GetString( "bnbt_tracker_title", string( ) ).empty( ) )
		CFG_SetString( "bnbt_tracker_title", string( ) );
		
        // Creates the External Javascript file location configuration key
        
	if( CFG_GetString( "bnbt_external_js", string( ) ).empty( ) )
		CFG_SetString( "bnbt_external_js", string( ) );

	// ------------------------------------------------- End of Addition
}
