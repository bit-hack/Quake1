#pragma once
#include "quakedef.h"

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// Module -> Quake
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

// sound device interface
typedef struct sound_api_t
{
    void (*StartSound)(int entnum, int entchannel, sfx_t* sfx, vec3_t origin, float fvol, float attenuation);
    void (*StopSound)(int entnum, int entchannel);
    void (*StopAllSounds)(bool clear);
    void (*StaticSound)(sfx_t* sfx, vec3_t origin, float vol, float attenuation);
    void (*BeginPrecaching)(void);
    void (*TouchSound)(char* sample);
    sfx_t* (*PrecacheSound)(char* sample);
    void (*EndPrecaching)(void);
    void (*LocalSound)(char* s);
    void (*ClearBuffer)(void);
    void (*Init)(void);
    void (*Shutdown)(void);
    void (*Update)(vec3_t origin, vec3_t v_forward, vec3_t v_right, vec3_t v_up);
    void (*ExtraUpdate)(void);
    int (*SampleRate)(void);
} sound_api_t;

// cd audio interface
typedef struct cdaudio_api_t
{
    int (*Init)(void);
    void (*Play)(uint8_t track, bool looping);
    void (*Stop)(void);
    void (*Pause)(void);
    void (*Resume)(void);
    void (*Shutdown)(void);
    void (*Update)(void);
} cdaudio_api_t;

// progs API
typedef struct progs_api_t {
    void (*PR_Init)(void);
    void (*PR_ExecuteProgram)(func_t fnum);
    void (*PR_LoadProgs)(void);
} progs_api_t;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// Quake -> Module
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

// console variable api
typedef struct cvar_api_t
{
    void (*RegisterVariable)(cvar_t* variable, void* function);
    void (*Set)(char* var_name, char* value);
    void (*SetValue)(char* var_name, float value);
    float (*VariableValue)(char* var_name);
    char* (*VariableString)(char* var_name);
    char* (*CompleteVariable)(char* partial);
    bool (*Command)(void);
    void (*WriteVariables)(FILE* f);
    cvar_t* (*FindVar)(char* var_name);
} cvar_api_t;

// command api
typedef struct cmd_api_t
{
    void (*AddCommand)(char* cmd_name, xcommand_t function);
    bool (*Exists)(char* cmd_name);
    char* (*CompleteCommand)(char* partial);
    int (*Argc)(void);
    char* (*Argv)(int arg);
    char* (*Args)(void);
    int (*CheckParm)(char* parm);
    void (*TokenizeString)(char* text);
    void (*ExecuteString)(char* text, cmd_source_t src);
    void (*ForwardToServer)(void);
} cmd_api_t;

// console api
typedef struct con_api_t
{
    void (*CheckResize)(void);
    void (*Init)(void);
    void (*DrawConsole)(int lines, bool drawinput);
    void (*Print)(char* txt);
    void (*Printf)(char* fmt, ...);
    void (*Warning)(char* fmt, ...);
    void (*DPrintf)(char* fmt, ...);
    void (*DPrintf2)(char* fmt, ...);
    void (*SafePrintf)(char* fmt, ...);
    void (*Clear_f)(void);
    void (*DrawNotify)(void);
    void (*ClearNotify)(void);
    void (*ToggleConsole_f)(void);
    void (*NotifyBox)(char* text);
} con_api_t;

// system api
typedef struct sys_api_t
{
    int (*FileOpenRead)(char* path, int* hndl);
    int (*FileOpenWrite)(char* path);
    void (*FileClose)(int handle);
    void (*FileSeek)(int handle, int position);
    int (*FileRead)(int handle, void* dest, int count);
    int (*FileWrite)(int handle, void* data, int count);
    int (*FileTime)(char* path);
    void (*mkdir)(char* path);
    void (*Error)(char* error, ...);
    void (*Printf)(char* fmt, ...);
    void (*Quit)(void);
    double (*FloatTime)(void);
    char* (*ConsoleInput)(void);
    void (*Sleep)(void);
} sys_api_t;

// common api
typedef struct com_api_t
{
    void (*WriteFile)(char* filename, void* data, int len);
    int (*OpenFile)(char* filename, int* hndl);
    int (*FOpenFile)(char* filename, FILE** file);
    void (*CloseFile)(int h);
    uint8_t* (*LoadStackFile)(char* path, void* buffer, int bufsize);
    uint8_t* (*LoadTempFile)(char* path);
    uint8_t* (*LoadHunkFile)(char* path);
    void (*LoadCacheFile)(char* path, struct cache_user_s* cu);
    uint8_t* (*LoadFile)(char* path, int usehunk);
} com_api_t;

typedef struct ren_api_t
{
  void (*R_Init)(void);
  void (*R_InitTextures)(void);
  void (*R_InitEfrags)(void);
  void (*R_RenderView)(void); // must set r_refdef first
  void (*R_ViewChanged)(vrect_t* pvrect, int lineadj, float aspect);
  void (*R_InitSky)(struct texture_s* mt); // called at level load
  void (*R_CheckEfrags)(void); //johnfitz
  void (*R_AddEfrags)(entity_t* ent);
  void (*R_RemoveEfrags)(entity_t* ent);
  void (*R_NewMap)(void);
  void (*R_ParseParticleEffect)(void);
  void (*R_RunParticleEffect)(vec3_t org, vec3_t dir, int color, int count);
  void (*R_RocketTrail)(vec3_t start, vec3_t end, int type);
  void (*R_EntityParticles)(entity_t* ent);
  void (*R_BlobExplosion)(vec3_t org);
  void (*R_ParticleExplosion)(vec3_t org);
  void (*R_ParticleExplosion2)(vec3_t org, int colorStart, int colorLength);
  void (*R_LavaSplash)(vec3_t org);
  void (*R_TeleportSplash)(vec3_t org);
  void (*R_PushDlights)(void);
  int  (*D_SurfaceCacheForRes)(int width, int height);
  void (*D_FlushCaches)(void);
  void (*D_DeleteSurfaceCache)(void);
  void (*D_InitCaches)(void* buffer, int size);
  void (*R_SetVrect)(vrect_t* pvrect, vrect_t* pvrectin, int lineadj);
} ren_api_t;

typedef struct str_api_t
{
  void  (*Q_memset     )(void* dest, int fill, int count);
  void  (*Q_memcpy     )(void* dest, void* src, int count);
  int   (*Q_memcmp     )(void* m1, void* m2, int count);
  void  (*Q_strcpy     )(char* dest, char* src);
  void  (*Q_strncpy    )(char* dest, char* src, int count);
  int   (*Q_strlen     )(char* str);
  char* (*Q_strrchr    )(char* s, char c);
  void  (*Q_strcat     )(char* dest, char* src);
  int   (*Q_strcmp     )(char* s1, char* s2);
  int   (*Q_strncmp    )(char* s1, char* s2, int count);
  int   (*Q_strcasecmp )(char* s1, char* s2);
  int   (*Q_strncasecmp)(char* s1, char* s2, int n);
  int   (*Q_atoi       )(char* str);
  float (*Q_atof       )(char* str);
} str_api_t;

typedef struct math_api_t
{
  vec_t (*_DotProduct    )(vec3_t v1, vec3_t v2);
  void  (*_VectorSubtract)(vec3_t veca, vec3_t vecb, vec3_t out);
  void  (*_VectorAdd     )(vec3_t veca, vec3_t vecb, vec3_t out);
  void  (*_VectorCopy    )(vec3_t in, vec3_t out);
} math_api_t;

typedef struct mem_api_t {
  void  (*Cache_Flush)(void);
  void* (*Cache_Check)(cache_user_t* c);
  void  (*Cache_Free)(cache_user_t* c, bool freetextures);
  void* (*Cache_Alloc)(cache_user_t* c, int size, char* name);
  void  (*Cache_Report)(void);

  void* (*Z_Malloc)(int size);
  void  (*Z_Free)(void* ptr);

  void  (*Hunk_Check)(void);
  void* (*Hunk_Alloc)(int size);
  void* (*Hunk_AllocName)(int size, char* name);
  int   (*Hunk_LowMark)(void);
  void  (*Hunk_FreeToLowMark)(int mark);
  void* (*Hunk_TempAlloc)(int size);

} mem_api_t;

// api agregator
typedef struct quake_api_t
{
    const cvar_api_t* cvar;
    const cmd_api_t* cmd;
    const con_api_t* con;
    const sys_api_t* sys;
    const com_api_t* com;
    const ren_api_t* ren;
    const str_api_t* str;
    const math_api_t* math;
    const mem_api_t* mem;
} quake_api_t;

extern const quake_api_t* GetQuakeAPI();
