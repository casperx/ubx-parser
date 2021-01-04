#include "UBXParser.h"

#define WRITE_STRUCT( var, pos ) ( ( unsigned char * ) &var ) [ pos ]

/* parser states */

bool ubx_ready;

#define PARSER_WATCH_SYNC 1
#define PARSER_RECV_HEADER 2
#define PARSER_RECV_PAYLOAD 3
#define PARSER_RECV_CHECKSUM 4

int parser_state = PARSER_WATCH_SYNC;

/* packet sync state */

const unsigned char UBX_SYNC_CHAR [ 2 ] = { 0xB5, 0x62 };

int sync_pos = 0;

/* parsed data */

UBX_HEADER ubx_header;

UBX_PAYLOAD_NAV_POSLLH payload_nav_posllh;
UBX_PAYLOAD_NAV_TIMEUTC payload_nav_timeutc;

unsigned char ubx_checksum [ 2 ];

int header_pos = 0;
int payload_pos = 0;
int checksum_pos = 0;

unsigned char packet_checksum [ 2 ];

void ubx_parse ( unsigned char data ) {
  if ( parser_state == PARSER_RECV_HEADER || parser_state == PARSER_RECV_PAYLOAD ) {
    packet_checksum [ 1 ] += packet_checksum [ 0 ] += data;
  }

  switch ( parser_state ) {
  case PARSER_WATCH_SYNC:
    if ( UBX_SYNC_CHAR [ sync_pos ] == data ) {
      sync_pos += 1;
      if ( sync_pos == sizeof UBX_SYNC_CHAR ) {
        sync_pos = 0;
        ubx_ready = false;
        memset ( packet_checksum, 0, sizeof packet_checksum );
        parser_state = PARSER_RECV_HEADER;
      }
    } else {
      sync_pos = 0;
    }
    break;

  case PARSER_RECV_HEADER:
    WRITE_STRUCT ( ubx_header, header_pos ) = data;
    header_pos += 1;
    if ( header_pos == sizeof ubx_header ) {
      header_pos = 0;
      parser_state = PARSER_RECV_PAYLOAD;
    }
    break;

  case PARSER_RECV_PAYLOAD:
    switch ( ubx_header.cls ) {
    case UBX_CLS_NAV:
      switch ( ubx_header.id ) {
      case UBX_ID_NAV_POSLLH:
        WRITE_STRUCT ( payload_nav_posllh, payload_pos ) = data;
        break;
      case UBX_ID_NAV_TIMEUTC:
        WRITE_STRUCT ( payload_nav_timeutc, payload_pos ) = data;
        break;
      }
      break;
    }
    payload_pos += 1;
    if ( payload_pos == ubx_header.len ) {
      payload_pos = 0;
      parser_state = PARSER_RECV_CHECKSUM;
    }
    break;

  case PARSER_RECV_CHECKSUM:
    ubx_checksum [ checksum_pos ] = data;
    checksum_pos += 1;
    if ( checksum_pos == sizeof ubx_checksum ) {
      checksum_pos = 0;
      if ( memcmp ( ubx_checksum, packet_checksum, sizeof ubx_checksum ) == 0 ) ubx_ready = true;
      parser_state = PARSER_WATCH_SYNC;
    }
    break;
  }
}

bool ubx_get ( unsigned char cls, unsigned char id ) {
  bool match;
  match = ubx_ready && (ubx_header.cls == cls && ubx_header.id == id);
  if ( match )
    ubx_ready = false;
  return match;
}

