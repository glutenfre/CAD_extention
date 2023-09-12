#pragma once
#include "CBaseEvent.h"
#include "..\..\volume_extension\utilits.h"
#include <vector>
#include <GL\gl.h>

extern PointsToDraw points_to_draw;

class DocumentFrameEvent : public CBaseEvent
{
public:
	DocumentFrameEvent(LPUNKNOWN I, reference doc);
	virtual ~DocumentFrameEvent();

public:
	virtual   LPUNKNOWN GetUnknown();

	BEGIN_INTERFACE_PART(DocumentFrameNotify, IDocumentFrameNotify)
	INIT_INTERFACE_PART(DocumentFrameEvent, DocumentFrameNotify)

	STDMETHOD_(VARIANT_BOOL, IsNotifyProcess)(int notifyType);
	virtual VARIANT_BOOL STDMETHODCALLTYPE BeginPaint(IUnknown __RPC_FAR* paintObj);
	virtual VARIANT_BOOL STDMETHODCALLTYPE ClosePaint(IUnknown __RPC_FAR* paintObj);
	virtual VARIANT_BOOL STDMETHODCALLTYPE MouseDown(short nButton, short nShiftState, long x, long y);
	virtual VARIANT_BOOL STDMETHODCALLTYPE MouseUp(short nButton, short nShiftState, long x, long y);
	virtual VARIANT_BOOL STDMETHODCALLTYPE MouseDblClick(short nButton, short nShiftState, long x, long y);
	virtual VARIANT_BOOL STDMETHODCALLTYPE MouseMove(short nShiftState, long x, long y);
	virtual VARIANT_BOOL STDMETHODCALLTYPE BeginPaintGL(long drawMode);
	virtual VARIANT_BOOL STDMETHODCALLTYPE ClosePaintGL(long drawMode);
	virtual VARIANT_BOOL STDMETHODCALLTYPE AddGabarit(IUnknown __RPC_FAR* gabObj);
	virtual VARIANT_BOOL STDMETHODCALLTYPE Activate(void);
	virtual VARIANT_BOOL STDMETHODCALLTYPE Deactivate(void);
	virtual VARIANT_BOOL STDMETHODCALLTYPE CloseFrame(void);
	virtual VARIANT_BOOL STDMETHODCALLTYPE ShowOcxTree(IUnknown* tree, VARIANT_BOOL show);

	virtual VARIANT_BOOL STDMETHODCALLTYPE BeginPaintTmpObjects();
	virtual VARIANT_BOOL STDMETHODCALLTYPE ClosePaintTmpObjects();

	END_INTERFACE_PART(DocumentFrameNotify)

};

CBaseEvent* NewDocumentFrameEvent(IApplicationPtr app, int redrawType = 0);
void VisualizeVolume(long Right, long Bottom);
void TurnOffVisual();