#include "quakedef.h"

static int NetNull_Init(void)
{
    return 0;
}

static void NetNull_Listen(bool state) {}

static void NetNull_SearchForHosts(bool xmit) {}

static qsocket_t* NetNull_Connect(char* host)
{
    return NULL;
}

static qsocket_t* NetNull_CheckNewConnections(void)
{
    return NULL;
}

static int NetNull_QGetMessage(qsocket_t* sock)
{
    return 0;
}

static int NetNull_QSendMessage(qsocket_t* sock, sizebuf_t* data)
{
    return 0;
}

static int NetNull_SendUnreliableMessage(qsocket_t* sock, sizebuf_t* data)
{
    return 0;
}

static bool NetNull_CanSendMessage(qsocket_t* sock)
{
    return false;
}

static bool NetNull_CanSendUnreliableMessage(qsocket_t* sock)
{
    return false;
}

static void NetNull_Close(qsocket_t* sock) {}

static void NetNull_Shutdown(void) {}

static net_driver_t NetNullDriver = {
    "NetNullDriver",
    true,
    NetNull_Init,
    NetNull_Listen,
    NetNull_SearchForHosts,
    NetNull_Connect,
    NetNull_CheckNewConnections,
    NetNull_QGetMessage,
    NetNull_QSendMessage,
    NetNull_SendUnreliableMessage,
    NetNull_CanSendMessage,
    NetNull_CanSendUnreliableMessage,
    NetNull_Close,
    NetNull_Shutdown,
    0,
};

net_driver_t* getNetNullDriver()
{
    return &NetNullDriver;
}
