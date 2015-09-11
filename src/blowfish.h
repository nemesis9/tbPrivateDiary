#ifndef _BLOWFISH_H
#define _BLOWFISH_H

#include "blf.h"

extern void blf_key(blf_ctx *c, const uint8_t *k, uint16_t len);
extern void blf_enc(blf_ctx *c, uint32_t *data, uint16_t blocks);
extern void blf_dec(blf_ctx *c, uint32_t *data, uint16_t blocks);



#endif
