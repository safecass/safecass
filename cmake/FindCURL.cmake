#
# Safety Framework for Component-based Robotics
#
# Copyright (C) 2012 Min Yang Jung, Peter Kazanzides
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

#
# FindCURL.cmake
#
# Looks for CURL library for client-side multi-protocol data transfers and defines
# following cmake variables:
#
#   CURL_FOUND
#   CURL_INCLUDE_DIR
#   CURL_LIBRARIES
#
# About CURL [2]:
#   - Supported protocols: DICT, FILE, FTP, FTPS, Gopher, HTTP, HTTPS, IMAP, IMAPS, 
#     LDAP, LDAPS, POP3, POP3S, RTMP, RTSP, SCP, SFTP, SMTP, SMTPS, Telnet and TFTP.
#   - Supported features: SSL certificates, HTTP POST, HTTP PUT, FTP uploading, HTTP 
#     form based upload, proxies, cookies, user+password authentication (Basic, Digest, 
#     NTLM, Negotiate, Kerberos), file transfer resume, http proxy tunneling
#   - Supported platforms: Solaris, NetBSD, FreeBSD, OpenBSD, Darwin, HPUX, IRIX, AIX,
#     Tru64, Linux, UnixWare, HURD, Windows, Amiga, OS/2, BeOs, Mac OS X, Ultrix, QNX, 
#     OpenVMS, RISC OS, Novell NetWare, DOS and more...
#
# References:
#   [1] cURL library main: http://curl.haxx.se 
#   [2] libcurl (the multiprotocol file transfer library): http://curl.haxx.se/libcurl
#

find_path(CURL_INCLUDE_DIR NAMES curl/curl.h DOC "Directory containing curl/curl.h")
find_library(CURL_LIBRARY NAMES curl DOC "Path to curl library file (libcurl)")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CURL DEFAULT_MSG CURL_INCLUDE_DIR CURL_LIBRARY)

if(CURL_FOUND)
  set(CURL_LIBRARIES ${CURL_LIBRARY})
  set(CURL_INCLUDE_DIR ${CURL_INCLUDE_DIR})
endif(CURL_FOUND)
