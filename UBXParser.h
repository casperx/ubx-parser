/*

This is UBX Protocol Parser for u-blox GNSS Receiver.
written by CasperX

*/

#include <stdbool.h>

/* packet header */

struct UBX_HEADER {
    unsigned char cls;
    unsigned char id;

    unsigned short len;
};

typedef struct UBX_HEADER UBX_HEADER;

/* payload structure */

struct UBX_PAYLOAD_NAV_POSLLH {
    unsigned long ToW;

    long lon;
    long lat;

    long height;
    long heightMSL;

    unsigned long hAcc;
    unsigned long vAcc;
};

typedef struct UBX_PAYLOAD_NAV_POSLLH UBX_PAYLOAD_NAV_POSLLH;

struct UBX_PAYLOAD_NAV_TIMEUTC {
    unsigned long ToW;

    unsigned long acc;

    long nano;

    short year;
    char month;
    char day;

    char hour;
    char minute;
    char sec;

    char valid;
};

typedef struct UBX_PAYLOAD_NAV_TIMEUTC UBX_PAYLOAD_NAV_TIMEUTC;

/* interfaces */

/* payload class */

#define UBX_CLS_NAV 1

/* payload id */

#define UBX_ID_NAV_POSLLH 2
#define UBX_ID_NAV_TIMEUTC 33

/* parsed data */

extern bool ubx_ready;

extern UBX_HEADER ubx_header;

extern UBX_PAYLOAD_NAV_POSLLH payload_nav_posllh;
extern UBX_PAYLOAD_NAV_TIMEUTC payload_nav_timeutc;

void ubx_parse ( unsigned char data );

bool ubx_get ( unsigned char cls, unsigned char id );
