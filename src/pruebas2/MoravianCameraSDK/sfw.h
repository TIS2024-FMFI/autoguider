// sfw: Standalone Filter Wheel USB driver

# pragma once
# pragma pack(8)

# include "windows.h"

# define EXPORT_ __declspec(dllimport)

// messages notifying FW connect state change
// sent to HWND passed as RegisterNotifyHWND parameter
#define WM_FW_CONNECT    1039
#define WM_FW_DISCONNECT 1040

// GetBoolean indexes
#define gbConnected               0
#define gbInitialized             1
#define gbMicrometerFilterOffsets 2
#define gbConfigured              127

// GetInteger indexes
#define giVersion1                0
#define giVersion2                1
#define giVersion3                2
#define giVersion4                3
#define giFilterWheelId           4
#define giFilters                 5

// GetString indexes
#define gsFWDescription           0
#define gsManufacturer            1
#define gsSerialNumber            2

namespace sfw {

typedef int           INTEGER;
typedef unsigned      CARDINAL;
typedef char          CHAR;
typedef unsigned char BOOLEAN;
typedef void *        ADDRESS;

struct CSFW;

extern "C" EXPORT_ void    __cdecl Enumerate(void (__cdecl *CallbackProc)( CARDINAL ));
extern "C" EXPORT_ CSFW *  __cdecl Create (CARDINAL Id );
extern "C" EXPORT_ void    __cdecl Release( CSFW *PSFW );
extern "C" EXPORT_ void    __cdecl Initialize( CSFW *PSFW );
extern "C" EXPORT_ void    __cdecl UnInitialize( CSFW *PSFW );
extern "C" EXPORT_ void    __cdecl RegisterNotifyHWND( CSFW *PSFW, ADDRESS NotifyHWND );
extern "C" EXPORT_ BOOLEAN __cdecl GetBoolean( CSFW *PSFW, CARDINAL Index, BOOLEAN *Boolean );
extern "C" EXPORT_ BOOLEAN __cdecl GetInteger( CSFW *PSFW, CARDINAL Index, INTEGER *Integer );
extern "C" EXPORT_ BOOLEAN __cdecl GetString( CSFW *PSFW, CARDINAL Index, CARDINAL String_HIGH, CHAR *String );
extern "C" EXPORT_ BOOLEAN __cdecl EnumerateFilters( CSFW *PSFW, CARDINAL Index, CARDINAL Description_HIGH, CHAR *Description, CARDINAL *Color, INTEGER *Offset );
extern "C" EXPORT_ BOOLEAN __cdecl SetFilter( CSFW *PSFW, CARDINAL Index );
extern "C" EXPORT_ BOOLEAN __cdecl ReinitFilterWheel( CSFW *PSFW );

}; // namespace sfw
