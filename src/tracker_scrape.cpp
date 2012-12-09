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
#include "tracker.h"
#include "util.h"

void CTracker :: serverResponseScrape( struct request_t *pRequest, struct response_t *pResponse, user_t user )
{
	if( !m_bAllowScrape )
	{
		pResponse->strCode = "403 Forbidden";

		return;
	}

	pResponse->strCode = "200 OK";

	pResponse->mapHeaders.insert( pair<string, string>( "Content-Type", "text/plain" ) );

	// retrieve info hash (note that the client sends an actual hash so don't convert it)

	string strInfoHash = pRequest->mapParams["info_hash"];

	CAtomDicti *pScrape = new CAtomDicti( );
	CAtomDicti *pFiles = new CAtomDicti( );
	CAtomDicti *pFlags = new CAtomDicti( );

	pScrape->setItem( "files", pFiles );
	pScrape->setItem( "flags", pFlags );

	pFlags->setItem( "min_request_interval", new CAtomLong( m_iMinRequestInterval ) );

	if( m_pDFile )
	{
		if( strInfoHash.empty( ) )
		{
			//
			// full scrape
			//

			map<string, CAtom *> mapDicti = m_pDFile->getValue( );

			for( map<string, CAtom *> :: iterator i = mapDicti.begin( ); i != mapDicti.end( ); i++ )
			{
				if( (*i).second->isDicti( ) )
				{
					CAtomDicti *pHuh = new CAtomDicti( );

					map<string, CAtom *> mapPeersDicti = ( (CAtomDicti *)(*i).second )->getValue( );

					unsigned long iSeeders = 0;
					unsigned long iLeechers = 0;

					for( map<string, CAtom *> :: iterator j = mapPeersDicti.begin( ); j != mapPeersDicti.end( ); j++ )
					{
						if( (*j).second->isDicti( ) )
						{
							CAtom *pLeft = ( (CAtomDicti *)(*j).second )->getItem( "left" );

							if( pLeft && pLeft->isLong( ) )
							{
								if( ( (CAtomLong *)pLeft )->getValue( ) == 0 )
									iSeeders++;
								else
									iLeechers++;
							}
						}
					}

					pHuh->setItem( "complete", new CAtomInt( iSeeders ) );
					pHuh->setItem( "incomplete", new CAtomInt( iLeechers ) );

					if( m_pAllowed )
					{
						CAtom *pList = m_pAllowed->getItem( (*i).first );

						if( pList && pList->isList( ) )
						{
							vector<CAtom *> vecTorrent = ( (CAtomList *)pList )->getValue( );

							if( vecTorrent.size( ) == 6 )
							{
								CAtom *pName = vecTorrent[1];

								if( pName )
									pHuh->setItem( "name", new CAtomString( pName->toString( ) ) );
							}
						}
					}

					if( m_pCompleted )
					{
						CAtom *pCompleted = m_pCompleted->getItem( (*i).first );

						if( pCompleted && pCompleted->isLong( ) )
							pHuh->setItem( "downloaded", new CAtomLong( ( (CAtomLong *)pCompleted )->getValue( ) ) );
					}

					pFiles->setItem( (*i).first, pHuh );
				}
			}
		}
		else
		{
			//
			// single scrape
			//

			CAtom *pPeers = m_pDFile->getItem( strInfoHash );

			if( pPeers && pPeers->isDicti( ) )
			{
				CAtomDicti *pHuh = new CAtomDicti( );

				map<string, CAtom *> mapPeersDicti = ( (CAtomDicti *)pPeers )->getValue( );

				unsigned long iSeeders = 0;
				unsigned long iLeechers = 0;

				for( map<string, CAtom *> :: iterator j = mapPeersDicti.begin( ); j != mapPeersDicti.end( ); j++ )
				{
					if( (*j).second->isDicti( ) )
					{
						CAtom *pLeft = ( (CAtomDicti *)(*j).second )->getItem( "left" );

						if( pLeft && pLeft->isLong( ) )
						{
							if( ( (CAtomLong *)pLeft )->getValue( ) == 0 )
								iSeeders++;
							else
								iLeechers++;
						}
					}
				}

				pHuh->setItem( "complete", new CAtomInt( iSeeders ) );
				pHuh->setItem( "incomplete", new CAtomInt( iLeechers ) );

				if( m_pAllowed )
				{
					CAtom *pList = m_pAllowed->getItem( strInfoHash );

					if( pList && pList->isList( ) )
					{
						vector<CAtom *> vecTorrent = ( (CAtomList *)pList )->getValue( );

						if( vecTorrent.size( ) == 6 )
						{
							CAtom *pName = vecTorrent[1];

							if( pName )
								pHuh->setItem( "name", new CAtomString( pName->toString( ) ) );
						}
					}
				}

				if( m_pCompleted )
				{
					CAtom *pCompleted = m_pCompleted->getItem( strInfoHash );

					if( pCompleted && pCompleted->isLong( ) )
						pHuh->setItem( "downloaded", new CAtomLong( ( (CAtomLong *)pCompleted )->getValue( ) ) );
				}

				pFiles->setItem( strInfoHash, pHuh );
			}
		}
	}

	pResponse->strContent = Encode( pScrape );

	delete pScrape;
}
