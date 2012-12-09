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

#ifndef WIN32
 #include <sys/stat.h>
 #include <dirent.h>
#endif

#include "bnbt.h"
#include "atom.h"
#include "bencode.h"
#include "config.h"
#include "md5.h"
#include "server.h"
#include "sort.h"
#include "tracker.h"
#include "util.h"

map<string, string> gmapMime;

CTracker :: CTracker( )
{
	m_mtxQueued.Initialize( );

	m_strAllowedDir = CFG_GetString( "allowed_dir", string( ) );
	m_strUploadDir = CFG_GetString( "bnbt_upload_dir", string( ) );
	m_strExternalTorrentDir = CFG_GetString( "bnbt_external_torrent_dir", string( ) );
	m_strArchiveDir = CFG_GetString( "bnbt_archive_dir", string( ) );
	m_strFileDir = CFG_GetString( "bnbt_file_dir", string( ) );

	if( !m_strAllowedDir.empty( ) && m_strAllowedDir[m_strAllowedDir.size( ) - 1] != PATH_SEP )
		m_strAllowedDir += PATH_SEP;

	if( !m_strUploadDir.empty( ) && m_strUploadDir[m_strUploadDir.size( ) - 1] != PATH_SEP )
		m_strUploadDir += PATH_SEP;

	if( !m_strExternalTorrentDir.empty( ) && m_strExternalTorrentDir[m_strExternalTorrentDir.size( ) - 1] != '/' )
		m_strExternalTorrentDir += '/';

	if( !m_strArchiveDir.empty( ) && m_strArchiveDir[m_strArchiveDir.size( ) - 1] != PATH_SEP )
		m_strArchiveDir += PATH_SEP;

	if( !m_strFileDir.empty( ) && m_strFileDir[m_strFileDir.size( ) - 1] != PATH_SEP )
		m_strFileDir += PATH_SEP;

	m_strDFile = CFG_GetString( "dfile", "dstate.bnbt" );
	m_strCommentsFile = CFG_GetString( "bnbt_comments_file", string( ) );
	m_strTagFile = CFG_GetString( "bnbt_tag_file", "tags.bnbt" );
	m_strUsersFile = CFG_GetString( "bnbt_users_file", "users.bnbt" );
	m_strStaticHeaderFile = CFG_GetString( "bnbt_static_header", string( ) );
	m_strStaticFooterFile = CFG_GetString( "bnbt_static_footer", string( ) );
	// The Trinity Edition - ADD BEGINS (TRINCUSTSTATDOX)
	m_strStaticHeaderSignupFile = CFG_GetString( "bnbt_static_signup_header", string( ) );
	m_strStaticFooterSignupFile = CFG_GetString( "bnbt_static_signup_footer", string( ) );

	m_strStaticHeaderUploadFile = CFG_GetString( "bnbt_static_upload_header", string( ) );
	m_strStaticFooterUploadFile = CFG_GetString( "bnbt_static_upload_footer", string( ) );
	// ---------------------------------------- END OF ADD
	m_strDumpXMLFile = CFG_GetString( "bnbt_dump_xml_file", string( ) );
	m_strImageBarFill = CFG_GetString( "image_bar_fill", string( ) );
	m_strImageBarTrans = CFG_GetString( "image_bar_trans", string( ) );
	m_strForceAnnounceURL = CFG_GetString( "bnbt_force_announce_url", string( ) );
	m_bForceAnnounceOnDL = CFG_GetInt( "bnbt_force_announce_on_download", 0 ) == 0 ? false : true;
	m_iParseAllowedInterval = CFG_GetInt( "parse_allowed_interval", 0 );
	m_iSaveDFileInterval = CFG_GetInt( "save_dfile_interval", 300 );
	m_iDownloaderTimeOutInterval = CFG_GetInt( "downloader_timeout_interval", 2700 );
	m_iRefreshStaticInterval = CFG_GetInt( "bnbt_refresh_static_interval", 10 );
	m_iDumpXMLInterval = CFG_GetInt( "bnbt_dump_xml_interval", 600 );
	m_iParseAllowedNext = GetTime( ) + m_iParseAllowedInterval * 60;
	m_iSaveDFileNext = GetTime( ) + m_iSaveDFileInterval;
	m_iPrevTime = 1;
	m_iDownloaderTimeOutNext = GetTime( ) + m_iDownloaderTimeOutInterval;
	m_iRefreshStaticNext = 0;
	m_iDumpXMLNext = GetTime( ) + m_iDumpXMLInterval;
	m_iAnnounceInterval = CFG_GetInt( "announce_interval", 1800 );
	m_iMinRequestInterval = CFG_GetInt( "min_request_interval", 18000 );
	m_iResponseSize = CFG_GetInt( "response_size", 50 );
	m_iMaxGive = CFG_GetInt( "max_give", 200 );
	m_bKeepDead = CFG_GetInt( "keep_dead", 0 ) == 0 ? false : true;
	m_bAllowScrape = CFG_GetInt( "bnbt_allow_scrape", 1 ) == 0 ? false : true;
	m_bCountUniquePeers = CFG_GetInt( "bnbt_count_unique_peers", 1 ) == 0 ? false : true;
	m_bDeleteInvalid = CFG_GetInt( "bnbt_delete_invalid", 0 ) == 0 ? false : true;
	m_bParseOnUpload = CFG_GetInt( "bnbt_parse_on_upload", 1 ) == 0 ? false : true;
	m_iMaxTorrents = CFG_GetInt( "bnbt_max_torrents", 0 );
	m_bShowInfoHash = CFG_GetInt( "bnbt_show_info_hash", 0 ) == 0 ? false : true;
	m_bShowNames = CFG_GetInt( "show_names", 1 ) == 0 ? false : true;
	m_bShowStats = CFG_GetInt( "bnbt_show_stats", 1 ) == 0 ? false : true;
	m_bAllowTorrentDownloads = CFG_GetInt( "bnbt_allow_torrent_downloads", 1 ) == 0 ? false : true;
	m_bAllowComments = CFG_GetInt( "bnbt_allow_comments", 0 ) == 0 ? false : true;
	m_bShowAdded = CFG_GetInt( "bnbt_show_added", 1 ) == 0 ? false : true;
	m_bShowSize = CFG_GetInt( "bnbt_show_size", 1 ) == 0 ? false : true;
	m_bShowNumFiles = CFG_GetInt( "bnbt_show_num_files", 1 ) == 0 ? false : true;
	m_bShowCompleted = CFG_GetInt( "bnbt_show_completed", 0 ) == 0 ? false : true;
	m_bShowTransferred = CFG_GetInt( "bnbt_show_transferred", 0 ) == 0 ? false : true;
	m_bShowMinLeft = CFG_GetInt( "bnbt_show_min_left", 0 ) == 0 ? false : true;
	m_bShowAverageLeft = CFG_GetInt( "bnbt_show_average_left", 0 ) == 0 ? false : true;
	m_bShowMaxiLeft = CFG_GetInt( "bnbt_show_max_left", 0 ) == 0 ? false : true;
	m_bShowLeftAsProgress = CFG_GetInt( "bnbt_show_left_as_progress", 1 ) == 0 ? false : true;
	m_bShowUploader = CFG_GetInt( "bnbt_show_uploader", 0 ) == 0 ? false : true;
	m_bAllowInfoLink = CFG_GetInt( "bnbt_allow_info_link", 0 ) == 0 ? false : true;
	m_bSearch = CFG_GetInt( "bnbt_allow_search", 1 ) == 0 ? false : true;
	m_bSort = CFG_GetInt( "bnbt_allow_sort", 1 ) == 0 ? false : true;
	m_bShowFileComment = CFG_GetInt( "bnbt_show_file_comment", 1 ) == 0 ? false : true;
	m_bShowFileContents = CFG_GetInt( "bnbt_show_file_contents", 0 ) == 0 ? false : true;
	m_bShowShareRatios = CFG_GetInt( "bnbt_show_share_ratios", 1 ) == 0 ? false : true;
	m_bShowAvgDLRate = CFG_GetInt( "bnbt_show_average_dl_rate", 0 ) == 0 ? false : true;
	m_bShowAvgULRate = CFG_GetInt( "bnbt_show_average_ul_rate", 0 ) == 0 ? false : true;
	m_bDeleteOwnTorrents = CFG_GetInt( "bnbt_delete_own_torrents", 1 ) == 0 ? false : true;
	m_bGen = CFG_GetInt( "bnbt_show_gen_time", 1 ) == 0 ? false : true;
	m_iPerPage = CFG_GetInt( "bnbt_per_page", 20 );
	m_iUsersPerPage = CFG_GetInt( "bnbt_users_per_page", 50 );
	m_iMaxPeersDisplay = CFG_GetInt( "bnbt_max_peers_display", 500 );
	m_iGuestAccess = CFG_GetInt( "bnbt_guest_access", ACCESS_VIEW | ACCESS_DL | ACCESS_SIGNUP );
	m_iMemberAccess = CFG_GetInt( "bnbt_member_access", ACCESS_VIEW | ACCESS_DL | ACCESS_COMMENTS | ACCESS_UPLOAD | ACCESS_SIGNUP );
	m_iFileExpires = CFG_GetInt( "bnbt_file_expires", 180 );
	m_iNameLength = CFG_GetInt( "bnbt_name_length", 32 );
	m_iCommentLength = CFG_GetInt( "bnbt_comment_length", 800 );

	// The Trinity Edition - Addition Begins

	// Retrieves the "trinity_use_mouseovers" value
	m_bUseMouseovers = CFG_GetInt( "trinity_use_mouseovers", 0 ) == 0 ? false : true;

	// Retrieves the "bnbt_custom_labels" value
	m_bUseCustomlabels = CFG_GetInt( "bnbt_custom_labels", 1 ) == 0 ? false : true;

	// Retrieves the "bnbt_navigation_bar" value
	m_bUseNavbar = CFG_GetInt( "bnbt_navigation_bar", 1 ) == 0 ? false : true;

	// ------------------------------------------------- End of Addition

	// tags

	int iTag = 1;

	string strName = "bnbt_tag" + CAtomInt( iTag ).toString( );
	string strTag;

	while( !( strTag = CFG_GetString( strName, string( ) ) ).empty( ) )
	{
		string :: size_type iSplit = strTag.find( "|" );

		strName = "bnbt_tag" + CAtomInt( ++iTag ).toString( );

		if( iSplit == string :: npos )
			m_vecTags.push_back( pair<string, string>( strTag, string( ) ) );
		else
			m_vecTags.push_back( pair<string, string>( strTag.substr( 0, iSplit ), strTag.substr( iSplit + 1 ) ) );
	}

	m_pAllowed = NULL;
	m_pTimeDicti = new CAtomDicti( );
	m_pCached = new CAtomDicti( );
	m_pIPs = new CAtomDicti( );

	// decode dfile

	CAtom *pState = DecodeFile( m_strDFile.c_str( ) );

	if( pState && pState->isDicti( ) )
	{
		m_pState = (CAtomDicti *)pState;

		if( m_pState->getItem( "peers" ) && m_pState->getItem( "completed" ) )
		{
			CAtom *pDFile = m_pState->getItem( "peers" );

			if( pDFile && pDFile->isDicti( ) )
				m_pDFile = (CAtomDicti *)pDFile;

			CAtom *pCompleted = m_pState->getItem( "completed" );

			if( pCompleted && pCompleted->isDicti( ) )
				m_pCompleted = (CAtomDicti *)pCompleted;
		}
		else
		{
			if( pState )
				delete pState;

			m_pState = new CAtomDicti( );
			m_pDFile = new CAtomDicti( );
			m_pCompleted = new CAtomDicti( );

			m_pState->setItem( "peers", m_pDFile );
			m_pState->setItem( "completed", m_pCompleted );
		}

		// populate time dicti

		map<string, CAtom *> mapDicti = m_pDFile->getValue( );

		for( map<string, CAtom *> :: iterator i = mapDicti.begin( ); i != mapDicti.end( ); i++ )
		{
			CAtomDicti *pTS = new CAtomDicti( );

			if( (*i).second->isDicti( ) )
			{
				CAtomDicti *pPeersDicti = (CAtomDicti *)(*i).second;

				map<string, CAtom *> mapPeersDicti = pPeersDicti->getValue( );

				for( map<string, CAtom *> :: iterator j = mapPeersDicti.begin( ); j != mapPeersDicti.end( ); j++ )
					pTS->setItem( (*j).first, new CAtomLong( 0 ) );

				// reset connected times

				for( map<string, CAtom *> :: iterator j = mapPeersDicti.begin( ); j != mapPeersDicti.end( ); j++ )
				{
					if( (*j).second->isDicti( ) )
					{
						CAtomDicti *pPeerDicti = (CAtomDicti *)(*j).second;

						if( pPeerDicti )
							pPeerDicti->setItem( "connected", new CAtomLong( 0 ) );
					}
				}
			}

			m_pTimeDicti->setItem( (*i).first, pTS );
		}
	}
	else
	{
		if( pState )
			delete pState;

		m_pState = new CAtomDicti( );
		m_pDFile = new CAtomDicti( );
		m_pCompleted = new CAtomDicti( );

		m_pState->setItem( "peers", m_pDFile );
		m_pState->setItem( "completed", m_pCompleted );
	}

	// decode comments file

	if( m_strCommentsFile.empty( ) )
		m_pComments = new CAtomDicti( );
	else
	{
		CAtom *pComments = DecodeFile( m_strCommentsFile.c_str( ) );

		if( pComments && pComments->isDicti( ) )
			m_pComments = (CAtomDicti *)pComments;
		else
		{
			if( pComments )
				delete pComments;

			pComments = NULL;

			m_pComments = new CAtomDicti( );
		}
	}

	// decode tag file

	if( m_strTagFile.empty( ) )
		m_pTags = new CAtomDicti( );
	else
	{
		CAtom *pTags = DecodeFile( m_strTagFile.c_str( ) );

		if( pTags && pTags->isDicti( ) )
			m_pTags = (CAtomDicti *)pTags;
		else
		{
			if( pTags )
				delete pTags;

			pTags = NULL;

			m_pTags = new CAtomDicti( );
		}
	}

	// decode users file

	if( m_strUsersFile.empty( ) )
		m_pUsers = new CAtomDicti( );
	else
	{
		CAtom *pUsers = DecodeFile( m_strUsersFile.c_str( ) );

		if( pUsers && pUsers->isDicti( ) )
			m_pUsers = (CAtomDicti *)pUsers;
		else
		{
			if( pUsers )
				delete pUsers;

			pUsers = NULL;

			m_pUsers = new CAtomDicti( );
		}
	}

	if( m_bCountUniquePeers )
		CountUniquePeers( );

	// parse allowed dir

	if( !m_strAllowedDir.empty( ) )
		parseTorrents( m_strAllowedDir.c_str( ) );

	// mime

	gmapMime[".hqx"]	= "application/mac-binhex40";
	gmapMime[".exe"]	= "application/octet-stream";
	gmapMime[".pdf"]	= "application/pdf";
	gmapMime[".gtar"]	= "application/x-gtar";
	gmapMime[".gz"]		= "application/x-gzip";
	gmapMime[".js"]		= "application/x-javascript";
	gmapMime[".swf"]	= "application/x-shockwave-flash";
	gmapMime[".sit"]	= "application/x-stuffit";
	gmapMime[".tar"]	= "application/x-tar";
	gmapMime[".zip"]	= "application/zip";
	gmapMime[".bmp"]	= "image/bmp";
	gmapMime[".gif"]	= "image/gif";
	gmapMime[".jpg"]	= "image/jpeg";
	gmapMime[".jpeg"]	= "image/jpeg";
	gmapMime[".jpe"]	= "image/jpeg";
	gmapMime[".png"]	= "image/png";
	gmapMime[".tiff"]	= "image/tiff";
	gmapMime[".tif"]	= "image/tiff";
	gmapMime[".css"]	= "text/css";
	gmapMime[".html"]	= "text/html";
	gmapMime[".htm"]	= "text/html";
	gmapMime[".txt"]	= "text/plain";
	gmapMime[".rtf"]	= "text/rtf";
	gmapMime[".xml"]	= "text/xml";
}

CTracker :: ~CTracker( )
{
	delete m_pAllowed;
	delete m_pState;
	delete m_pTimeDicti;
	delete m_pCached;
	delete m_pComments;
	delete m_pTags;
	delete m_pUsers;
	delete m_pIPs;

	m_pAllowed = NULL;
	m_pState = NULL;
	m_pDFile = NULL;
	m_pCompleted = NULL;
	m_pTimeDicti = NULL;
	m_pCached = NULL;
	m_pComments = NULL;
	m_pTags = NULL;
	m_pUsers = NULL;
	m_pIPs = NULL;

	m_mtxQueued.Destroy( );
}

void CTracker :: saveDFile( )
{
	string strData = Encode( m_pState );

	FILE *pFile = NULL;

	if( ( pFile = fopen( m_strDFile.c_str( ), "wb" ) ) == NULL )
	{
		UTIL_LogPrint( "tracker warning - unable to open %s for writing\n", m_strDFile.c_str( ) );

		return;
	}

	fwrite( (void *)strData.c_str( ), sizeof( char ), strData.size( ), pFile );
	fclose( pFile );
}

void CTracker :: saveComments( )
{
	if( m_strCommentsFile.empty( ) )
		return;

	string strData = Encode( m_pComments );

	FILE *pFile = NULL;

	if( ( pFile = fopen( m_strCommentsFile.c_str( ), "wb" ) ) == NULL )
	{
		UTIL_LogPrint( "tracker warning - unable to open %s for writing\n", m_strCommentsFile.c_str( ) );

		return;
	}

	fwrite( (void *)strData.c_str( ), sizeof( char ), strData.size( ), pFile );
	fclose( pFile );
}

void CTracker :: saveTags( )
{
	string strData = Encode( m_pTags );

	FILE *pFile = NULL;

	if( ( pFile = fopen( m_strTagFile.c_str( ), "wb" ) ) == NULL )
	{
		UTIL_LogPrint( "tracker warning - unable to open %s for writing\n", m_strTagFile.c_str( ) );

		return;
	}

	fwrite( (void *)strData.c_str( ), sizeof( char ), strData.size( ), pFile );
	fclose( pFile );
}

void CTracker :: saveUsers( )
{
	string strData = Encode( m_pUsers );

	FILE *pFile = NULL;

	if( ( pFile = fopen( m_strUsersFile.c_str( ), "wb" ) ) == NULL )
	{
		UTIL_LogPrint( "tracker warning - unable to open %s for writing\n", m_strUsersFile.c_str( ) );

		return;
	}

	fwrite( (void *)strData.c_str( ), sizeof( char ), strData.size( ), pFile );
	fclose( pFile );
}

void CTracker :: saveXML( )
{
	string strData;

	strData += "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
	strData += "<torrents>\n";

	if( m_pDFile )
	{
		map<string, CAtom *> mapDicti = m_pDFile->getValue( );

		for( map<string, CAtom *> :: iterator i = mapDicti.begin( ); i != mapDicti.end( ); i++ )
		{
			strData += "<torrent hash=\"";
			strData += UTIL_HashToString( (*i).first );
			strData += "\"";

			if( m_pAllowed )
			{
				CAtom *pList = m_pAllowed->getItem( (*i).first );

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
							strData += " filename=\"" + UTIL_RemoveHTML( pFileName->toString( ) ) + "\"";

						if( pName )
							strData += " name=\"" + UTIL_RemoveHTML( pName->toString( ) ) + "\"";

						if( pAdded )
							strData += " added=\"" + pAdded->toString( ) + "\"";

						if( pSize )
							strData += " size=\"" + pSize->toString( ) + "\"";

						if( pFiles )
							strData += " files=\"" + pFiles->toString( ) + "\"";
					}
				}
			}

			if( m_pCompleted )
			{
				CAtom *pCompleted = m_pCompleted->getItem( (*i).first );

				if( pCompleted )
					strData += " completed=\"" + pCompleted->toString( ) + "\"";
			}

			if( m_pTags )
			{
				CAtom *pDicti = m_pTags->getItem( (*i).first );

				if( pDicti && pDicti->isDicti( ) )
				{
					CAtom *pTag = ( (CAtomDicti *)pDicti )->getItem( "tag" );
					CAtom *pName = ( (CAtomDicti *)pDicti )->getItem( "name" );
					CAtom *pUploader = ( (CAtomDicti *)pDicti )->getItem( "uploader" );

					if( pTag )
						strData += " tag=\"" + UTIL_RemoveHTML( pTag->toString( ) ) + "\"";

					if( pName )
						strData += " uploadname=\"" + UTIL_RemoveHTML( pName->toString( ) ) + "\"";

					if( pUploader )
						strData += " uploader=\"" + UTIL_RemoveHTML( pUploader->toString( ) ) + "\"";
				}
			}

			strData += ">\n";

			if( (*i).second->isDicti( ) )
			{
				map<string, CAtom *> mapPeersDicti = ( (CAtomDicti *)(*i).second )->getValue( );

				if( mapPeersDicti.empty( ) )
					strData += "<peers />\n";
				else
				{
					strData += "<peers>\n";

					for( map<string, CAtom *> :: iterator j = mapPeersDicti.begin( ); j != mapPeersDicti.end( ); j++ )
					{
						if( (*j).second->isDicti( ) )
						{
							CAtomDicti *pPeerDicti = (CAtomDicti *)(*j).second;

							CAtom *pIP = pPeerDicti->getItem( "ip" );
							CAtom *pUpped = pPeerDicti->getItem( "uploaded" );
							CAtom *pDowned = pPeerDicti->getItem( "downloaded" );
							CAtom *pLef = pPeerDicti->getItem( "left" );
							CAtom *pConn = pPeerDicti->getItem( "connected" );

							strData += "<peer id=\"";
							strData += UTIL_HashToString( (*j).first );
							strData += "\"";

							if( pIP )
								strData += " ip=\"" + pIP->toString( ) + "\"";

							if( pUpped )
								strData += " uploaded=\"" + pUpped->toString( ) + "\"";

							if( pDowned )
								strData += " downloaded=\"" + pDowned->toString( ) + "\"";

							if( pLef && pLef->isLong( ) )
								strData += " left=\"" + pLef->toString( ) + "\"";

							if( pConn && pConn->isLong( ) )
								strData += " connected=\"" + CAtomLong( GetTime( ) - (unsigned long)( (CAtomLong *)pConn )->getValue( ) ).toString( ) + "\"";

							strData += " />\n";
						}
					}

					strData += "</peers>\n";
				}
			}

			strData += "</torrent>\n";
		}
	}

	strData += "</torrents>\n";

	FILE *pFile = NULL;

	if( ( pFile = fopen( m_strDumpXMLFile.c_str( ), "wb" ) ) == NULL )
	{
		UTIL_LogPrint( "tracker warning - unable to open %s for writing\n", m_strDumpXMLFile.c_str( ) );

		return;
	}

	fwrite( (void *)strData.c_str( ), sizeof( char ), strData.size( ), pFile );
	fclose( pFile );
}

void CTracker :: expireDownloaders( )
{
	// use getValuePtr here because we're screwing around with the map structure!

	if( m_pTimeDicti )
	{
		map<string, CAtom *> mapTimeDicti = m_pTimeDicti->getValue( );

		for( map<string, CAtom *> :: iterator i = mapTimeDicti.begin( ); i != mapTimeDicti.end( ); i++ )
		{
			if( (*i).second->isDicti( ) )
			{
				map<string, CAtom *> *pmapPeersDicti = ( (CAtomDicti *)(*i).second )->getValuePtr( );

				for( map<string, CAtom *> :: iterator j = pmapPeersDicti->begin( ); j != pmapPeersDicti->end( ); )
				{
					if( (*j).second->isLong( ) && ( (CAtomLong *)(*j).second )->getValue( ) < m_iPrevTime )
					{
						if( m_pDFile )
						{
							CAtom *pPeers = m_pDFile->getItem( (*i).first );

							if( pPeers && pPeers->isDicti( ) )
								( (CAtomDicti *)pPeers )->delItem( (*j).first );
						}

						delete (*j).second;

						pmapPeersDicti->erase( j++ );
					}
					else
						j++;
				}
			}
		}

		CountUniquePeers( );

		m_iPrevTime = GetTime( );

		if( m_bKeepDead )
			return;

		// delete empty hashes

		if( m_pDFile )
		{
			map<string, CAtom *> *pmapDicti = m_pDFile->getValuePtr( );

			for( map<string, CAtom *> :: iterator i = pmapDicti->begin( ); i != pmapDicti->end( ); )
			{
				if( (*i).second->isDicti( ) && ( (CAtomDicti *)(*i).second )->isEmpty( ) )
				{
					m_pTimeDicti->delItem( (*i).first );

					delete (*i).second;

					pmapDicti->erase( i++ );
				}
				else
					i++;
			}
		}
	}
}

void CTracker :: parseTorrents( const char *szDir )
{
	CAtomDicti *pAllowed = new CAtomDicti( );

#ifdef WIN32
	char szMatch[1024];
	memset( szMatch, 0, sizeof( char ) * 1024 );
	strcpy( szMatch, szDir );
	strcat( szMatch, "*.torrent" );

	WIN32_FIND_DATA fdt;

	HANDLE hFind = FindFirstFile( szMatch, &fdt );

	if( hFind != INVALID_HANDLE_VALUE )
	{
		do
#else
	DIR *pDir = opendir( szDir );

	struct dirent *dp;

	if( pDir )
	{
		while( ( dp = readdir( pDir ) ) )
#endif
		{
			// let the server accept new connections while parsing

			if( gpServer )
				gpServer->Update( false );

			char szFile[1024];
			memset( szFile, 0, sizeof( char ) * 1024 );
			strcpy( szFile, szDir );

#ifdef WIN32
			string strFileName = fdt.cFileName;
#else
			string strFileName = dp->d_name;
#endif

			strcat( szFile, strFileName.c_str( ) );

#ifndef WIN32
			if( strlen( szFile ) > strlen( ".torrent" ) )
			{
				if( strcmp( szFile + strlen( szFile ) - strlen( ".torrent" ), ".torrent" ) )
					continue;
			}
			else
				continue;
#endif

			CAtom *pFile = DecodeFile( szFile );

			if( pFile && pFile->isDicti( ) )
			{
				CAtom *pInfo = ( (CAtomDicti *)pFile )->getItem( "info" );

				if( pInfo && pInfo->isDicti( ) )
				{
					CAtomDicti *pInfoDicti = (CAtomDicti *)pInfo;

					string strHash = UTIL_InfoHash( pFile );

					CAtom *pName = pInfoDicti->getItem( "name" );
					CAtom *pLen = pInfoDicti->getItem( "length" );
					CAtom *pFiles = pInfoDicti->getItem( "files" );

					if( pName && ( pLen && pLen->isLong( ) || ( pFiles && pFiles->isList( ) ) ) )
					{
						CAtomList *pList = new CAtomList( );

						//
						// filename
						//

						pList->addItem( new CAtomString( strFileName ) );

						//
						// name
						//

						pList->addItem( new CAtomString( pName->toString( ) ) );

						//
						// added time (i.e. modification time)
						//

						char pTime[256];
						memset( pTime, 0, sizeof( char ) * 256 );

#ifdef WIN32
						FILETIME ft;
						SYSTEMTIME st;

						FileTimeToLocalFileTime( &fdt.ftLastWriteTime, &ft );
						FileTimeToSystemTime( &ft, &st );

						sprintf( pTime, "%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
#else
						struct stat info;

						stat( szFile, &info );

						strftime( pTime, sizeof( char ) * 256, "%Y-%m-%d %H:%M:%S", localtime( &info.st_mtime ) );
#endif

						pList->addItem( new CAtomString( pTime ) );

						//
						// file size
						//

						if( pLen )
							pList->addItem( new CAtomLong( *(CAtomLong *)pLen ) );
						else
						{
							int64 iSize = 0;

							vector<CAtom *> vecFiles = ( (CAtomList *)pFiles )->getValue( );

							for( vector<CAtom *> :: iterator j = vecFiles.begin( ); j != vecFiles.end( ); j++ )
							{
								if( (*j)->isDicti( ) )
								{
									CAtom *pSubLength = ( (CAtomDicti *)(*j) )->getItem( "length" );

									if( pSubLength && pSubLength->isLong( ) )
										iSize += ( (CAtomLong *)pSubLength )->getValue( );
								}
							}

							pList->addItem( new CAtomLong( iSize ) );
						}

						//
						// number of files
						//

						if( pLen )
							pList->addItem( new CAtomInt( 1 ) );
						else
							pList->addItem( new CAtomInt( ( (CAtomList *)pFiles )->getValue( ).size( ) ) );

						//
						// file comment
						//

						CAtom *pComment = ( (CAtomDicti *)pFile )->getItem( "comment" );

						if( pComment )
							pList->addItem( new CAtomString( pComment->toString( ) ) );
						else
							pList->addItem( new CAtomString( ) );

						pAllowed->setItem( strHash, pList );

						if( m_pDFile && m_bKeepDead )
						{
							if( !m_pDFile->getItem( strHash ) )
								m_pDFile->setItem( strHash, new CAtomDicti( ) );
						}
					}
					else
					{
						UTIL_LogPrint( "error parsing torrents - %s has an incomplete or invalid info key, skipping\n", strFileName.c_str( ) );

						if( m_bDeleteInvalid )
						{
							if( m_strArchiveDir.empty( ) )
								UTIL_DeleteFile( szFile );
							else
								UTIL_MoveFile( szFile, ( m_strArchiveDir + strFileName ).c_str( ) );
						}
					}
				}
				else
				{
					UTIL_LogPrint( "error parsing torrents - %s doesn't have an info key or info is not a valid bencoded dictionary, skipping\n", strFileName.c_str( ) );

					if( m_bDeleteInvalid )
					{
						if( m_strArchiveDir.empty( ) )
							UTIL_DeleteFile( szFile );
						else
							UTIL_MoveFile( szFile, ( m_strArchiveDir + strFileName ).c_str( ) );
					}
				}
			}
			else
			{
				UTIL_LogPrint( "error parsing torrents - %s is not a valid bencoded dictionary or unable to decode, skipping\n", strFileName.c_str( ) );

				if( m_bDeleteInvalid )
				{
					if( m_strArchiveDir.empty( ) )
						UTIL_DeleteFile( szFile );
					else
						UTIL_MoveFile( szFile, ( m_strArchiveDir + strFileName ).c_str( ) );
				}
			}

			delete pFile;

			pFile = NULL;

#ifdef WIN32
		} while( FindNextFile( hFind, &fdt ) );

		FindClose( hFind );
#else
		}

		closedir( pDir );
#endif
	}
	else
		UTIL_LogPrint( "error parsing torrents - unable to open %s or no torrents found\n", szDir );

	if( m_pAllowed )
		delete m_pAllowed;

	m_pAllowed = pAllowed;
}

void CTracker :: parseTorrent( const char *szFile )
{
#ifdef WIN32
	HANDLE hFile = CreateFile( szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if( hFile == INVALID_HANDLE_VALUE )
	{
		UTIL_LogPrint( "error parsing torrent - unable to open %s for reading\n", szFile );

		return;
	}
#endif

	CAtom *pTorrent = DecodeFile( szFile );

	if( pTorrent && pTorrent->isDicti( ) )
	{
		CAtom *pInfo = ( (CAtomDicti *)pTorrent )->getItem( "info" );

		if( pInfo && pInfo->isDicti( ) )
		{
			CAtomDicti *pInfoDicti = (CAtomDicti *)pInfo;

			string strHash = UTIL_InfoHash( pTorrent );

			CAtom *pName = pInfoDicti->getItem( "name" );
			CAtom *pLen = pInfoDicti->getItem( "length" );
			CAtom *pFiles = pInfoDicti->getItem( "files" );

			if( pName && ( pLen && pLen->isLong( ) || ( pFiles && pFiles->isList( ) ) ) )
			{
				CAtomList *pList = new CAtomList( );

				//
				// filename
				//

				pList->addItem( new CAtomString( UTIL_StripPath( szFile ).c_str( ) ) );

				//
				// name
				//

				pList->addItem( new CAtomString( pName->toString( ) ) );

				//
				// added time (i.e. modification time)
				//

				char pTime[256];
				memset( pTime, 0, sizeof( char ) * 256 );

#ifdef WIN32
				FILETIME ft;
				SYSTEMTIME st;

				GetFileTime( hFile, NULL, NULL, &ft );

				FileTimeToLocalFileTime( &ft, &ft );
				FileTimeToSystemTime( &ft, &st );

				sprintf( pTime, "%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
#else
				struct stat info;

				stat( szFile, &info );

				strftime( pTime, sizeof( char ) * 256, "%Y-%m-%d %H:%M:%S", localtime( &info.st_mtime ) );
#endif

				pList->addItem( new CAtomString( pTime ) );

				//
				// file size
				//

				if( pLen )
					pList->addItem( new CAtomLong( *(CAtomLong *)pLen ) );
				else
				{
					int64 iSize = 0;

					vector<CAtom *> vecFiles = ( (CAtomList *)pFiles )->getValue( );

					for( vector<CAtom *> :: iterator j = vecFiles.begin( ); j != vecFiles.end( ); j++ )
					{
						if( (*j)->isDicti( ) )
						{
							CAtom *pSubLength = ( (CAtomDicti *)(*j) )->getItem( "length" );

							if( pSubLength && pSubLength->isLong( ) )
								iSize += ( (CAtomLong *)pSubLength )->getValue( );
						}
					}

					pList->addItem( new CAtomLong( iSize ) );
				}

				//
				// number of files
				//

				if( pLen )
					pList->addItem( new CAtomInt( 1 ) );
				else
					pList->addItem( new CAtomInt( ( (CAtomList *)pFiles )->getValue( ).size( ) ) );

				//
				// file comment
				//

				CAtom *pComment = ( (CAtomDicti *)pTorrent )->getItem( "comment" );

				if( pComment )
					pList->addItem( new CAtomString( pComment->toString( ) ) );
				else
					pList->addItem( new CAtomString( ) );

				m_pAllowed->setItem( strHash, pList );

				if( m_pDFile && m_bKeepDead )
				{
					if( !m_pDFile->getItem( strHash ) )
						m_pDFile->setItem( strHash, new CAtomDicti( ) );
				}
			}
			else
			{
				UTIL_LogPrint( "error parsing torrent - %s has an incomplete or invalid info key, skipping\n", szFile );

				if( m_bDeleteInvalid )
				{
					if( m_strArchiveDir.empty( ) )
						UTIL_DeleteFile( szFile );
					else
						UTIL_MoveFile( szFile, ( m_strArchiveDir + UTIL_StripPath( szFile ) ).c_str( ) );
				}
			}
		}
		else
		{
			UTIL_LogPrint( "error parsing torrent - %s doesn't have an info key or info is not a valid bencoded dictionary, skipping\n", szFile );

			if( m_bDeleteInvalid )
			{
				if( m_strArchiveDir.empty( ) )
					UTIL_DeleteFile( szFile );
				else
					UTIL_MoveFile( szFile, ( m_strArchiveDir + UTIL_StripPath( szFile ) ).c_str( ) );
			}
		}
	}
	else
	{
		UTIL_LogPrint( "error parsing torrent - %s is not a valid bencoded dictionary or unable to decode, skipping\n", szFile );

		if( m_bDeleteInvalid )
		{
			if( m_strArchiveDir.empty( ) )
				UTIL_DeleteFile( szFile );
			else
				UTIL_MoveFile( szFile, ( m_strArchiveDir + UTIL_StripPath( szFile ) ).c_str( ) );
		}
	}

	delete pTorrent;

	pTorrent = NULL;

#ifdef WIN32
	CloseHandle( hFile );
#endif
}

bool CTracker :: checkTag( string &strTag )
{
	if( m_vecTags.size( ) == 0 )
		return true;

	for( vector< pair<string, string> > :: iterator i = m_vecTags.begin( ); i != m_vecTags.end( ); i++ )
	{
		if( (*i).first == strTag )
			return true;
	}

	return false;
}

void CTracker :: addTag( string strInfoHash, string strTag, string strName, string strUploader, string strInfoLink )
{
	if( !m_pTags->getItem( strInfoHash ) )
		m_pTags->setItem( strInfoHash, new CAtomDicti( ) );

	CAtom *pTag = m_pTags->getItem( strInfoHash );

	if( pTag && pTag->isDicti( ) )
	{
		( (CAtomDicti *)pTag )->setItem( "tag", new CAtomString( strTag ) );

		if( !strName.empty( ) )
			( (CAtomDicti *)pTag )->setItem( "name", new CAtomString( strName ) );

		if( !strUploader.empty( ) )
			( (CAtomDicti *)pTag )->setItem( "uploader", new CAtomString( strUploader ) );

		// The Trinity Edition - Addition Begins
		// The following code allows the removal of the Uploader's name for any 
		// given torrent by setting this value to REMOVE and saving changes

		// This code is brought to you by courtesy of user ConfusedFish :)

		if( ( !strUploader.empty( ) ) && ( strUploader!="REMOVE" ) )
			( (CAtomDicti *)pTag )->setItem( "uploader", new CAtomString( strUploader ) );

		if( strUploader == "REMOVE" )
			( (CAtomDicti *)pTag )->delItem( "uploader" );

		// ------------------------------------------------- End of Addition

		if( !strInfoLink.empty( ) )
			( (CAtomDicti *)pTag )->setItem( "infolink", new CAtomString( strInfoLink ) );

		// The Trinity Edition - Addition Begins
		// The following code allows the removal of the InfoLink for any 
		// given torrent by setting this value to REMOVE and saving changes

		// This code is brought to you by courtesy of user ConfusedFish :)

		if( ( !strInfoLink.empty( ) ) && ( strInfoLink!="REMOVE" ) )
			( (CAtomDicti *)pTag )->setItem( "infolink", new CAtomString( strInfoLink ) );
			
		if( strInfoLink == "REMOVE" )
			( (CAtomDicti *)pTag )->delItem( "infolink" );

		// ------------------------------------------------- End of Addition

	}

	saveTags( );
}

void CTracker :: deleteTag( string strInfoHash )
{
	m_pTags->delItem( strInfoHash );

	saveTags( );
}

user_t CTracker :: checkUser( string strLogin, string strMD5 )
{
	user_t user;

	user.iAccess = m_iGuestAccess;

	// if no users exist, grant full access

	if( m_pUsers->isEmpty( ) )
		user.iAccess = ~0;

	CAtom *pUser = m_pUsers->getItem( strLogin );

	if( pUser && pUser->isDicti( ) )
	{
		CAtom *pMD5 = ( (CAtomDicti *)pUser )->getItem( "md5" );
		CAtom *pAccess = ( (CAtomDicti *)pUser )->getItem( "access" );
		CAtom *pMail = ( (CAtomDicti *)pUser )->getItem( "email" );
		CAtom *pCreated = ( (CAtomDicti *)pUser )->getItem( "created" );

		if( pMD5 && pAccess && pAccess->isLong( ) && pMail )
		{
			// check hash

			if( strMD5 == pMD5->toString( ) )
			{
				user.strLogin = strLogin;
				user.strLowerLogin = UTIL_ToLower( user.strLogin );
				user.strMD5 = strMD5;
				user.strMail = pMail->toString( );
				user.strLowerMail = UTIL_ToLower( user.strMail );
				user.iAccess = (int)( (CAtomLong *)pAccess )->getValue( );
				user.strCreated = pCreated->toString( );
			}
		}
	}

	return user;
}

void CTracker :: addUser( string strLogin, string strPass, int iAccess, string strMail )
{
	CAtomDicti *pUser = new CAtomDicti( );

	// calculate md5 hash of A1

	string strA1 = strLogin + ":" + gstrRealm + ":" + strPass;

	unsigned char szMD5[16];

	MD5_CTX md5;

	MD5Init( &md5 );
	MD5Update( &md5, (unsigned char *)strA1.c_str( ), strA1.size( ) );
	MD5Final( szMD5, &md5 );

	pUser->setItem( "md5", new CAtomString( string( (char *)szMD5, 16 ) ) );
	pUser->setItem( "access", new CAtomLong( iAccess ) );
	pUser->setItem( "email", new CAtomString( strMail ) );

	time_t tNow = time( NULL );

	char pTime[256];
	memset( pTime, 0, sizeof( char ) * 256 );
	strftime( pTime, sizeof( char ) * 256, "%Y %m/%d %H:%M:%S", localtime( &tNow ) );

	pUser->setItem( "created", new CAtomString( pTime ) );

	m_pUsers->setItem( strLogin, pUser );

	saveUsers( );
}

void CTracker :: deleteUser( string strLogin )
{
	m_pUsers->delItem( strLogin );

	saveUsers( );
}

void CTracker :: CountUniquePeers( )
{
	delete m_pIPs;

	m_pIPs = new CAtomDicti( );

	map<string, CAtom *> mapDicti = m_pDFile->getValue( );

	for( map<string, CAtom *> :: iterator i = mapDicti.begin( ); i != mapDicti.end( ); i++ )
	{
		if( (*i).second->isDicti( ) )
		{
			map<string, CAtom *> mapPeersDicti = ( (CAtomDicti *)(*i).second )->getValue( );

			for( map<string, CAtom *> :: iterator j = mapPeersDicti.begin( ); j != mapPeersDicti.end( ); j++ )
			{
				if( (*j).second->isDicti( ) )
				{
					CAtom *pIP = ( (CAtomDicti *)(*j).second )->getItem( "ip" );

					if( pIP )
						AddUniquePeer( pIP->toString( ) );
				}
			}
		}
	}
}

void CTracker :: AddUniquePeer( string strIP )
{
	// increment unique count for this ip

	CAtom *pNum = m_pIPs->getItem( strIP );

	int iNum = 1;

	if( pNum && pNum->isInt( ) )
		iNum = ( (CAtomInt *)pNum )->getValue( ) + 1;

	m_pIPs->setItem( strIP, new CAtomInt( iNum ) );
}

void CTracker :: RemoveUniquePeer( string strIP )
{
	// decrement unique count for this ip

	CAtom *pNum = m_pIPs->getItem( strIP );

	int iNum = 0;

	if( pNum && pNum->isInt( ) )
		iNum = ( (CAtomInt *)pNum )->getValue( ) - 1;

	if( iNum > 0 )
		m_pIPs->setItem( strIP, new CAtomInt( iNum ) );
	else
		m_pIPs->delItem( strIP );
}

void CTracker :: QueueAnnounce( CAtomDicti *pParams )
{
	// normally called from link.cpp

	m_mtxQueued.Claim( );
	m_vecQueued.push_back( new CAtomDicti( *pParams ) );
	m_mtxQueued.Release( );
}

void CTracker :: serverResponseGET( struct request_t *pRequest, struct response_t *pResponse, user_t user )
{
	if( pRequest->strURL == "/" || pRequest->strURL == "/index.html" )
		serverResponseIndex( pRequest, pResponse, user );
	else if( pRequest->strURL == "/announce" )
		serverResponseAnnounce( pRequest, pResponse, user );
	else if( pRequest->strURL == "/scrape" )
		serverResponseScrape( pRequest, pResponse, user );
	else if( pRequest->strURL == "/stats.html" )
		serverResponseStats( pRequest, pResponse, user );
	else if( pRequest->strURL == "/comments.html" )
		serverResponseComments( pRequest, pResponse, user );
	else if( pRequest->strURL == "/admin.html" )
		serverResponseAdmin( pRequest, pResponse, user );
	else if( pRequest->strURL.substr( 0, 7 ) == "/files/" )
		serverResponseFile( pRequest, pResponse, user );
	else if( pRequest->strURL == "/torrent.html" )
		serverResponseTorrent( pRequest, pResponse, user );
	else if( pRequest->strURL == "/upload.html" )
		serverResponseUploadGET( pRequest, pResponse, user );
	else if( pRequest->strURL == "/users.html" )
		serverResponseUsers( pRequest, pResponse, user );
	else if( pRequest->strURL == "/login.html" )
		serverResponseLogin( pRequest, pResponse, user );
	else if( pRequest->strURL == "/info.html" )
		serverResponseInfo( pRequest, pResponse, user );
	else if( pRequest->strURL == "/signup.html" )
		serverResponseSignup( pRequest, pResponse, user );
	else
		pResponse->strCode = "404 Not Found";
}

void CTracker :: serverResponsePOST( struct request_t *pRequest, struct response_t *pResponse, CAtomList *pPost, user_t user )
{
	if( pPost )
	{
		if( pRequest->strURL == "/upload.html" )
			serverResponseUploadPOST( pRequest, pResponse, pPost, user );
		else
			pResponse->strCode = "404 Not Found";
	}
	else
		pResponse->strCode = "500 Server Error";
}

void CTracker :: Update( )
{
	if( !m_strAllowedDir.empty( ) && m_iParseAllowedInterval > 0 )
	{
		if( GetTime( ) > m_iParseAllowedNext )
		{
			if( gbDebug )
				UTIL_LogPrint( "tracker - parsing torrents (%s)\n", m_strAllowedDir.c_str( ) );

			// don't parse torrents again until we're done since parseTorrents calls gpServer->Update( ) which calls m_pTracker->Update( )

			m_iParseAllowedNext = GetTime( ) + 9999;

			parseTorrents( m_strAllowedDir.c_str( ) );

			m_iParseAllowedNext = GetTime( ) + m_iParseAllowedInterval * 60;
		}
	}

	if( GetTime( ) > m_iSaveDFileNext )
	{
		if( gbDebug )
			UTIL_LogPrint( "tracker - saving dfile (%s)\n", m_strDFile.c_str( ) );

		saveDFile( );

		m_iSaveDFileNext = GetTime( ) + m_iSaveDFileInterval;
	}

	// The Trinity Edition - Modification Begins (TRINCUSTSTATDOX)
	// Adds the SIGNUP AND UPLOAD Static Header and Footer to be handled by the Static Refresh Value

	if( !m_strStaticHeaderFile.empty( ) || !m_strStaticFooterFile.empty( ) || !m_strStaticHeaderSignupFile.empty( ) || !m_strStaticFooterSignupFile.empty( ) || !m_strStaticHeaderUploadFile.empty( ) || !m_strStaticFooterUploadFile.empty( ))
	{
		if( GetTime( ) > m_iRefreshStaticNext )
		{
			if( gbDebug )
				UTIL_LogPrint( "tracker - refreshing static header and footer\n" );

			if( !m_strStaticHeaderFile.empty( ) )
				m_strStaticHeader = UTIL_ReadFile( m_strStaticHeaderFile.c_str( ) );

			if( !m_strStaticFooterFile.empty( ) )
				m_strStaticFooter = UTIL_ReadFile( m_strStaticFooterFile.c_str( ) );

			if( !m_strStaticHeaderSignupFile.empty( ) )
				m_strStaticHeaderSignup = UTIL_ReadFile( m_strStaticHeaderSignupFile.c_str( ) );

			if( !m_strStaticFooterSignupFile.empty( ) )
				m_strStaticFooterSignup = UTIL_ReadFile( m_strStaticFooterSignupFile.c_str( ) );
				
			if( !m_strStaticHeaderUploadFile.empty( ) )
				m_strStaticHeaderUpload = UTIL_ReadFile( m_strStaticHeaderUploadFile.c_str( ) );

			if( !m_strStaticFooterUploadFile.empty( ) )
				m_strStaticFooterUpload = UTIL_ReadFile( m_strStaticFooterUploadFile.c_str( ) );

			m_iRefreshStaticNext = GetTime( ) + m_iRefreshStaticInterval * 60;
		}
	}

	/* Original Source Code:
	if( !m_strStaticHeaderFile.empty( ) || !m_strStaticFooterFile.empty( ) )
	{
		if( GetTime( ) > m_iRefreshStaticNext )
		{
			if( gbDebug )
				UTIL_LogPrint( "tracker - refreshing static header and footer\n" );

			if( !m_strStaticHeaderFile.empty( ) )
				m_strStaticHeader = UTIL_ReadFile( m_strStaticHeaderFile.c_str( ) );

			if( !m_strStaticFooterFile.empty( ) )
				m_strStaticFooter = UTIL_ReadFile( m_strStaticFooterFile.c_str( ) );

			m_iRefreshStaticNext = GetTime( ) + m_iRefreshStaticInterval * 60;
		}
	}
	*/

	if( GetTime( ) > m_iDownloaderTimeOutNext )
	{
		if( gbDebug )
			UTIL_LogPrint( "tracker - expiring downloaders\n" );

		expireDownloaders( );

		m_iDownloaderTimeOutNext = GetTime( ) + m_iDownloaderTimeOutInterval;
	}

	if( !m_strDumpXMLFile.empty( ) )
	{
		if( GetTime( ) > m_iDumpXMLNext )
		{
			if( gbDebug )
				UTIL_LogPrint( "tracker - dumping xml\n" );

			saveXML( );

			m_iDumpXMLNext = GetTime( ) + m_iDumpXMLInterval;
		}
	}

	// queued announces

	m_mtxQueued.Claim( );
	vector<CAtomDicti *> vecTemp = m_vecQueued;
	m_vecQueued.clear( );
	m_mtxQueued.Release( );

	for( vector<CAtomDicti *> :: iterator i = vecTemp.begin( ); i != vecTemp.end( ); i++ )
	{
		CAtom *pInfoHash = (*i)->getItem( "info_hash" );
		CAtom *pIP = (*i)->getItem( "ip" );
		CAtom *pEvent = (*i)->getItem( "event" );
		CAtom *pPort = (*i)->getItem( "port" );
		CAtom *pUploaded = (*i)->getItem( "uploaded" );
		CAtom *pDownloaded = (*i)->getItem( "downloaded" );
		CAtom *pLeft = (*i)->getItem( "left" );
		CAtom *pPeerID = (*i)->getItem( "peer_id" );

		string strInfoHash;
		string strIP;
		string strEvent;
		int iPort;
		int64 iUploaded;
		int64 iDownloaded;
		int64 iLeft;
		string strPeerID;

		if( pInfoHash && pIP && pPort && pUploaded && pDownloaded && pLeft && pPeerID )
		{
			strInfoHash = pInfoHash->toString( );
			strIP = pIP->toString( );
			iPort = (int)( (CAtomLong *)pPort )->getValue( );
			iUploaded = ( (CAtomLong *)pUploaded )->getValue( );
			iDownloaded = ( (CAtomLong *)pDownloaded )->getValue( );
			iLeft = ( (CAtomLong *)pLeft )->getValue( );
			strPeerID = pPeerID->toString( );
		}

		if( pEvent )
			strEvent = pEvent->toString( );

		//
		// begin announce
		//

		if( m_pAllowed )
		{
			if( !m_pAllowed->getItem( strInfoHash ) )
				continue;
		}

		if( strEvent != "stopped" )
		{
			if( m_pDFile )
			{
				if( !m_pDFile->getItem( strInfoHash ) )
					m_pDFile->setItem( strInfoHash, new CAtomDicti( ) );

				CAtom *pPeers = m_pDFile->getItem( strInfoHash );

				if( pPeers && pPeers->isDicti( ) )
				{
					CAtom *pPeer = ( (CAtomDicti *)pPeers )->getItem( strPeerID );

					if( pPeer && pPeer->isDicti( ) )
					{
						( (CAtomDicti *)pPeer )->setItem( "uploaded", new CAtomLong( iUploaded ) );
						( (CAtomDicti *)pPeer )->setItem( "downloaded", new CAtomLong( iDownloaded ) );
						( (CAtomDicti *)pPeer )->setItem( "left", new CAtomLong( iLeft ) );
					}
					else
					{
						CAtomDicti *pPeerDicti = new CAtomDicti( );

						pPeerDicti->setItem( "ip", new CAtomString( strIP ) );
						pPeerDicti->setItem( "port", new CAtomLong( iPort ) );
						pPeerDicti->setItem( "uploaded", new CAtomLong( iUploaded ) );
						pPeerDicti->setItem( "downloaded", new CAtomLong( iDownloaded ) );
						pPeerDicti->setItem( "left", new CAtomLong( iLeft ) );
						pPeerDicti->setItem( "connected", new CAtomLong( GetTime( ) ) );

						( (CAtomDicti *)pPeers )->setItem( strPeerID, pPeerDicti );

						if( m_bCountUniquePeers )
							AddUniquePeer( strIP );
					}

					if( m_pTimeDicti )
					{
						if( !m_pTimeDicti->getItem( strInfoHash ) )
							m_pTimeDicti->setItem( strInfoHash, new CAtomDicti( ) );

						CAtom *pTS = m_pTimeDicti->getItem( strInfoHash );

						if( pTS && pTS->isDicti( ) )
							( (CAtomDicti *)pTS )->setItem( strPeerID, new CAtomLong( GetTime( ) ) );
					}
				}
			}

			if( strEvent == "completed" )
			{
				if( m_pCompleted )
				{
					CAtom *pCompleted = m_pCompleted->getItem( strInfoHash );

					int64 iCompleted = 0;

					if( pCompleted && pCompleted->isLong( ) )
						iCompleted = ( (CAtomLong *)pCompleted )->getValue( );

					m_pCompleted->setItem( strInfoHash, new CAtomLong( iCompleted + 1 ) );
				}
			}
		}
		else
		{
			// strEvent == "stopped"

			if( m_pDFile )
			{
				if( !m_pDFile->getItem( strInfoHash ) )
					m_pDFile->setItem( strInfoHash, new CAtomDicti( ) );

				CAtom *pPeers = m_pDFile->getItem( strInfoHash );

				if( pPeers && pPeers->isDicti( ) )
				{
					CAtom *pPeer = ( (CAtomDicti *)pPeers )->getItem( strPeerID );

					if( pPeer && pPeer->isDicti( ) )
					{
						CAtom *pPeerIP = ( (CAtomDicti *)pPeer )->getItem( "ip" );

						if( pPeerIP )
						{
							if( pPeerIP->toString( ) == strIP )
							{
								( (CAtomDicti *)pPeers )->delItem( strPeerID );

								if( m_pTimeDicti )
								{
									if( !m_pTimeDicti->getItem( strInfoHash ) )
										m_pTimeDicti->setItem( strInfoHash, new CAtomDicti( ) );

									CAtom *pTS = m_pTimeDicti->getItem( strInfoHash );

									if( pTS && pTS->isDicti( ) )
										( (CAtomDicti *)pTS )->delItem( strPeerID );
								}

								if( m_bCountUniquePeers )
									RemoveUniquePeer( strIP );
							}
						}
					}
				}
			}
		}

		//
		// end announce
		//

		delete *i;
	}
}
