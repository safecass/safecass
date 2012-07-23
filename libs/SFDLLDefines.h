#ifndef _SFLIB_DLLDEFINES_H_
#define _SFLIB_DLLDEFINES_H_

/* Cmake will define SFLIB_EXPORTS on Windows when it
 * configures to build a shared library. If you are going to use
 * another build system on windows or create the visual studio
 * projects by hand you need to define SFLIB_EXPORTS when
 * building a DLL on windows.
 * */

#if defined (_WIN32) 
  #if defined(SFLIB_EXPORTS)
    #define SFLIB_EXPORT __declspec(dllexport)
  #else
    #define SFLIB_EXPORT __declspec(dllimport)
  #endif
#else
  #define SFLIB_EXPORT
#endif

#endif // _SFLIB_DLLDEFINES_H_
