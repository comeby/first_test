//
// Copyright (C) 2003 Trevor Hogan
//

// Trinity Edition Modified Source File

#ifndef TRACKER_H
 #define TRACKER_H

struct torrent_t
{
	string strInfoHash;
	string strName;
	string strLowerName;
	int iComplete;
	int iDL;
	string strFileName;
	string strAdded;
	int64 iSize;
	int iFiles;
	int iComments;
	int64 iAverageLeft;
	int iAverageLeftPercent;
	int64 iMinLeft;
	int64 iMaxiLeft;
	string strTag;
	int iCompleted;
	int64 iTransferred;
	string strUploader;
	string strInfoLink;
};

struct peer_t
{
	string strIP;
	int64 iUpped;
	int64 iDowned;
	int64 iLeft;
	unsigned long iConnected;
	float flShareRatio;
};

// user access levels

#define ACCESS_VIEW				( 1 << 0 )		// 1
#define ACCESS_DL				( 1 << 1 )		// 2
#define ACCESS_COMMENTS			( 1 << 2 )		// 4
#define ACCESS_UPLOAD			( 1 << 3 )		// 8
#define ACCESS_EDIT				( 1 << 4 )		// 16
#define ACCESS_ADMIN			( 1 << 5 )		// 32
#define ACCESS_SIGNUP			( 1 << 6 )		// 64

struct user_t
{
	string strLogin;
	string strLowerLogin;
	string strMD5;
	string strMail;
	string strLowerMail;
	string strCreated;
	int iAccess;
};

/*

#define ALW_FILENAME		0
#define ALW_NAME			1
#define ALW_ADDED			2
#define ALW_SIZE			3
#define ALW_FILES			4
#define ALW_FILECOMMENT		5

*/

#define MAX_FILENAME_LEN	128		// user specified filename on upload
#define MAX_INFOLINK_LEN	128		// user specified info link on upload

extern map<string, string> gmapMime;

class CTracker
{
public:
	CTracker( );
	virtual ~CTracker( );

	void saveDFile( );
	void saveComments( );
	void saveTags( );
	void saveUsers( );
	void saveXML( );
	void expireDownloaders( );
	void parseTorrents( const char *szDir );
	void parseTorrent( const char *szFile );
	bool checkTag( string &strTag );
	void addTag( string strInfoHash, string strTag, string strName, string strUploader, string strInfoLink );
	void deleteTag( string strInfoHash );
	user_t checkUser( string strLogin, string strMD5 );
	void addUser( string strLogin, string strPass, int iAccess, string strMail );
	void deleteUser( string strLogin );
	void CountUniquePeers( );
	void AddUniquePeer( string strIP );
	void RemoveUniquePeer( string strIP );
	void QueueAnnounce( CAtomDicti *pParams );

	void serverResponseGET( struct request_t *pRequest, struct response_t *pResponse, user_t user );
	void serverResponsePOST( struct request_t *pRequest, struct response_t *pResponse, CAtomList *pPost, user_t user );
	void serverResponseIndex( struct request_t *pRequest, struct response_t *pResponse, user_t user );
	void serverResponseAnnounce( struct request_t *pRequest, struct response_t *pResponse, user_t user );
	void serverResponseScrape( struct request_t *pRequest, struct response_t *pResponse, user_t user );
	void serverResponseStats( struct request_t *pRequest, struct response_t *pResponse, user_t user );
	void serverResponseComments( struct request_t *pRequest, struct response_t *pResponse, user_t user );
	void serverResponseAdmin( struct request_t *pRequest, struct response_t *pResponse, user_t user );
	void serverResponseFile( struct request_t *pRequest, struct response_t *pResponse, user_t user );
	void serverResponseTorrent( struct request_t *pRequest, struct response_t *pResponse, user_t user );
	void serverResponseUploadGET( struct request_t *pRequest, struct response_t *pResponse, user_t user );
	void serverResponseUploadPOST( struct request_t *pRequest, struct response_t *pResponse, CAtomList *pPost, user_t user );
	void serverResponseUsers( struct request_t *pRequest, struct response_t *pResponse, user_t user );
	void serverResponseLogin( struct request_t *pRequest, struct response_t *pResponse, user_t user );
	void serverResponseInfo( struct request_t *pRequest, struct response_t *pResponse, user_t user );
	void serverResponseSignup( struct request_t *pRequest, struct response_t *pResponse, user_t user );

	void Update( );

private:
	string m_strAllowedDir;
	string m_strUploadDir;
	string m_strExternalTorrentDir;
	string m_strArchiveDir;
	string m_strFileDir;
	string m_strDFile;
	string m_strCommentsFile;
	string m_strTagFile;
	string m_strUsersFile;
	string m_strStaticHeaderFile;
	string m_strStaticHeader;
	string m_strStaticFooterFile;
	string m_strStaticFooter;
	// The Trinity Edition - ADD BEGINS (TRINCUSTSTATDOX)
	string m_strStaticHeaderSignupFile;
	string m_strStaticHeaderSignup;
	string m_strStaticFooterSignupFile;
	string m_strStaticFooterSignup;

	string m_strStaticHeaderUploadFile;
	string m_strStaticHeaderUpload;
	string m_strStaticFooterUploadFile;
	string m_strStaticFooterUpload;
	// -------- END OF ADD
	string m_strDumpXMLFile;
	string m_strImageBarFill;
	string m_strImageBarTrans;
	string m_strForceAnnounceURL;
	bool m_bForceAnnounceOnDL;
	int m_iParseAllowedInterval;
	int m_iSaveDFileInterval;
	int m_iDownloaderTimeOutInterval;
	int m_iRefreshStaticInterval;
	int m_iDumpXMLInterval;
	unsigned long m_iParseAllowedNext;
	unsigned long m_iSaveDFileNext;
	unsigned long m_iPrevTime;
	unsigned long m_iDownloaderTimeOutNext;
	unsigned long m_iRefreshStaticNext;
	unsigned long m_iDumpXMLNext;
	int m_iAnnounceInterval;
	int m_iMinRequestInterval;
	int m_iResponseSize;
	int m_iMaxGive;
	bool m_bKeepDead;
	bool m_bAllowScrape;
	bool m_bCountUniquePeers;
	bool m_bDeleteInvalid;
	bool m_bParseOnUpload;
	int m_iMaxTorrents;
	bool m_bShowInfoHash;
	bool m_bShowNames;
	bool m_bShowStats;
	bool m_bAllowTorrentDownloads;
	bool m_bAllowComments;
	bool m_bShowAdded;
	bool m_bShowSize;
	bool m_bShowNumFiles;
	bool m_bShowCompleted;
	bool m_bShowTransferred;
	bool m_bShowMinLeft;
	bool m_bShowAverageLeft;
	bool m_bShowMaxiLeft;
	bool m_bShowLeftAsProgress;
	bool m_bShowUploader;
	bool m_bAllowInfoLink;
	bool m_bSearch;
	bool m_bSort;
	bool m_bShowFileComment;
	bool m_bShowFileContents;
	bool m_bShowShareRatios;
	bool m_bShowAvgDLRate;
	bool m_bShowAvgULRate;
	bool m_bDeleteOwnTorrents;
	bool m_bGen;
	int m_iPerPage;
	int m_iUsersPerPage;
	int m_iMaxPeersDisplay;
	int m_iGuestAccess;
	int m_iMemberAccess;
	int m_iFileExpires;
	int m_iNameLength;
	int m_iCommentLength;

	// The Trinity Edition - Addition Begins

	// Declares new variable for "trinity_use_mouseovers"
	bool m_bUseMouseovers;

	// Declares new variable for "bnbt_custom_labels"
	bool m_bUseCustomlabels;

	// Declares new variable for "bnbt_navigation_bar"
	bool m_bUseNavbar;

	// ------------------------------------------------- End of Addition


	vector< pair<string, string> > m_vecTags;

	// queued announces

	CMutex m_mtxQueued;

	vector<CAtomDicti *> m_vecQueued;

	CAtomDicti *m_pAllowed;		// self.allowed
	CAtomDicti *m_pState;		// self.state
	CAtomDicti *m_pDFile;		// self.downloads
	CAtomDicti *m_pCompleted;	// self.completed
	CAtomDicti *m_pTimeDicti;	// self.times
	CAtomDicti *m_pCached;		// self.cached
	CAtomDicti *m_pComments;
	CAtomDicti *m_pTags;
	CAtomDicti *m_pUsers;
	CAtomDicti *m_pIPs;
};

#endif
