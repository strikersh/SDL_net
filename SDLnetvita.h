/*
  SDL_net:  An example cross-platform network library for use with SDL
  Copyright (C) 1997-2017 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/* $Id$ */

#pragma once

#include <sys/select.h>
#include <vitasdk.h>

// These are all defined in SDLnetvita.c

int SDLNet_Vita_InitNet(void);
void SDLNet_Vita_QuitNet(void);

// These are missing from our newlib, so replacements are provided in
// SDLnetvita.c.
// REMOVE these after corresponding functions get added to Vita newlib.

char *_vita_inet_ntoa(struct in_addr in);
in_addr_t _vita_inet_addr(const char *cp);
struct hostent *_vita_gethostbyaddr(const void *addr, socklen_t len, int type);

#undef inet_ntoa
#undef inet_addr
#undef gethostbyaddr
#define inet_ntoa _vita_inet_ntoa
#define inet_addr _vita_inet_addr
#define gethostbyaddr _vita_gethostbyaddr

// SDL2_net looks for these and uses them exactly as rand() and srand()
// for some reason, and we don't have them in libc.

#undef random
#undef srandom
#define random rand
#define srandom srand

// Some potentially missing constants.

#ifndef SOL_SOCKET
#define SOL_SOCKET SCE_NET_SOL_SOCKET
#endif

#ifndef IPPROTO_IP
#define IPPROTO_IP SCE_NET_IPPROTO_IP
#endif

#ifndef SO_BROADCAST
#define SO_BROADCAST SCE_NET_SO_BROADCAST
#endif

#ifndef IP_ADD_MEMBERSHIP
// HACK
#undef ip_mreq
#define ip_mreq SceNetIpMreq
#define IP_ADD_MEMBERSHIP SCE_NET_IP_ADD_MEMBERSHIP
#endif

#ifndef TCP_NODELAY
#define TCP_NODELAY SCE_NET_TCP_NODELAY
#endif
