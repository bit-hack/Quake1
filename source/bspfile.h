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
#pragma once
#include <stdint.h>

enum
{
    MAX_MAP_HULLS = 4,
    MAX_MAP_MODELS = 256,
    MAX_MAP_BRUSHES = 4096,
    MAX_MAP_ENTITIES = 1024,
    MAX_MAP_ENTSTRING = 65536,
    MAX_MAP_PLANES = 32767,
    MAX_MAP_NODES = 32767, // because negative shorts are contents
    MAX_MAP_CLIPNODES = 32767,
    MAX_MAP_LEAFS = 32767, //johnfitz -- was 8192
    MAX_MAP_VERTS = 65535,
    MAX_MAP_FACES = 65535,
    MAX_MAP_MARKSURFACES = 65535,
    MAX_MAP_TEXINFO = 4096,
    MAX_MAP_EDGES = 256000,
    MAX_MAP_SURFEDGES = 512000,
    MAX_MAP_TEXTURES = 512,
    MAX_MAP_MIPTEX = 0x200000,
    MAX_MAP_LIGHTING = 0x100000,
    MAX_MAP_VISIBILITY = 0x100000,
    MAX_MAP_PORTALS = 65536,
};

enum
{
    LUMP_ENTITIES = 0,
    LUMP_PLANES = 1,
    LUMP_TEXTURES = 2,
    LUMP_VERTEXES = 3,
    LUMP_VISIBILITY = 4,
    LUMP_NODES = 5,
    LUMP_TEXINFO = 6,
    LUMP_FACES = 7,
    LUMP_LIGHTING = 8,
    LUMP_CLIPNODES = 9,
    LUMP_LEAFS = 10,
    LUMP_MARKSURFACES = 11,
    LUMP_EDGES = 12,
    LUMP_SURFEDGES = 13,
    LUMP_MODELS = 14,
    HEADER_LUMPS = 15,
};

enum
{
    // 0-2 are axial planes
    PLANE_X = 0,
    PLANE_Y = 1,
    PLANE_Z = 2,
    // 3-5 are non-axial planes snapped to the nearest
    PLANE_ANYX = 3,
    PLANE_ANYY = 4,
    PLANE_ANYZ = 5,
};

enum
{
    CONTENTS_EMPTY = -1,
    CONTENTS_SOLID = -2,
    CONTENTS_WATER = -3,
    CONTENTS_SLIME = -4,
    CONTENTS_LAVA = -5,
    CONTENTS_SKY = -6,
    CONTENTS_ORIGIN = -7, // removed at csg time
    CONTENTS_CLIP = -8, // changed to contents_solid
    CONTENTS_CURRENT_0 = -9,
    CONTENTS_CURRENT_90 = -10,
    CONTENTS_CURRENT_180 = -11,
    CONTENTS_CURRENT_270 = -12,
    CONTENTS_CURRENT_UP = -13,
    CONTENTS_CURRENT_DOWN = -14,
};

enum
{
    TEX_SPECIAL = 1, // sky or slime, no lightmap or 256 subdivision
    TEX_MISSING = 2, // johnfitz -- this texinfo does not have a texture
};

enum
{
    AMBIENT_WATER = 0,
    AMBIENT_SKY = 1,
    AMBIENT_SLIME = 2,
    AMBIENT_LAVA = 3,
    NUM_AMBIENTS = 4, // automatic ambient sounds
};

#define MAX_KEY 32
#define MAX_VALUE 1024
#define MAXLIGHTMAPS 4
#define BSPVERSION 29
#define TOOLVERSION 2
#define MIPLEVELS 4

typedef struct
{
    int32_t fileofs, filelen;
} lump_t;

typedef struct
{
    float mins[3], maxs[3];
    float origin[3];
    int32_t headnode[MAX_MAP_HULLS];
    int32_t visleafs; // not including the solid leaf 0
    int32_t firstface, numfaces;
} dmodel_t;

typedef struct
{
    int32_t version;
    lump_t lumps[HEADER_LUMPS];
} dheader_t;

typedef struct
{
    int32_t nummiptex;
    int32_t dataofs[4]; // [nummiptex]
} dmiptexlump_t;

typedef struct miptex_s
{
    int8_t name[16];
    uint32_t width, height;
    uint32_t offsets[MIPLEVELS]; // four mip maps stored
} miptex_t;

typedef struct
{
    float point[3];
} dvertex_t;

typedef struct
{
    float normal[3];
    float dist;
    int32_t type; // PLANE_X - PLANE_ANYZ ?remove? trivial to regenerate
} dplane_t;

typedef struct
{
    int32_t planenum;
    int16_t children[2]; // negative numbers are -(leafs+1), not nodes
    int16_t mins[3]; // for sphere culling
    int16_t maxs[3];
    uint16_t firstface;
    uint16_t numfaces; // counting both sides
} dnode_t;

typedef struct
{
    int32_t planenum;
    int16_t children[2]; // negative numbers are contents
} dclipnode_t;

typedef struct texinfo_s
{
    float vecs[2][4]; // [s/t][xyz offset]
    int32_t miptex;
    int32_t flags;
} texinfo_t;

// note that edge 0 is never used, because negative edge nums are used for
// counterclockwise use of the edge in a face
typedef struct
{
    uint16_t v[2]; // vertex numbers
} dedge_t;

typedef struct
{
    int16_t planenum;
    int16_t side;

    int32_t firstedge; // we must support > 64k edges
    int16_t numedges;
    int16_t texinfo;

    // lighting info
    uint8_t styles[MAXLIGHTMAPS];
    int32_t lightofs; // start of [numstyles*surfsize] samples
} dface_t;

// leaf 0 is the generic CONTENTS_SOLID leaf, used for all solid areas
// all other leafs need visibility info
typedef struct
{
    int32_t contents;
    int32_t visofs; // -1 = no visibility info

    int16_t mins[3]; // for frustum culling
    int16_t maxs[3];

    uint16_t firstmarksurface;
    uint16_t nummarksurfaces;

    uint8_t ambient_level[NUM_AMBIENTS];
} dleaf_t;
