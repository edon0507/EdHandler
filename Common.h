#ifndef COMMON_H_
#define COMMON_H_

#if (!defined(TARGET_OS_OVERRIDE))
	#define TARGET_OS_OVERRIDE		0
#endif

#if (!defined(TARGET_OS_ANDROID))
// Android
	#if (!TARGET_OS_OVERRIDE)
		#if (defined(__ANDROID__))
			#define	TARGET_OS_ANDROID	1
		#else
			#define	TARGET_OS_ANDROID	0
		#endif
	#else
		#define	TARGET_OS_ANDROID		0
	#endif
#endif

#if (!defined(TARGET_OS_MAC))
// Macintosh
	#if (!TARGET_OS_OVERRIDE)
		#if (defined(__MACH__) && __MACH__)
			// TargetConditionals.h will define this flag.
		#else
			#define	TARGET_OS_MAC		0
		#endif
	#else
		#define TARGET_OS_MAC			0
	#endif
#endif

#if( !defined( TARGET_OS_LINUX ) )
// Linux
	#if( !TARGET_OS_OVERRIDE )
		#if( defined( __linux__ ) )
			#define	TARGET_OS_LINUX			1
		#else
			#define	TARGET_OS_LINUX			0
		#endif
	#else
		#define	TARGET_OS_LINUX				0
	#endif
#endif

#if (!defined(TARGET_OS_WINDOWS_KERNEL))
// Windows
	#if (!TARGET_OS_OVERRIDE)
		#if(defined(NT_INST)) // NT_INST was the best thing I could see defined when building Windows Drivers.
			#define	TARGET_OS_WINDOWS_KERNEL	1
			
			#undef	TARGET_OS_WIN32
			#define	TARGET_OS_WIN32				0 // Force off because EFI uses Windows compilers and looks like Windows otherwise.
		#else
			#define	TARGET_OS_WINDOWS_KERNEL	0
		#endif
	#else
		#define	TARGET_OS_WINDOWS_KERNEL		0
	#endif
#endif

#if (!defined(TARGET_OS_WIN32))
	#if (!TARGET_OS_OVERRIDE)
		#if ((defined(macintosh) && macintosh) || (defined(__MACH__) && __MACH__))
			// ConditionalMacros.h in CoreServices will define this TARGET_* flag.
		#else
			#if (defined(_WIN32) && !TARGET_OS_WINDOWS_KERNEL)
				#define	TARGET_OS_WIN32		1
			#else
				#define	TARGET_OS_WIN32		0
			#endif
		#endif
	#else
		#define	TARGET_OS_WIN32				0
	#endif
#endif
#if (!defined( TARGET_OS_WIN64))
	#if (!TARGET_OS_OVERRIDE)
		#if (defined( _WIN64))
			#define	TARGET_OS_WIN64		1
		#else
			#define	TARGET_OS_WIN64		0
		#endif
	#else
		#define	TARGET_OS_WIN64			0
	#endif
#endif
#if (!defined(TARGET_OS_WINDOWS))
	#if ((defined( TARGET_OS_WIN32) && TARGET_OS_WIN32) || (defined(TARGET_OS_WIN64) && TARGET_OS_WIN64))
		#define	TARGET_OS_WINDOWS	1
	#else
		#define	TARGET_OS_WINDOWS	0
	#endif
#endif

#if(!defined(TARGET_OS_WINDOWS_CE))
// Windows CE
	#if(!TARGET_OS_OVERRIDE)
		#if(defined(_WIN32_WCE))
			#define	TARGET_OS_WINDOWS_CE	1
		#else
			#define	TARGET_OS_WINDOWS_CE	0
		#endif
	#else
		#define	TARGET_OS_WINDOWS_CE		0
	#endif
#endif


#if(TARGET_OS_WINDOWS)
	#include <stdint.h>
	#include <winsock2.h>
	#include <windows.h>

	#include <process.h>
	#include <winioctl.h>
	
	#include <Ws2tcpip.h>
	#include <tchar.h>
	#include <time.h>
	#include <io.h>
	
	typedef SOCKET SocketRef;
	typedef unsigned long long BLEAddressRef;
	typedef int		socklen_t;
	#define	IsValidSocket( X ) ( (X) != INVALID_SOCKET )
	#define kInvalidSocketRef INVALID_SOCKET
	#define	close_compat( X ) closesocket( X )
	#define	usleep( USECS ) Sleep( (DWORD)( ( ( USECS ) / 1000 ) + ( ( ( USECS ) % 1000 ) ? 1 : 0 ) ) )
#else
	#include <stdint.h>

	#include <sys/types.h>
	
	#include <net/if.h>
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <unistd.h>
	#include <arpa/inet.h>
	typedef unsigned long long BLEAddressRef;
	typedef int	SocketRef;
	#define	IsValidSocket( X ) ( (X) >= 0 )
	#define	close_compat( X ) close( X )
	#define kInvalidSocketRef -1

	#define HAVE_PTHREADS 
#endif

// STATIC_INLINE -- Portable way to marking an inline function for use in a header file.

#if( !defined( STATIC_INLINE ) )
#if( defined( __GNUC__ ) && ( __GNUC__ >= 4 ) )
#define STATIC_INLINE		static __inline__ __attribute__( ( always_inline ) )
#elif( defined( __GNUC__ ) )
#define STATIC_INLINE		static __inline__
	#elif( defined( __MWERKS__ ) || defined( __cplusplus ) )
		#define STATIC_INLINE		static inline
	#elif( COMPILER_VISUAL_CPP )
		#define STATIC_INLINE		static __inline
	#elif( defined( __WIN32__ ) )
		#define STATIC_INLINE		static __inline__
	#else
		#define STATIC_INLINE		static inline
#endif
#endif

#ifndef uint8
	typedef uint8_t uint8;
#endif
#ifndef int8
	typedef int8_t int8;
#endif
#ifndef uint16
	typedef uint16_t uint16;
#endif
#ifndef int16
	typedef int16_t int16;
#endif
#ifndef uint32
	typedef uint32_t uint32;
#endif
#ifndef int32
	typedef int32_t int32;
#endif
#ifndef uint64
	typedef uint64_t uint64;
#endif
#ifndef int64
	typedef int64_t int64;
#endif

#if( !defined( __MACTYPES__ ) && !defined( __MACTYPES_H__ ) && !defined( __COREFOUNDATION_CFBASE__ ) )
typedef float		Float32; // 32 bit IEEE float: 1 sign bit, 8 exponent bits, 23 fraction bits.
typedef double		Float64; // 64 bit IEEE float: 1 sign bit, 11 exponent bits, 52 fraction bits.
#endif

// int128 support

typedef struct
{
	int64_t		hi;
	uint64_t	lo;

}	int128_compat;

typedef struct
{
	uint64_t	hi;
	uint64_t	lo;

}	uint128_compat;


//#ifndef int128
//    typedef __int128 int128;
//#endif

#if( !defined( UINT64_C ) )
#if( COMPILER_VISUAL_CPP )
#define UINT64_C( value )	value ## UI64
#else
#define UINT64_C( value )	value ## ULL
#endif
#endif

#define ForgetSocket( X ) \
	do \
	{ \
		if( IsValidSocket( *(X) ) ) \
		{ \
			close_compat( *(X) ); \
			*(X) = kInvalidSocketRef; \
		} \
		\
	}	while( 0 )

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define DISALLOW_EVIL_CONSTRUCTORS(ClassName) \
    ClassName(const ClassName&); \
    void operator=(const ClassName&);

#endif  // end
