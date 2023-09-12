#pragma once
#ifndef __LDEFIN2D_H
#include <ldefin2d.h>
#endif


class CBaseEvent : public CCmdTarget
{
protected:
	static CObList&           m_EventList;  // ������ ������������ �������
	DWORD                     m_dwCookie;   // ������������� ����������
	NotifyConnectionParam     m_params;     // ��������� ��������
	reference                 refDoc;       // �������� ��������� ��� �������   

public:
	CBaseEvent(int ifType, reference p = 0, long objType = 0, reference doc = 0);
	CBaseEvent(int ifType, LPUNKNOWN iContainer = NULL, long objType = 0, LPUNKNOWN iObj = NULL, reference doc = 0);
	virtual  ~CBaseEvent();

	bool         Advise();                      // ����������� �� ��������� �������
	void         Unadvise();                    // ���������� �� ��������� �������
	void         RemoveThis();                  // ������� ���� �� ������   
	void         Disconnect();                  // �������������   
	virtual void Clear();                       // ���������� ������   
											    
	static void  TerminateEvents(void);         // �������� ��� �������
	static void  DestroyList();                 // ������� ������      
	int   GetType() { return m_params.ifType; } // ��� �������
	long GetDocReference() { return refDoc; }   // �������� ���������

	static void        TerminateEvents(long eventType, reference doc = 0, int objType = -1, LPUNKNOWN iObj = NULL);
	static CBaseEvent* FindEvents(long eventType, reference doc = 0, int objType = -1, LPUNKNOWN iObj = NULL);

	virtual LPUNKNOWN  GetUnknown() = 0;
};

