/*
Copyright (C) 1996-2001 Id Software, Inc.
Copyright (C) 2002-2009 John Fitzgibbons and others

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// net_wipx.c

#include "../quakedef.h"

#include <wsipx.h>

#include "net_wipx.h"

//=============================================================================

int WIPX_Init(void)
{
    return -1;
}

//=============================================================================

void WIPX_Shutdown(void)
{
}

//=============================================================================

void WIPX_Listen(bool state)
{
}

//=============================================================================

int WIPX_OpenSocket(int port)
{
    return -1;
}

//=============================================================================

int WIPX_CloseSocket(int handle)
{
    return -1;
}

//=============================================================================

int WIPX_Connect(int handle, struct qsockaddr* addr)
{
    return 0;
}

//=============================================================================

int WIPX_CheckNewConnections(void)
{
    return -1;
}

//=============================================================================

static uint8_t packetBuffer[NET_DATAGRAMSIZE + 4];

int WIPX_Read(int handle, uint8_t* buf, int len, struct qsockaddr* addr)
{
    return 0;
}

//=============================================================================

int WIPX_Broadcast(int handle, uint8_t* buf, int len)
{
    return 0;
}

//=============================================================================

int WIPX_Write(int handle, uint8_t* buf, int len, struct qsockaddr* addr)
{
    return -1;
}

//=============================================================================

char* WIPX_AddrToString(struct qsockaddr* addr)
{
    return "";
}

//=============================================================================

int WIPX_StringToAddr(char* string, struct qsockaddr* addr)
{
    return 0;
}

//=============================================================================

int WIPX_GetSocketAddr(int handle, struct qsockaddr* addr)
{
    return 0;
}

//=============================================================================

int WIPX_GetNameFromAddr(struct qsockaddr* addr, char* name)
{
    return 0;
}

//=============================================================================

int WIPX_GetAddrFromName(char* name, struct qsockaddr* addr)
{
    return -1;
}

//=============================================================================

int WIPX_AddrCompare(struct qsockaddr* addr1, struct qsockaddr* addr2)
{
    return 0;
}

//=============================================================================

int WIPX_GetSocketPort(struct qsockaddr* addr)
{
    return 0;
}

int WIPX_SetSocketPort(struct qsockaddr* addr, int port)
{
    return 0;
}

//=============================================================================
