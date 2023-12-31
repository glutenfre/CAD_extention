#include "StdAfx.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <GL\gl.h>
#include "volume_extension.h"
#include "..\Events\COM\DocumentFrameEvent.h"
#include "utilits.h"
#include "..\Events\COM\ApplicationEvent.h"

IApplicationPtr newKompasAPI(NULL);
long original_correction = 0;
long correction = 0;

void GetNewKompasAPI();

/************ DLL-MAINTENANCE ************/

static AFX_EXTENSION_MODULE volume_extensionDLL = { NULL, NULL };
HINSTANCE g_hInstance = NULL;

void OnProcessAttach()
{
	GetNewKompasAPI();
}

void OnProcessDetach()
{
	newKompasAPI = NULL;
}

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	g_hInstance = hInstance;

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("volume_extension.AWX Initializing!\n");

		AfxInitExtensionModule(volume_extensionDLL, hInstance);

		new CDynLinkLibrary(volume_extensionDLL);

		OnProcessAttach();
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("volume_extension.AWX Terminating!\n");

		OnProcessDetach();

		AfxTermExtensionModule(volume_extensionDLL);
	}
	return 1;
}

/************ DLL-MAINTENANCE ************/

int WINAPI LibInterfaceNotifyEntry(IDispatch* application)
{
	GetNewKompasAPI();
	NewApplicationEvent(newKompasAPI);
	return 0;
}

CString LoadStr(int strID)
{
	TCHAR temp[_MAX_PATH];
	LoadString(volume_extensionDLL.hModule, strID, temp, _MAX_PATH);
	return temp;
}

/*********** KOMPAS DECORATION ***********/

void CalculateVolume() {
	double volume_res = 0;
	double volume_accurate = 0;
	if (newKompasAPI) {
		IKompasDocument3DPtr doc3d = newKompasAPI->GetActiveDocument();
		if (doc3d) {
			IEmbodimentsManagerPtr embManager(doc3d);
			if (embManager) {
				IEmbodimentPtr curEmb = embManager->GetCurrentEmbodiment();
				if (curEmb) {
					// Here im going to find the model size
					IPart7Ptr rootPart = curEmb->GetPart();
					if (rootPart) {
						points_to_draw.SetPart(rootPart);

						// Finding gabarits of the model
						Parallelepiped res_parallepiped(100000);
						bool to_update_paral = true;
						TraversePart(rootPart,
							to_update_paral,
							&res_parallepiped,
							NULL,
							NULL,
							NULL,
							NULL);

						// Generating points
						int x_size = res_parallepiped.second_point.x - res_parallepiped.first_point.x;
						int y_size = res_parallepiped.second_point.y - res_parallepiped.first_point.y;
						int z_size = res_parallepiped.second_point.z - res_parallepiped.first_point.z;

						points_to_draw.RandomizePoints(
							correction,
							res_parallepiped.first_point.x,
							res_parallepiped.first_point.y,
							res_parallepiped.first_point.z,
							x_size, y_size, z_size);

						IDocumentFramesPtr doc_frames = doc3d->GetDocumentFrames();
						if (doc_frames) {
							points_to_draw.SetDocFrames(doc_frames);

							// Searching for the points inside the model
							bool to_check_points = false;
							TraversePart(rootPart,
								to_check_points,
								&res_parallepiped,
								&points_to_draw.x_points_vector,
								&points_to_draw.y_points_vector,
								&points_to_draw.z_points_vector,
								&points_to_draw.points_inside_model);

							// The volume
							int volume_points_num = 0;
							for (int i = 0; i < correction; i++) {
								if (points_to_draw.points_inside_model[i] % 2 == 1) volume_points_num++;
							}
							double c = 1000;
							volume_res = ((volume_points_num / (double)correction) * x_size * y_size * z_size) / c;

							IMassInertiaParam7Ptr massinertparam(rootPart);
							volume_accurate = massinertparam->Volume;
								
						}
					}
				}
			}
		}
	}
	std::string mes = "For the correction: " + std::to_string(correction) + 
		"\nVolume of model is: " + std::to_string(volume_res) + 
		" sm3\n( accurate volume is " + std::to_string(volume_accurate) + 
		" sm3 )";
	Message(const_cast<char*>(mes.c_str()));
}

unsigned int WINAPI LIBRARYID()
{
	return IDR_LIBID;
}

void WINAPI LIBRARYENTRY(unsigned int comm)
{
	switch (comm)
	{
	case 1:
	{
		points_to_draw.Reset();
		// asking for the ACCURANCY number
		if (correction == 0) ReadLong("Insert an accurancy in the range of 1 - 10000 :", 100, 1, 10000, &correction);
		if (original_correction == 0) original_correction = correction;
		CalculateVolume();
		break;
	}
	case 2:
	{
		points_to_draw.StartDrawing();
		VisualizeVolume(1000, 1000);
		break;
	}
	case 3:
	{
		points_to_draw.EndDrawing();
		TurnOffVisual();
		break;
	}
	case 4:
	{
		points_to_draw.EndDrawing();
		TurnOffVisual();
		int result = ReadLong("Insert new accurancy in the range of 1 - 10000 :", 100, 1, 10000, &correction);
		break;
	}
	case 5:
	{
		points_to_draw.EndDrawing();
		TurnOffVisual();
		correction = original_correction;
		break;
	}
	case 6:
	{
		points_to_draw.EndDrawing();
		TurnOffVisual();
		original_correction = 0;
		correction = 0;
		points_to_draw.Reset();
		break;
	}
	}
}

#define ADDINS_PATH "Software\\ASCON\\KOMPAS-3D\\AddIns\\volume_extension"

void GetNewKompasAPI() {
	if (!newKompasAPI) {
		CString filename;
		if (::GetModuleFileName(NULL, filename.GetBuffer(255), 255))
		{
			filename.ReleaseBuffer(255);
			CString libname;

			libname = "kAPI7.dll";
			filename.Replace(filename.Right(filename.GetLength() - (filename.ReverseFind('\\') + 1)),
				libname);

			HINSTANCE hAppAuto = LoadLibrary(filename);

			if (hAppAuto)
			{
				typedef LPDISPATCH(WINAPI* FCreateKompasApplication)();

				FCreateKompasApplication pCreateKompasApplication =
					(FCreateKompasApplication)GetProcAddress(hAppAuto, "CreateKompasApplication");

				if (pCreateKompasApplication)
					newKompasAPI = IDispatchPtr(pCreateKompasApplication(), false);
				FreeLibrary(hAppAuto);
			}
		}
	}
}

STDAPI DllRegisterServer()
{
	HRESULT hr = NOERROR;
	TCHAR szModulePath[MAX_PATH];

	// Obtain the path to this module's executable file for later use.
	GetModuleFileName(volume_extensionDLL.hModule, szModulePath,
		sizeof(szModulePath) / sizeof(TCHAR));

	CString strPath(ADDINS_PATH);
	HKEY hKey;
	DWORD dwDisposition;
	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE,
		strPath, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
		&hKey, &dwDisposition) != ERROR_SUCCESS)
	{
		hr = E_UNEXPECTED;
	}
	else
	{
		hr = RegSetValueEx(hKey, _T("Path"), 0L, REG_SZ,
			(CONST BYTE*)szModulePath, _tcslen(szModulePath));
		DWORD dwVal = 1;
		hr = RegSetValueEx(hKey, _T("AutoConnect"), 0L, REG_DWORD,
			(CONST BYTE*) & dwVal, sizeof(DWORD));
		hr = RegCloseKey(hKey);
	}
	return hr;
}

STDAPI DllUnregisterServer()
{
	HRESULT hr = NOERROR;
	if (RegDeleteKey(HKEY_LOCAL_MACHINE, (LPCTSTR)ADDINS_PATH) != ERROR_SUCCESS)
	{
		hr = E_UNEXPECTED;
	}
	return hr;
}

/*********** KOMPAS DECORATION ***********/