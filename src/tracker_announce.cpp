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

#include "bnbt.h"
#include "atom.h"
#include "bencode.h"
#include "link.h"
#include "tracker.h"
#include "util.h"

void CTracker :: serverResponseAnnounce( struct request_t *pRequest, struct response_t *pResponse, user_t user )
{
	pResponse->strCode = "200 OK";

	pResponse->mapHeaders.insert( pair<string, string>( "Content-Type", "text/plain" ) );

	// retrieve info hash

	string strInfoHash = pRequest->mapParams["info_hash"];

	if( strInfoHash.empty( ) )
	{
		pResponse->strContent = UTIL_FailureReason( "info hash missing" );
		pResponse->bCompressOK = false;

		return;
	}

	if( m_pAllowed )
	{
		if( !m_pAllowed->getItem( strInfoHash ) )
		{
			pResponse->strContent = UTIL_FailureReason( "requested download is not authorized for use with this tracker" );
			pResponse->bCompressOK = false;

			return;
		}
	}

	// retrieve ip

	string strIP = inet_ntoa( pRequest->sin.sin_addr );
	string strTemp = pRequest->mapParams["ip"];

	if( !strTemp.empty( ) && strTemp.find_first_not_of( "1234567890." ) == string :: npos )
		strIP = strTemp;

	// retrieve a ton of parameters

	string strEvent = pRequest->mapParams["event"];
	string strPort = pRequest->mapParams["port"];
	string strUploaded = pRequest->mapParams["uploaded"];
	string strDownloaded = pRequest->mapParams["downloaded"];
	string strLeft = pRequest->mapParams["left"];
	string strPeerID = pRequest->mapParams["peer_id"];
	string strNumWant = pRequest->mapParams["num_want"];
	string strNoPeerID = pRequest->mapParams["no_peer_id"];
	string strCompact = pRequest->mapParams["compact"];

	if( !strEvent.empty( ) && strEvent != "started" && strEvent != "completed" && strEvent != "stopped" )
	{
		pResponse->strContent = UTIL_FailureReason( "invalid event" );
		pResponse->bCompressOK = false;

		return;
	}

	if( strPort.empty( ) )
	{
		pResponse->strContent = UTIL_FailureReason( "port missing" );
		pResponse->bCompressOK = false;

		return;
	}

	if( strUploaded.empty( ) )
	{
		pResponse->strContent = UTIL_FailureReason( "uploaded missing" );
		pResponse->bCompressOK = false;

		return;
	}

	if( strDownloaded.empty( ) )
	{
		pResponse->strContent = UTIL_FailureReason( "downloaded missing" );
		pResponse->bCompressOK = false;

		return;
	}

	if( strLeft.empty( ) )
	{
		pResponse->strContent = UTIL_FailureReason( "left missing" );
		pResponse->bCompressOK = false;

		return;
	}

	if( strPeerID.size( ) != 20 )
	{
		pResponse->strContent = UTIL_FailureReason( "peer id not of length 20" );
		pResponse->bCompressOK = false;

		return;
	}

	int iPort = atoi( strPort.c_str( ) );

	int64 iUploaded = UTIL_StringTo64( strUploaded.c_str( ) );
	int64 iDownloaded = UTIL_StringTo64( strDownloaded.c_str( ) );
	int64 iLeft = UTIL_StringTo64( strLeft.c_str( ) );

	unsigned int iRSize = m_iResponseSize;

	if( !strNumWant.empty( ) )
		iRSize = atoi( strNumWant.c_str( ) );

	if( iRSize > (unsigned int)m_iMaxGive )
		iRSize = (unsigned int)m_iMaxGive;

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

	// link

	if( gpLinkServer || gpLink )
	{
		CAtomDicti *pAnnounce = new CAtomDicti( );

		pAnnounce->setItem( "info_hash", new CAtomString( strInfoHash ) );
		pAnnounce->setItem( "ip", new CAtomString( strIP ) );

		if( !strEvent.empty( ) )
			pAnnounce->setItem( "event", new CAtomString( strEvent ) );

		pAnnounce->setItem( "port", new CAtomInt( iPort ) );
		pAnnounce->setItem( "uploaded", new CAtomLong( iUploaded ) );
		pAnnounce->setItem( "downloaded", new CAtomLong( iDownloaded ) );
		pAnnounce->setItem( "left", new CAtomLong( iLeft ) );
		pAnnounce->setItem( "peer_id", new CAtomString( strPeerID ) );

		struct linkmsg_t lm;

		lm.len = pAnnounce->EncodedLength( );
		lm.type = LINKMSG_ANNOUNCE;
		lm.msg = Encode( pAnnounce );

		if( gpLinkServer )
			gpLinkServer->Queue( lm );
		else
			gpLink->Queue( lm );

		delete pAnnounce;
	}

	// populate cache

	if( !m_pCached->getItem( strInfoHash ) )
		m_pCached->setItem( strInfoHash, new CAtomList( ) );

	CAtom *pCache = m_pCached->getItem( strInfoHash );

	if( pCache && pCache->isList( ) )
	{
		CAtomList *pCacheList = (CAtomList *)pCache;

		if( pCacheList->getValue( ).size( ) < iRSize )
		{
			if( m_pDFile )
			{
				if( !m_pDFile->getItem( strInfoHash ) )
					m_pDFile->setItem( strInfoHash, new CAtomDicti( ) );

				CAtom *pPeers = m_pDFile->getItem( strInfoHash );

				if( pPeers && pPeers->isDicti( ) )
				{
					map<string, CAtom *> mapPeersDicti = ( (CAtomDicti *)pPeers )->getValue( );

					for( map<string, CAtom *> :: iterator i = mapPeersDicti.begin( ); i != mapPeersDicti.end( ); i++ )
					{
						if( (*i).second->isDicti( ) )
						{
							CAtomDicti *pAdd = new CAtomDicti( );

							CAtom *pIP = ( (CAtomDicti *)(*i).second )->getItem( "ip" );
							CAtom *pPort = ( (CAtomDicti *)(*i).second )->getItem( "port" );

							pAdd->setItem( "peer id", new CAtomString( (*i).first ) );

							if( pIP )
								pAdd->setItem( "ip", new CAtomString( pIP->toString( ) ) );

							if( pPort && pPort->isLong( ) )
								pAdd->setItem( "port", new CAtomLong( *(CAtomLong *)pPort ) );

							pCacheList->addItem( pAdd );
						}
					}
				}
			}

			pCacheList->Randomize( );
		}
	}

	// clamp response

	if( pCache && pCache->isList( ) )
	{
		CAtomList *pCacheList = (CAtomList *)pCache;

		CAtom *pPeers = NULL;

		if( strCompact == "1" )
		{
			// compact announce

			string strPeers;

			vector<CAtom *> *vecList = pCacheList->getValuePtr( );

			for( vector<CAtom *> :: iterator i = vecList->begin( ); i != vecList->end( ); )
			{
				if( (*i)->isDicti( ) )
				{
					// if( strCompact.size( ) / 6 == iRSize ) -- FIXED COMPACT ANNOUNCE BUG
					if( strPeers.size( ) / 6 >= iRSize )
						break;

					CAtom *pIP = ( (CAtomDicti *)(*i) )->getItem( "ip" );
					CAtom *pPort = ( (CAtomDicti *)(*i) )->getItem( "port" );

					if( pIP && pPort && pPort->isLong( ) )
						strPeers += UTIL_Compact( pIP->toString( ), (unsigned int)( (CAtomLong *)pPort )->getValue( ) );

					delete *i;

					i = vecList->erase( i );
				}
				else
					i++;
			}

			pPeers = new CAtomString( strPeers );

			// don't compress

			pResponse->bCompressOK = false;
		}
		else
		{
			// regular announce

			CAtomList *pPeersList = new CAtomList( );

			vector<CAtom *> *vecList = pCacheList->getValuePtr( );

			for( vector<CAtom *> :: iterator i = vecList->begin( ); i != vecList->end( ); )
			{
				if( (*i)->isDicti( ) )
				{
					if( pPeersList->getValue( ).size( ) == iRSize )
						break;

					if( strNoPeerID == "1" )
						( (CAtomDicti *)(*i) )->delItem( "peer id" );

					pPeersList->addItem( new CAtomDicti( *(CAtomDicti *)(*i) ) );

					delete *i;

					i = vecList->erase( i );
				}
				else
					i++;
			}

			pPeers = pPeersList;
		}

		CAtomDicti *pData = new CAtomDicti( );

		pData->setItem( "interval", new CAtomLong( m_iAnnounceInterval ) );
		pData->setItem( "peers", pPeers );

		pResponse->strContent = Encode( pData );

		delete pData;
	}
}
