#include "StdAfx.h"

#include "ApplicationEvent.h"
//#include "DocumentFrameEvent.h"

CBaseEvent* NewApplicationEvent(IApplicationPtr app)
{
	CBaseEvent* res = CBaseEvent::FindEvents(ntKompasObjectNotify, 0);
	if (!res)
	{
		res = new ApplicationEvent(app);
		if (!res->Advise())
		{
			delete res;
			res = NULL;
		}
	}
	return res;
}


ApplicationEvent::ApplicationEvent(IApplicationPtr app)
	: CBaseEvent(ntKompasObjectNotify, (reference)0)
{
	m_xKompasObjectNotify.m_app = app;
}


ApplicationEvent::~ApplicationEvent()
{
}


LPUNKNOWN ApplicationEvent::GetUnknown()
{
	m_xKompasObjectNotify.AddRef();
	return &m_xKompasObjectNotify;
}


STDMETHODIMP_(ULONG) ApplicationEvent::XKompasObjectNotify::Release()
{
	METHOD_PROLOGUE_EX_(ApplicationEvent, KompasObjectNotify)
	return (ULONG)pThis->InternalRelease();
}


STDMETHODIMP_(ULONG) ApplicationEvent::XKompasObjectNotify::AddRef()
{
	METHOD_PROLOGUE_EX_(ApplicationEvent, KompasObjectNotify)
	return (ULONG)pThis->InternalAddRef();
}


STDMETHODIMP ApplicationEvent::XKompasObjectNotify::QueryInterface(
	REFIID iid, LPVOID* ppvObj)
{
	METHOD_PROLOGUE_EX_(ApplicationEvent, KompasObjectNotify)

	ASSERT(AfxIsValidAddress(ppvObj, sizeof(LPVOID), FALSE));

	if (IsEqualIID(iid, IID_IUnknown) ||
		IsEqualIID(iid, IID_IKompasObjectNotify))
	{
		*ppvObj = this;
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}


STDMETHODIMP_(VARIANT_BOOL) ApplicationEvent::XKompasObjectNotify::IsNotifyProcess(int notifyType)
{
	return  notifyType >= koCreateDocument && notifyType <= koKeyPress;
}


STDMETHODIMP_(VARIANT_BOOL) ApplicationEvent::XKompasObjectNotify::CreateDocument(long pDoc, int docType)
{
	METHOD_PROLOGUE_EX_(ApplicationEvent, KompasObjectNotify)
	NewDocumentFrameEvent(m_app);
	return VARIANT_TRUE;
}


STDMETHODIMP_(VARIANT_BOOL) ApplicationEvent::XKompasObjectNotify::BeginOpenDocument(LPSTR docName)
{
	METHOD_PROLOGUE_EX_(ApplicationEvent, KompasObjectNotify)
	return VARIANT_TRUE;
}


STDMETHODIMP_(VARIANT_BOOL) ApplicationEvent::XKompasObjectNotify::OpenDocument(long pDoc, int docType)
{
	METHOD_PROLOGUE_EX_(ApplicationEvent, KompasObjectNotify)
	NewDocumentFrameEvent(m_app);
	return VARIANT_TRUE;
}


STDMETHODIMP_(VARIANT_BOOL) ApplicationEvent::XKompasObjectNotify::ChangeActiveDocument(long _pDoc, int docType)
{
	METHOD_PROLOGUE_EX_(ApplicationEvent, KompasObjectNotify)
	return VARIANT_TRUE;
}


STDMETHODIMP_(VARIANT_BOOL) ApplicationEvent::XKompasObjectNotify::ApplicationDestroy()
{
	METHOD_PROLOGUE_EX_(ApplicationEvent, KompasObjectNotify)
	pThis->Disconnect();
	return VARIANT_TRUE;
}


STDMETHODIMP_(VARIANT_BOOL) ApplicationEvent::XKompasObjectNotify::BeginCreate(long type)
{
	METHOD_PROLOGUE_EX_(ApplicationEvent, KompasObjectNotify)
	return VARIANT_TRUE;
}


STDMETHODIMP_(VARIANT_BOOL) ApplicationEvent::XKompasObjectNotify::BeginOpenFile()
{
	METHOD_PROLOGUE_EX_(ApplicationEvent, KompasObjectNotify)
	return VARIANT_TRUE;
}


STDMETHODIMP_(VARIANT_BOOL) ApplicationEvent::XKompasObjectNotify::BeginCloseAllDocument()
{
	return VARIANT_TRUE;
}


STDMETHODIMP_(VARIANT_BOOL) ApplicationEvent::XKompasObjectNotify::KeyDown(long* key, long flags, VARIANT_BOOL  sysKey)
{
	return VARIANT_TRUE;
}


STDMETHODIMP_(VARIANT_BOOL) ApplicationEvent::XKompasObjectNotify::KeyUp(long* key, long flags, VARIANT_BOOL  sysKey)
{
	return VARIANT_TRUE;
}


STDMETHODIMP_(VARIANT_BOOL) ApplicationEvent::XKompasObjectNotify::KeyPress(long* key, VARIANT_BOOL  sysKey)
{
	METHOD_PROLOGUE_EX_(ApplicationEvent, KompasObjectNotify)
	return VARIANT_TRUE;
}


STDMETHODIMP_(VARIANT_BOOL) ApplicationEvent::XKompasObjectNotify::BeginRequestFiles(THIS_ long requestID, VARIANT* files)
{
	return VARIANT_TRUE;
}


STDMETHODIMP_(VARIANT_BOOL) ApplicationEvent::XKompasObjectNotify::BeginChoiceMaterial(THIS_ long MaterialPropertyId)
{
	return VARIANT_TRUE;
}


STDMETHODIMP_(VARIANT_BOOL) ApplicationEvent::XKompasObjectNotify::ChoiceMaterial(long MaterialPropertyId, LPSTR material, double density)
{
	return VARIANT_TRUE;
}

STDMETHODIMP_(VARIANT_BOOL) ApplicationEvent::XKompasObjectNotify::IsNeedConvertToSavePrevious(long pDoc, int docType, long saveVersion, IUnknown* saveToPreviusParam, VARIANT_BOOL* needConvert)
{
	return VARIANT_TRUE;
}


STDMETHODIMP_(VARIANT_BOOL) ApplicationEvent::XKompasObjectNotify::BeginConvertToSavePrevious(long pDoc, int docType, long saveVersion, IUnknown* saveToPreviusParam)
{
	return VARIANT_TRUE;
}


STDMETHODIMP_(VARIANT_BOOL) ApplicationEvent::XKompasObjectNotify::EndConvertToSavePrevious(long pDoc, int docType, long saveVersion, IUnknown* saveToPreviusParam)
{
	return VARIANT_TRUE;
}


STDMETHODIMP_(VARIANT_BOOL) ApplicationEvent::XKompasObjectNotify::ChangeTheme(long)
{
	return VARIANT_TRUE;
}


STDMETHODIMP_(VARIANT_BOOL) ApplicationEvent::XKompasObjectNotify::BeginDragOpenFiles(VARIANT*, VARIANT_BOOL, VARIANT_BOOL*)
{
	return VARIANT_TRUE;
}
