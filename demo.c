#include <unistd.h>
#include <fcntl.h>

#include "UBXParser.h"

int main ( int argc, char **argv ) {
    int ser, len, i;
    char c, buf [ 256 ];
    /* open serial port */
    ser = open ( "/dev/ttyS1", O_RDWR );
    for ( ;; ) {
        /* read data from GNSS module */
        len = read ( ser, buf, sizeof buf );
        /* feed each byte into parser until parsed success */
        for ( i = 0; i < len; i += 1 ) {
            c = buf [ i ];
            ubx_parse ( c );
            if ( ubx_ready ) break;
        }
        /* do something with data received */
        if ( ubx_get ( UBX_CLS_NAV, UBX_ID_NAV_POSLLH ) ) {
            printf ( "location: %d, %d", payload_nav_posllh.lat, payload_nav_posllh.lon ); putchar ( 10 );
        }
        if ( ubx_get ( UBX_CLS_NAV, UBX_ID_NAV_TIMEUTC ) ) {
            printf ( "datetime: %d-%d-%d %d:%d:%d",
                payload_nav_timeutc.year,
                payload_nav_timeutc.month,
                payload_nav_timeutc.day,
                payload_nav_timeutc.hour,
                payload_nav_timeutc.minute,
                payload_nav_timeutc.sec ); putchar ( 10 );
        }
    }
    close ( ser );
}