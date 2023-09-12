#include "StdAfx.h"
#include "DocumentFrameEvent.h"


#pragma comment(lib, "opengl32.lib")

PointsToDraw points_to_draw;

CBaseEvent* NewDocumentFrameEvent(IApplicationPtr app, int)
{
	CBaseEvent* res = NULL;

	IKompasDocumentPtr docPtr = app->GetActiveDocument();
	if (docPtr) {
		reference doc = docPtr->GetReference();
		IDocumentFramesPtr frames = docPtr->GetDocumentFrames();
		if (doc && frames) {
			for (int i = 0; i < frames->GetCount(); ++i) {
				IDocumentFramePtr documentFramePtr = frames->GetItem(i);
				if (documentFramePtr->GetActive()) {

					if (!CBaseEvent::FindEvents(ntDocumentFrameNotify, doc, -1, (LPUNKNOWN) documentFramePtr)) {
						res = new DocumentFrameEvent((LPUNKNOWN) documentFramePtr, doc);
						if (!res->Advise()) {
							delete res;
							res = NULL;
						}
					}
					break;

				}
			}
		}
	}

	return res;
}

void VisualizeVolume(long Right, long Bottom) {
	if (points_to_draw.DrawMode()) {
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0.0, Right, 0.0, Bottom, -1.0, 1.0);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glDisable(GL_LIGHTING);

		for (int i = 0; i < points_to_draw.x_points_vector.size(); i++) {
			long x = 0;
			long y = 0;
			points_to_draw.UnConvertCoordinates(i, &x, &y);
			glPointSize(2);
			glBegin(GL_POINTS);
			if (points_to_draw.points_inside_model[i] % 2 == 1)
				glColor3d(1.0, 0, 0.0);
			else glColor3d(0, 0, 1.0);
			glVertex2d(x , y);
			glEnd();
		}

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		glEnable(GL_LIGHTING);
		glPopMatrix();
	}
}

void TurnOffVisual() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clean the screen and the depth buffer
	glLoadIdentity(); // Reset The Projection Matrix
}

DocumentFrameEvent::DocumentFrameEvent(LPUNKNOWN I, reference doc) :
	CBaseEvent(ntDocumentFrameNotify, NULL, 0, I, doc)
{
	m_params.pContainer = doc;
}


DocumentFrameEvent::~DocumentFrameEvent()
{
}


LPUNKNOWN DocumentFrameEvent::GetUnknown() {
	m_xDocumentFrameNotify.AddRef();
	return &m_xDocumentFrameNotify;
}


STDMETHODIMP_(ULONG) DocumentFrameEvent::XDocumentFrameNotify::Release()
{
	METHOD_PROLOGUE_EX_(DocumentFrameEvent, DocumentFrameNotify)
	return (ULONG)pThis->InternalRelease();
}


STDMETHODIMP_(ULONG) DocumentFrameEvent::XDocumentFrameNotify::AddRef()
{
	METHOD_PROLOGUE_EX_(DocumentFrameEvent, DocumentFrameNotify)
	return (ULONG)pThis->InternalAddRef();
}


STDMETHODIMP DocumentFrameEvent::XDocumentFrameNotify::QueryInterface(REFIID iid, LPVOID* ppvObj)
{
	METHOD_PROLOGUE_EX_(DocumentFrameEvent, DocumentFrameNotify)

	ASSERT(AfxIsValidAddress(ppvObj, sizeof(LPVOID), FALSE));

	if (IsEqualIID(iid, IID_IUnknown) ||
		IsEqualIID(iid, IID_IDocumentFrameNotify))
	{
		*ppvObj = this;
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}


STDMETHODIMP_(VARIANT_BOOL) DocumentFrameEvent::XDocumentFrameNotify::IsNotifyProcess(int notifyType)
{
	return  notifyType >= frBeginPaint && notifyType <= frCloseFrame;
}


// isn't used in 3D
VARIANT_BOOL STDMETHODCALLTYPE DocumentFrameEvent::XDocumentFrameNotify::BeginPaint(IUnknown __RPC_FAR* paintObj) {
	return true;
}


// isn't used in 3D
VARIANT_BOOL STDMETHODCALLTYPE DocumentFrameEvent::XDocumentFrameNotify::ClosePaint(IUnknown __RPC_FAR* paintObj) {

	return true;
}


VARIANT_BOOL STDMETHODCALLTYPE DocumentFrameEvent::XDocumentFrameNotify::MouseDown(short nButton, short nShiftState, long x, long y) {
	METHOD_PROLOGUE_EX_(DocumentFrameEvent, DocumentFrameNotify)
	return true;
}


VARIANT_BOOL STDMETHODCALLTYPE DocumentFrameEvent::XDocumentFrameNotify::MouseUp(short nButton, short nShiftState, long x, long y) {
	return true;
}


VARIANT_BOOL STDMETHODCALLTYPE DocumentFrameEvent::XDocumentFrameNotify::MouseDblClick(short nButton, short nShiftState, long x, long y) {
	METHOD_PROLOGUE(DocumentFrameEvent, DocumentFrameNotify)
	return true;
}


VARIANT_BOOL STDMETHODCALLTYPE DocumentFrameEvent::XDocumentFrameNotify::BeginPaintGL(long drawMode)
{
	METHOD_PROLOGUE_EX_(DocumentFrameEvent, DocumentFrameNotify)
	return true;
}


// keep document painted and draw on top
VARIANT_BOOL STDMETHODCALLTYPE DocumentFrameEvent::XDocumentFrameNotify::ClosePaintGL(long drawMode)
{
	METHOD_PROLOGUE(DocumentFrameEvent, DocumentFrameNotify)

	//points_to_draw.SetPlacement();
	long Left, Right, Bottom, Top;
	IDocumentFramePtr documentFrame(pThis->m_params.iObj);
	documentFrame->GetWorkAreaRect(&Left, &Top, &Right, &Bottom);

	long oX = (Left + Right) / 2;
	long oY = (Top + Bottom) / 2;
	VisualizeVolume(Right, Bottom);
	return true;
}



VARIANT_BOOL STDMETHODCALLTYPE DocumentFrameEvent::XDocumentFrameNotify::AddGabarit(IUnknown __RPC_FAR* gabObj)
{
	METHOD_PROLOGUE(DocumentFrameEvent, DocumentFrameNotify)
	return false;
}


VARIANT_BOOL STDMETHODCALLTYPE DocumentFrameEvent::XDocumentFrameNotify::Activate(void)
{
	return true;
}


VARIANT_BOOL STDMETHODCALLTYPE DocumentFrameEvent::XDocumentFrameNotify::Deactivate(void)
{
	return true;
}


VARIANT_BOOL STDMETHODCALLTYPE DocumentFrameEvent::XDocumentFrameNotify::CloseFrame(void)
{
	METHOD_PROLOGUE(DocumentFrameEvent, DocumentFrameNotify)
	pThis->Disconnect();
	return true;
}


VARIANT_BOOL STDMETHODCALLTYPE DocumentFrameEvent::XDocumentFrameNotify::MouseMove(short nShiftState, long x, long y)
{
	METHOD_PROLOGUE_EX_(DocumentFrameEvent, DocumentFrameNotify)
	return true;
}


VARIANT_BOOL STDMETHODCALLTYPE DocumentFrameEvent::XDocumentFrameNotify::ShowOcxTree(IUnknown* tree, VARIANT_BOOL show)
{
	METHOD_PROLOGUE_EX_(DocumentFrameEvent, DocumentFrameNotify)
		return true;
}


VARIANT_BOOL DocumentFrameEvent::XDocumentFrameNotify::BeginPaintTmpObjects() {
	METHOD_PROLOGUE_EX_(DocumentFrameEvent, DocumentFrameNotify)
	return true;
}


VARIANT_BOOL DocumentFrameEvent::XDocumentFrameNotify::ClosePaintTmpObjects() {
	METHOD_PROLOGUE_EX_(DocumentFrameEvent, DocumentFrameNotify)
	return true;
}
