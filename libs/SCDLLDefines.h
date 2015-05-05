#ifndef _SCLIB_DLLDEFINES_H_
#define _SCLIB_DLLDEFINES_H_

/* Cmake will define SCLIB_EXPORTS on Windows when it
 * configures to build a shared library. If you are going to use
 * another build system on windows or create the visual studio
 * projects by hand you need to define SCLIB_EXPORTS when
 * building a DLL on windows.
 * */

#if defined (_WIN32) 
  #if defined(SCLib_EXPORTS)
    #define SCLIB_EXPORT __declspec(dllexport)
  #else
    #define SCLIB_EXPORT __declspec(dllimport)
  #endif
#else
  #define SCLIB_EXPORT
#endif

#endif // _SCLIB_DLLDEFINES_H_
