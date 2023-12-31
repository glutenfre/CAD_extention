#pragma once

#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcmn.h>         // MFC support for Windows 95 Common Controls


#include <COMUTIL.H>
#include <comdef.h>

#pragma warning( disable : 4192 )
#pragma warning( disable : 4278 )

#import "ksconstants.tlb"   no_namespace named_guids

#import "ksConstants3D.tlb" no_namespace named_guids


#include <libtool.h>
 
#import "kAPI2D5COM.tlb"   no_namespace named_guids
 
#import "kAPI3D5COM.tlb"    no_namespace named_guids 

#include <LHead3d.h>
  
#import "kAPI7.tlb"         no_namespace named_guids


#pragma warning( once : 4192 ) 
#pragma warning( once : 4278 ) 

