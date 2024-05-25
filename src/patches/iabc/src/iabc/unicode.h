#ifndef IABC_UNICODE_H
#define IABC_UNICODE_H
#include <wx/setup.h>

#ifdef WIN32
#if wxUSE_UNICODE
#define MS_LP_STRING  LPWSTR
#else
#define MS_LP_STRING  LPSTR
#endif
#endif

#if wxUSE_UNICODE
#define B2U(s) (wxString(s,wxConvUTF8))
#define U2B(s) (s.mb_str())
#else
#define B2U(s) ((wxString)s)
#define U2B(s) ((const char*)s)
#endif
#endif

