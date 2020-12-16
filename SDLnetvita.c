/*
  SDL_net:  An example cross-platform network library for use with SDL
  Copyright (C) 1997-2017 Sam Lantinga <slouken@libsdl.org>
  Copyright (C) 2012 Simeon Maxein <smaxein@googlemail.com>

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

#ifdef __vita__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <psp2/sysmodule.h>

#include "SDL_net.h"
#include "SDLnetvita.h"

#define NET_INIT_SIZE (1 * 1024 * 1024)
#ifndef SCE_NET_CTL_ERROR_NOT_TERMINATED
#define SCE_NET_CTL_ERROR_NOT_TERMINATED 0x80412102
#endif

// Vita-related initialization stuff.

static void *net_memory = NULL;

// Call this in SDLNet_Init() before anything else.

int SDLNet_Vita_InitNet(void)
{
	SceNetInitParam initparam;
	int ret;

	ret = sceSysmoduleLoadModule(SCE_SYSMODULE_NET);
	if (ret) {
		SDLNet_SetError("Could not load SCE_SYSMODULE_NET: error %d\n", ret);
		return ret;
	}

	ret = sceNetShowNetstat();
	if (ret == SCE_NET_ERROR_ENOTINIT) {
		net_memory = malloc(NET_INIT_SIZE);
		if (!net_memory) {
			SDLNet_SetError("Could not allocate %d bytes for libnet buffer.", NET_INIT_SIZE);
			return SCE_NET_ENOMEM;
		}

		initparam.memory = net_memory;
		initparam.size = NET_INIT_SIZE;
		initparam.flags = 0;

		ret = sceNetInit(&initparam);
		if (ret) {
			SDLNet_SetError("sceNetInit(): error %d\n", ret);
			free(net_memory);
			return ret;
		}
	}

	ret = sceNetCtlInit();
	if (ret) {
		SDLNet_SetError("sceNetCtlInit(): error %d\n", ret);
		// SCE_NET_CTL_ERROR_NOT_TERMINATED just means it's already been inited
		if (ret != SCE_NET_CTL_ERROR_NOT_TERMINATED)
			return ret;
	}

	return 0;
}

// Call this in SDLNet_Quit() after anything else.

void SDLNet_Vita_QuitNet(void)
{
	sceNetCtlTerm();
	sceNetTerm();
	if (net_memory) {
		free(net_memory);
		net_memory = NULL;
	}
	sceSysmoduleUnloadModule(SCE_SYSMODULE_NET);
}

// Don't have these in our libc.

char *_vita_inet_ntoa(struct in_addr in)
{
	static char buf[32];
	SceNetInAddr addr;
	addr.s_addr = in.s_addr;
	sceNetInetNtop(SCE_NET_AF_INET, &addr, buf, sizeof(buf));
	return buf;
}

in_addr_t _vita_inet_addr(const char *cp)
{
	SceNetInAddr addr;
	int res;
	res = sceNetInetPton(SCE_NET_AF_INET, cp, &addr);
	if (res <= 0) return (in_addr_t)(-1); // is actually expected behavior
	return (in_addr_t)(addr.s_addr);
}

struct hostent *_vita_gethostbyaddr(const void *addr, socklen_t len, int type)
{
	static struct hostent ent;
	static char sname[256];
	static struct SceNetInAddr saddr;
	static char *addrlist[2];
	int rid, e;

	addrlist[0] = (char *)&saddr;
	addrlist[1] = NULL;

	if (type != AF_INET || len != sizeof(uint32_t)) return NULL;

	rid = sceNetResolverCreate("sdlnet_resolv", NULL, 0);
	if (rid < 0) return NULL;

	memcpy(&saddr.s_addr, addr, sizeof(uint32_t));

	e = sceNetResolverStartAton(rid, &saddr, sname, sizeof(sname), 0, 0, 0);
	sceNetResolverDestroy(rid);
	if (e < 0) return NULL;

	ent.h_name = sname;
	ent.h_aliases = 0;
	ent.h_addrtype = AF_INET;
	ent.h_length = sizeof(struct SceNetInAddr);
	ent.h_addr_list = addrlist;
	ent.h_addr = addrlist[0];

	return &ent;
}

#endif
