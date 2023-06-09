
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <vector>
#include <string>

#include "parser.h"

using namespace std;

typedef long long LL;

uint8_t SAV_active[] = { 0x03, 0xff, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00 };
uint8_t EAV_active[] = { 0x03, 0xff, 0x00, 0x00, 0x00, 0x00, 0x02, 0x74 };
uint8_t SAV_nonactive[] = { 0x03, 0xff, 0x00, 0x00, 0x00, 0x00, 0x02, 0xAC };
uint8_t EAV_nonactive[] = { 0x03, 0xff, 0x00, 0x00, 0x00, 0x00, 0x02, 0xDB };



// Not sure if we can compute the width, since the layout of a stride can be varies regarding
// to resolution
uint32_t stride_to_width
(
    uint32_t stride
)
{
    switch (stride) {
        case 8800:
        {
            return 1920;
        }
        default:
        {
            return 0;
        }
    }
}


string savType2Str(SAV_TYPE_E eType) {
    switch (eType) {
        case SAV_TYPE_NON_ACTIVE:
        {
            return "SAV Non Active";
        }
        case SAV_TYPE_ACTIVE:
        {
            return "SAV Active";
        }
        default:
        {
            return "unknown";
        }
    }
    return "unknown";
}

string eavType2Str(EAV_TYPE_E eType) {
    switch (eType) {
        case EAV_TYPE_NON_ACTIVE:
        {
            return "EAV Non Active";
        }
        case EAV_TYPE_ACTIVE:
        {
            return "EAV Active";
        }
        default:
        {
            return "unknown";
        }
    }
    return "unknown";
}

void buildMuxPattern(uint8_t mux[], uint8_t pattern[], size_t pattern_len) {
  int k = 0;
  for (size_t i = 0; i < pattern_len; i += 2) {
    mux[k++] = pattern[i];
    mux[k++] = pattern[i+1];
    mux[k++] = pattern[i];
    mux[k++] = pattern[i+1];
  }
}

int parse(uint8_t *sdi_buf, size_t sdi_buf_size)
{
  uint8_t *p = sdi_buf;

  size_t len = sizeof(SAV_active);
  uint8_t sav_mux_nonactive[ len*2 ];   // multiplex non active SAV pattern
  uint8_t eav_mux_nonactive[ len*2 ];   // multiplex non active EAV pattern
  uint8_t sav_mux_active[ len*2 ];      // multiplex active SAV pattern
  uint8_t eav_mux_active[ len*2 ];      // multiplex active EAV pattern


  buildMuxPattern(sav_mux_nonactive, SAV_nonactive, len);
  cout << "SAV Multiplex Non Active Pattern: " << endl;
  for (size_t i = 0; i < sizeof(sav_mux_nonactive); i++) {
    printf("0x%02x ", sav_mux_nonactive[i]);
  }
  printf("\n");

  buildMuxPattern(eav_mux_nonactive, EAV_nonactive, len);
  cout << "EAV Multiplex Non Active Pattern: " << endl;
  for (size_t i = 0; i < sizeof(eav_mux_nonactive); i++) {
    printf("0x%02x ", eav_mux_nonactive[i]);
  }
  printf("\n");

  buildMuxPattern(sav_mux_active, SAV_active, len);
  cout << "SAV Multiplex Active Pattern: " << endl;
  for (size_t i = 0; i < sizeof(sav_mux_active); i++) {
    printf("0x%02x ", sav_mux_active[i]);
  }
  printf("\n");

  buildMuxPattern(eav_mux_active, EAV_active, len);
  cout << "EAV Multiplex Active Pattern: " << endl;
  for (size_t i = 0; i < sizeof(eav_mux_active); i++) {
    printf("0x%02x ", eav_mux_active[i]);
  }
  printf("\n");

  vector< vector<LL> > vec_sav;
  vector< vector<LL> > vec_eav;
  for (size_t i = 0; i < sdi_buf_size; i++) {
    if (memcmp(p, sav_mux_nonactive, sizeof(sav_mux_nonactive)) == 0) {
      if (vec_sav.size() == 0) {
        vec_sav.push_back( { p - sdi_buf,  -1, SAV_TYPE_NON_ACTIVE } );        
      }
      else {
        vector<LL> &prev = vec_sav.back();
        LL offset = p - sdi_buf;
        LL prev_offset = prev[0];
        vec_sav.push_back( { offset, offset - prev_offset, SAV_TYPE_NON_ACTIVE } );
      }
    }

    if (memcmp(p, eav_mux_nonactive, sizeof(eav_mux_nonactive)) == 0) {
      if (vec_eav.size() == 0) {
        vec_eav.push_back( { p - sdi_buf,  -1, EAV_TYPE_NON_ACTIVE } ); 
      }
      else {
        vector<LL> &prev = vec_eav.back();
        LL offset = p - sdi_buf;
        LL prev_offset = prev[0];
        vec_eav.push_back( {offset, offset - prev_offset, EAV_TYPE_NON_ACTIVE } );
      }
    }

    if (memcmp(p, sav_mux_active, sizeof(sav_mux_active)) == 0) {
      if (vec_sav.size() == 0) {
        vec_sav.push_back( { p - sdi_buf,  -1, SAV_TYPE_ACTIVE } );        
      }
      else {
        vector<LL> &prev = vec_sav.back();
        LL offset = p - sdi_buf;
        LL prev_offset = prev[0];
        vec_sav.push_back( { offset, offset - prev_offset, SAV_TYPE_ACTIVE } );
      }
    }

    if (memcmp(p, eav_mux_active, sizeof(eav_mux_active)) == 0) {
      if (vec_eav.size() == 0) {
        vec_eav.push_back( { p - sdi_buf,  -1, EAV_TYPE_ACTIVE } ); 
      }
      else {
        vector<LL> &prev = vec_eav.back();
        LL offset = p - sdi_buf;
        LL prev_offset = prev[0];
        vec_eav.push_back( {offset, offset - prev_offset, EAV_TYPE_ACTIVE } );
      }
    }
    p++;
  }

  for (size_t i = 0; i < vec_sav.size(); i++) {
    printf("[%lu] Offset=0x%08llx distance=%lld ", i, vec_sav[i][0], vec_sav[i][1]);
    cout << "type='" << savType2Str( (SAV_TYPE_E) vec_sav[i][2] ) << "'" << endl;
  }
  for (size_t i = 0; i < vec_eav.size(); i++) {
    printf("[%lu] Offset=0x%08llx distance=%lld ", i, vec_eav[i][0], vec_eav[i][1]);
    cout << "type='" << eavType2Str( (EAV_TYPE_E) vec_eav[i][2] ) << "'" << endl;
  }

  cout << "Image width is : " << stride_to_width(vec_sav.back()[1]) << endl;

  return 0;
}
