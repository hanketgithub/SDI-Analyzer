
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

int parse(uint8_t *sdi_buf, size_t sdi_buf_size, vector< SAV_T > &savs, vector< EAV_T > &eavs)
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

  for (size_t i = 0; i < sdi_buf_size; i++) {
    if (memcmp(p, sav_mux_nonactive, sizeof(sav_mux_nonactive)) == 0) {
      if (savs.size() == 0) {
        savs.push_back( { (uint64_t) (p - sdi_buf), 0, SAV_TYPE_NON_ACTIVE } );        
      }
      else {
        SAV_T &prev = savs.back();
        uint64_t offset = (uint64_t) (p - sdi_buf);
        uint64_t prev_offset = prev.offset;
        savs.push_back( { offset, offset - prev_offset, SAV_TYPE_NON_ACTIVE } );
      }
    }

    if (memcmp(p, eav_mux_nonactive, sizeof(eav_mux_nonactive)) == 0) {
      if (eavs.size() == 0) {
        eavs.push_back( { (uint64_t) (p - sdi_buf),  0, EAV_TYPE_NON_ACTIVE } ); 
      }
      else {
        EAV_T &prev = eavs.back();
        uint64_t offset = (uint64_t) (p - sdi_buf);
        uint64_t prev_offset = prev.offset;
        eavs.push_back( { offset, offset - prev_offset, EAV_TYPE_NON_ACTIVE } );
      }
    }

    if (memcmp(p, sav_mux_active, sizeof(sav_mux_active)) == 0) {
      if (savs.size() == 0) {
        savs.push_back( { (uint64_t) (p - sdi_buf),  0, SAV_TYPE_ACTIVE } );        
      }
      else {
        SAV_T &prev = savs.back();
        uint64_t offset = (uint64_t) (p - sdi_buf);
        uint64_t prev_offset = prev.offset;
        savs.push_back( { offset, offset - prev_offset, SAV_TYPE_ACTIVE } );
      }
    }

    if (memcmp(p, eav_mux_active, sizeof(eav_mux_active)) == 0) {
      if (eavs.size() == 0) {
        eavs.push_back( { (uint64_t) (p - sdi_buf), 0, EAV_TYPE_ACTIVE } ); 
      }
      else {
        EAV_T &prev = eavs.back();
        uint64_t offset = (uint64_t) (p - sdi_buf);
        uint64_t prev_offset = prev.offset;
        eavs.push_back( { offset, offset - prev_offset, EAV_TYPE_ACTIVE } );
      }
    }
    p++;
  }

  for (size_t i = 0; i < savs.size(); i++) {
    printf("[%lu] Offset=0x%08lx distance=%ld ", i, savs[i].offset, savs[i].distance);
    cout << "type='" << savType2Str( (SAV_TYPE_E) savs[i].type ) << "'" << endl;
  }
  for (size_t i = 0; i < eavs.size(); i++) {
    printf("[%lu] Offset=0x%08lx distance=%ld ", i, eavs[i].offset, eavs[i].distance);
    cout << "type='" << eavType2Str( (EAV_TYPE_E) eavs[i].type ) << "'" << endl;
  }

  cout << "Image width is : " << stride_to_width(savs.back().distance) << endl;

  return 0;
}
