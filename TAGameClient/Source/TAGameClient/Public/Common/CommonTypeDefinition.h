#pragma once

namespace ta
{
	typedef unsigned __int64	uint64;
	typedef unsigned int		uint32;
	typedef unsigned short		uint16;
	typedef unsigned __int8		uint8;

	typedef __int64				int64;
	typedef int					int32;
	typedef short				int16;
	typedef __int8				int8;

	typedef unsigned __int64	hash_value;

#define _MIN_UINT8		((uint8)	0x00)
#define	_MIN_UINT16		((uint16)	0x0000)
#define	_MIN_UINT32		((uint32)	0x00000000)
#define _MIN_UINT64		((uint64)	0x0000000000000000)
#define _MIN_INT8		((int8)		-128)
#define _MIN_INT16		((int16)	-32768)
#define _MIN_INT32		((int32)	0x80000000)
#define _MIN_INT64		((int64)	0x8000000000000000)

#define _MAX_UINT8		((uint8)	0xff)
#define _MAX_UINT16		((uint16)	0xffff)
#define _MAX_UINT32		((uint32)	0xffffffff)
#define _MAX_UINT64		((uint64)	0xffffffffffffffff)
#define _MAX_INT8		((int8)		0x7f)
#define _MAX_INT16		((int16)	0x7fff)
#define _MAX_INT32		((int32)	0x7fffffff)
#define _MAX_INT64		((int64)	0x7fffffffffffffff)

#define _MIN_FLT			(1.175494351e-38F)			/* min positive value */
#define _MAX_FLT			(3.402823466e+38F)
#define _MIN_DBL			(2.2250738585072014e-308)	/* min positive value */
#define _MAX_DBL			(1.7976931348623158e+308)	


}
