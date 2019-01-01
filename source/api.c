#include "api.h"

static const cvar_api_t api_cvar = {
    Cvar_RegisterVariable,
    Cvar_Set,
    Cvar_SetValue,
    Cvar_VariableValue,
    Cvar_VariableString,
    Cvar_CompleteVariable,
    Cvar_Command,
    Cvar_WriteVariables,
    Cvar_FindVar
};

static const cmd_api_t api_cmd = {
    Cmd_AddCommand,
    Cmd_Exists,
    Cmd_CompleteCommand,
    Cmd_Argc,
    Cmd_Argv,
    Cmd_Args,
    Cmd_CheckParm,
    Cmd_TokenizeString,
    Cmd_ExecuteString,
    Cmd_ForwardToServer
};

static const con_api_t api_con = {
    Con_CheckResize,
    Con_Init,
    Con_DrawConsole,
    Con_Print,
    Con_Printf,
    Con_Warning,
    Con_DPrintf,
    Con_DPrintf2,
    Con_SafePrintf,
    Con_Clear_f,
    Con_DrawNotify,
    Con_ClearNotify,
    Con_ToggleConsole_f,
    Con_NotifyBox
};

static const sys_api_t api_sys = {
    Sys_FileOpenRead,
    Sys_FileOpenWrite,
    Sys_FileClose,
    Sys_FileSeek,
    Sys_FileRead,
    Sys_FileWrite,
    Sys_FileTime,
    Sys_mkdir,
    Sys_Error,
    Sys_Printf,
    Sys_Quit,
    Sys_FloatTime,
    Sys_ConsoleInput,
    Sys_Sleep
};

static const com_api_t api_com = {
    COM_WriteFile,
    COM_OpenFile,
    COM_FOpenFile,
    COM_CloseFile,
    COM_LoadStackFile,
    COM_LoadTempFile,
    COM_LoadHunkFile,
    COM_LoadCacheFile,
//    COM_LoadFile
    NULL
};

static const str_api_t api_str = {
    Q_memset,
    Q_memcpy,
    Q_memcmp,
    Q_strcpy,
    Q_strncpy,
    Q_strlen,
    Q_strrchr,
    Q_strcat,
    Q_strcmp,
    Q_strncmp,
    Q_strcasecmp,
    Q_strncasecmp,
    Q_atoi,
    Q_atof
};

static const math_api_t api_math = {
    _DotProduct,
    _VectorSubtract,
    _VectorAdd,
    _VectorCopy
};

static const mem_api_t api_mem = {
    Cache_Flush,
    Cache_Check,
    Cache_Free,
    Cache_Alloc,
    Cache_Report,
};

static const quake_api_t api_quake = {
    &api_cvar,
    &api_cmd,
    &api_con,
    &api_sys,
    &api_com,
    NULL,
    &api_str,
    &api_math,
    &api_mem,
};

const quake_api_t* GetQuakeAPI()
{
    return &api_quake;
}
