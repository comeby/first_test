//
// Copyright (C) 2003 Trevor Hogan
//

#ifndef SERVER_H
 #define SERVER_H

class CServer
{
public:
	CServer( );
	virtual ~CServer( );

	void Kill( );
	bool isDying( );

	// returns true if the server should be killed

	bool Update( bool bBlock );

	CTracker *getTracker( );

	vector<CClient *> m_vecClients;

private:
	bool m_bKill;

	CTracker *m_pTracker;

	int m_iSocketTimeOut;
	string m_strBind;
	int m_iCompression;

	SOCKET m_sckServer;
};

#endif
