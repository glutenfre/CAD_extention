#pragma once
//#include "cBaseEvent.h"
#include "DocumentFrameEvent.h"

class ApplicationEvent : public CBaseEvent
{
public:
	ApplicationEvent(IApplicationPtr);
	virtual ~ApplicationEvent();

protected:
	virtual   LPUNKNOWN GetUnknown();

	BEGIN_INTERFACE_PART(KompasObjectNotify, IKompasObjectNotify)
	INIT_INTERFACE_PART(ApplicationEvent, KompasObjectNotify)

	STDMETHOD_(VARIANT_BOOL, CreateDocument)(THIS_ long pDoc, int docType);
	STDMETHOD_(VARIANT_BOOL, BeginOpenDocument)(THIS_ LPSTR docName);
	STDMETHOD_(VARIANT_BOOL, OpenDocument)(THIS_ long pDoc, int docType);
	STDMETHOD_(VARIANT_BOOL, ChangeActiveDocument)(THIS_ long pDoc, int docType);
	STDMETHOD_(VARIANT_BOOL, ApplicationDestroy)(THIS_);
	STDMETHOD_(VARIANT_BOOL, BeginCreate)(THIS_ long type);
	STDMETHOD_(VARIANT_BOOL, BeginOpenFile)(THIS_);
	STDMETHOD_(VARIANT_BOOL, BeginCloseAllDocument)(THIS_);
	STDMETHOD_(VARIANT_BOOL, KeyDown)(THIS_ long* key, long flags, VARIANT_BOOL  sysKey);
	STDMETHOD_(VARIANT_BOOL, KeyUp)(THIS_ long* key, long flags, VARIANT_BOOL  sysKey);
	STDMETHOD_(VARIANT_BOOL, KeyPress)(THIS_ long* key, VARIANT_BOOL  sysKey);
	STDMETHOD_(VARIANT_BOOL, IsNotifyProcess)(THIS_ int notifyType);
	STDMETHOD_(VARIANT_BOOL, BeginChoiceMaterial) (THIS_ long MaterialPropertyId);
	STDMETHOD_(VARIANT_BOOL, ChoiceMaterial)(long MaterialPropertyId, LPSTR material, double density);
	STDMETHOD_(VARIANT_BOOL, BeginRequestFiles)(THIS_ long requestID, VARIANT* files);
	STDMETHOD_(VARIANT_BOOL, IsNeedConvertToSavePrevious) (long pDoc, int docType, long saveVersion, IUnknown* saveToPreviusParam, VARIANT_BOOL* needConvert);
	STDMETHOD_(VARIANT_BOOL, BeginConvertToSavePrevious) (long pDoc, int docType, long saveVersion, IUnknown* saveToPreviusParam);
	STDMETHOD_(VARIANT_BOOL, EndConvertToSavePrevious) (long pDoc, int docType, long saveVersion, IUnknown* saveToPreviusParam);
	STDMETHOD_(VARIANT_BOOL, ChangeTheme) (long newTheme);
	STDMETHOD_(VARIANT_BOOL, BeginDragOpenFiles) (VARIANT*, VARIANT_BOOL, VARIANT_BOOL*);

	IApplicationPtr m_app;

	END_INTERFACE_PART(KompasObjectNotify)
};

CBaseEvent* NewApplicationEvent(IApplicationPtr);
