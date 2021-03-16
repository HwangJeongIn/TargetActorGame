#pragma once

namespace ta
{
#define KINDA_SMALL_NUMBER	(1.e-4f)

#define INVALID_NAVNODEREF (0)
#define INVALID_NAVQUERYID uint32(0)
#define INVALID_NAVDATA uint32(0)
	//#define INVALID_NAVEXTENT (ta::Vector())

#define DEFAULT_NAV_QUERY_EXTENT_HORIZONTAL 50.f
#define DEFAULT_NAV_QUERY_EXTENT_VERTICAL 250.f

#define RECAST_MAX_SEARCH_NODES		2048

#define RECAST_MIN_TILE_SIZE		300.f

#define RECAST_MAX_AREAS			64
#define RECAST_DEFAULT_AREA			(RECAST_MAX_AREAS - 1)
#define RECAST_LOW_AREA				(RECAST_MAX_AREAS - 2)
#define RECAST_NULL_AREA			0
#define RECAST_UNWALKABLE_POLY_COST	FLT_MAX
}

#include "Common/CommonTypeDefinition.h"
//#include <iostream>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cstdarg>
#include <cfloat>
//#include <algorithm>

// 아래에 #define정의시 다음 에러가 뜬다.
// pch warning header stop cannot be in a linkage block.an intellisense pch file was not generated
// #define ~~
