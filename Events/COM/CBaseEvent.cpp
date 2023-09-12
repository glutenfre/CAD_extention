#include "stdafx.h"
#include <afxpriv.h>

#include "CBaseEvent.h"


CObList& CBaseEvent::m_EventList = *new CObList();


CBaseEvent::CBaseEvent(int ifType, reference p, long objType, reference doc)
	: CCmdTarget(),
	m_dwCookie(0),
	refDoc(doc)
{
	::memset(&m_params, 0, sizeof(NotifyConnectionParam));
	m_params.pContainer = p;
	m_params.ifType = ifType;
	m_params.objType = objType;
	m_EventList.AddTail(this);
}


CBaseEvent::CBaseEvent(int ifType, LPUNKNOWN iContainer, long objType, LPUNKNOWN iObj, reference doc)
	: CCmdTarget(),
	m_dwCookie(0),
	refDoc(doc)
{
	::memset(&m_params, 0, sizeof(NotifyConnectionParam));
	m_params.ifType = ifType;
	m_params.objType = objType;

	m_params.iContainer = iContainer;
	if (m_params.iContainer)
		m_params.iContainer->AddRef();

	m_params.iObj = iObj;
	if (m_params.iObj)
		m_params.iObj->AddRef();

	m_EventList.AddTail(this);
}


CBaseEvent::~CBaseEvent()
{
	RemoveThis();  // Удалим себя из списка обработчиков событий
}


bool CBaseEvent::Advise()
{
	ASSERT(!m_dwCookie);
	if (!m_dwCookie && m_params.ifType) {
		if (ReturnResult()) {
#ifdef _DEBUG
			MessageBoxResult();
#endif
			ResultNULL();
		}
		LPUNKNOWN obj = GetUnknown();
		m_dwCookie = ::ksConnectionAdvise(&m_params, obj);
		obj->Release();
		ASSERT(m_dwCookie);
	}
	return !!m_dwCookie;
}


void CBaseEvent::Unadvise()
{
	if (m_dwCookie)
	{
		VERIFY(::ksConnectionUnadvise(&m_params));
		m_dwCookie = 0;
	}
}


void CBaseEvent::TerminateEvents(long eventType, reference doc, int objType, LPUNKNOWN iObj)
{
	for (INT_PTR i = m_EventList.GetCount() - 1; i >= 0; i--) {
		CBaseEvent* evnt = (CBaseEvent*)m_EventList.GetAt(m_EventList.FindIndex(i));
		if (evnt && (!eventType || eventType == evnt->GetType())
			&& (!doc || doc == evnt->refDoc)
			&& (objType == -1 || objType == evnt->m_params.objType)
			&& (!iObj || iObj == evnt->m_params.iObj)) {

			evnt->Disconnect();
		}
	}
}


CBaseEvent* CBaseEvent::FindEvents(long eventType, reference doc, int objType, LPUNKNOWN iObj)
{
	CBaseEvent* res = NULL;
	for (INT_PTR i = m_EventList.GetCount() - 1; i >= 0; i--) {
		CBaseEvent* evnt = (CBaseEvent*)m_EventList.GetAt(m_EventList.FindIndex(i));
		if ((!eventType || eventType == evnt->GetType())
			&& ((!doc && !evnt->refDoc) || (doc == evnt->refDoc))
			&& ((objType == -1 && !evnt->m_params.objType) || (objType == evnt->m_params.objType))
			&& ((!iObj && !evnt->m_params.iObj) || (iObj == evnt->m_params.iObj))) {
			res = evnt;
			break;
		}
	}
	return res;
}


void CBaseEvent::TerminateEvents(void)
{
	while (!m_EventList.IsEmpty()) {
		CBaseEvent* headEvent = (CBaseEvent*)m_EventList.RemoveHead();
		headEvent->Disconnect();
	}
}


void CBaseEvent::DestroyList() {
	if (&m_EventList) {
		delete& m_EventList;
	}
}


void CBaseEvent::RemoveThis() {
	// Удалим себя из списка обработчиков событий
	POSITION pos = m_EventList.Find(this);
	if (pos) {
		m_EventList.RemoveAt(pos);
		Unadvise();                           // Отпишемся от получения событий
	}
}


void CBaseEvent::Clear()
{
	if (m_params.iContainer) {
		m_params.iContainer->Release();
		m_params.iContainer = NULL;
	}

	if (m_params.iObj) {
		m_params.iObj->Release();
		m_params.iObj = NULL;
	}
	::memset(&m_params, 0, sizeof(NotifyConnectionParam));
}


void CBaseEvent::Disconnect()
{
	Unadvise();
	Clear();
	ExternalRelease();
}
