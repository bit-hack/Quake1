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

static const quake_api_t api_quake = {
  &api_cvar
};

const cvar_api_t *GetAPICVar() {
  return &api_cvar;
}


const quake_api_t* GetQuakeAPI() {
  return &api_quake;
}
