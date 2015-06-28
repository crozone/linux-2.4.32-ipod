/********************/
/* 4x8 console font */
/********************/

#include <video/font.h>

#define FONTDATAMAX (8 * 256)

static unsigned char fontdata_4x8[FONTDATAMAX] = {
    0,0,0,0,0,0,0,0,64,160,224,160,160,224,160,64,
    64,224,160,224,224,160,224,64,0,160,224,224,224,224,64,0,
    0,64,64,224,224,64,64,0,0,224,64,224,64,64,224,0,
    0,64,64,224,224,64,64,0,0,0,96,96,96,96,0,0,
    240,240,144,144,144,144,240,240,0,96,144,144,144,144,96,0,
    240,144,96,96,96,96,144,240,112,48,80,64,160,160,64,0,
    64,160,160,64,64,224,64,0,96,96,96,64,64,64,192,128,
    224,160,224,160,160,160,160,160,64,224,64,160,160,64,224,64,
    128,192,192,224,192,192,128,0,32,96,96,224,96,96,32,0,
    64,64,224,64,64,224,64,64,160,160,160,160,160,0,160,0,
    112,208,208,80,80,80,80,0,96,144,64,96,96,32,144,96,
    0,0,0,0,96,96,96,0,64,64,224,64,224,64,64,240,
    64,64,224,64,64,64,64,0,64,64,64,64,224,64,64,0,
    0,0,32,240,32,0,0,0,0,0,64,240,64,0,0,0,
    0,0,128,128,128,224,0,0,0,0,96,240,96,0,0,0,
    0,0,96,96,240,240,0,0,0,0,240,240,96,96,0,0,
    0,0,0,0,0,0,0,0,64,64,64,64,64,0,64,0,
    160,160,160,0,0,0,0,0,160,160,224,160,224,160,160,0,
    64,96,192,64,96,192,64,0,0,160,32,64,128,160,0,0,
    64,192,64,224,160,224,64,0,64,64,128,0,0,0,0,0,
    32,64,128,128,128,64,32,0,128,64,32,32,32,64,128,0,
    0,160,64,224,64,160,0,0,0,64,64,224,64,64,0,0,
    0,0,0,0,0,64,64,128,0,0,0,224,0,0,0,0,
    0,0,0,0,0,64,64,0,0,32,32,64,128,128,0,0,
    64,160,160,224,160,160,64,0,64,192,64,64,64,64,64,0,
    64,160,32,32,64,128,224,0,192,32,32,192,32,32,192,0,
    32,96,160,160,224,32,32,0,224,128,192,32,32,160,64,0,
    96,128,128,192,160,160,64,0,224,32,32,64,64,64,64,0,
    64,160,160,64,160,160,64,0,64,160,160,96,32,32,192,0,
    0,64,64,0,0,64,64,0,0,64,64,0,0,64,64,128,
    0,32,64,128,64,32,0,0,0,0,224,0,0,224,0,0,
    0,128,64,32,64,128,0,0,192,32,64,64,64,0,64,0,
    64,160,160,160,160,128,96,0,64,160,160,224,160,160,160,0,
    192,160,160,192,160,160,192,0,96,128,128,128,128,128,96,0,
    192,160,160,160,160,160,192,0,224,128,128,192,128,128,224,0,
    224,128,128,192,128,128,128,0,96,128,128,128,160,160,64,0,
    160,160,160,224,160,160,160,0,224,64,64,64,64,64,224,0,
    32,32,32,32,32,160,64,0,160,160,160,192,160,160,160,0,
    128,128,128,128,128,128,224,0,160,224,224,160,160,160,160,0,
    160,160,224,224,224,160,160,0,64,160,160,160,160,160,64,0,
    192,160,160,192,128,128,128,0,64,160,160,160,160,224,96,0,
    192,160,160,192,160,160,160,0,64,160,128,64,32,160,64,0,
    224,64,64,64,64,64,64,0,160,160,160,160,160,160,224,0,
    160,160,160,160,160,160,64,0,160,160,160,160,224,224,224,0,
    160,160,160,64,160,160,160,0,160,160,160,64,64,64,64,0,
    224,32,64,64,64,128,224,0,224,128,128,128,128,128,224,0,
    0,128,128,64,32,32,0,0,224,32,32,32,32,32,224,0,
    64,160,0,0,0,0,0,0,0,0,0,0,0,0,0,240,
    128,64,32,0,0,0,0,0,0,0,192,32,96,160,96,0,
    128,128,192,160,160,160,192,0,0,0,96,128,128,128,96,0,
    32,32,96,160,160,160,96,0,0,0,64,160,224,128,96,0,
    96,128,128,224,128,128,128,0,0,0,64,160,160,96,32,192,
    128,128,192,160,160,160,160,0,64,0,64,64,64,64,64,0,
    32,0,32,32,32,32,160,64,128,128,160,160,192,160,160,0,
    192,64,64,64,64,64,64,0,0,0,64,224,224,160,160,0,
    0,0,192,160,160,160,160,0,0,0,64,160,160,160,64,0,
    0,0,192,160,160,192,128,128,0,0,96,160,160,96,32,32,
    0,0,160,192,128,128,128,0,0,0,96,128,64,32,192,0,
    128,128,224,128,128,128,96,0,0,0,160,160,160,160,96,0,
    0,0,160,160,160,160,64,0,0,0,160,160,224,224,64,0,
    0,0,160,160,64,160,160,0,0,0,160,160,160,96,32,192,
    0,0,224,32,64,128,224,0,32,64,64,128,64,64,32,0,
    64,64,64,64,64,64,64,0,128,64,64,32,64,64,128,0,
    80,160,0,0,0,0,0,0,0,0,64,160,160,160,224,0,
    64,160,128,128,160,64,64,128,160,0,160,160,160,160,96,0,
    32,64,64,160,224,128,96,0,64,160,192,32,96,160,96,0,
    160,0,192,32,96,160,96,0,128,64,192,32,96,160,96,0,
    64,0,192,32,96,160,96,0,0,0,96,128,128,96,64,128,
    64,160,64,160,224,128,96,0,160,0,64,160,224,128,96,0,
    128,64,64,160,224,128,96,0,160,0,192,64,64,64,64,0,
    64,160,192,64,64,64,64,0,128,64,0,192,64,64,64,0,
    160,0,64,160,224,160,160,0,64,160,64,160,224,160,160,0,
    32,64,224,128,192,128,224,0,0,0,224,64,96,192,96,0,
    96,192,192,224,192,192,224,0,64,160,64,160,160,160,64,0,
    160,0,64,160,160,160,64,0,128,64,64,160,160,160,64,0,
    64,160,0,160,160,160,96,0,128,64,160,160,160,160,96,0,
    160,0,160,160,160,96,32,192,160,0,64,160,160,160,64,0,
    160,0,160,160,160,160,64,0,64,64,96,128,128,96,64,64,
    64,160,128,192,128,160,192,0,160,160,160,224,64,224,64,64,
    128,192,192,160,160,224,160,160,32,64,64,224,64,64,128,0,
    32,64,64,32,96,160,96,0,32,64,0,192,64,64,64,0,
    32,64,64,160,160,160,64,0,32,64,160,160,160,160,96,0,
    80,160,0,192,160,160,160,0,80,160,0,160,224,224,160,0,
    96,160,160,96,0,224,1,0,64,160,160,64,0,224,0,0,
    64,0,64,64,64,128,96,0,0,0,0,224,128,128,0,0,
    0,0,0,224,32,32,0,0,160,32,32,64,128,160,160,0,
    160,32,32,64,128,224,224,0,64,0,64,64,64,64,64,0,
    0,0,96,192,96,0,0,0,0,0,192,96,192,0,0,0,
    0,160,0,160,0,160,0,160,80,160,80,160,80,160,80,160,
    80,240,80,240,80,240,80,240,64,64,64,64,64,64,64,64,
    64,64,64,64,192,64,64,64,64,64,192,64,192,64,64,64,
    96,96,96,96,224,96,96,96,0,0,0,0,224,96,96,96,
    0,0,192,64,192,64,64,64,96,96,224,32,224,96,96,96,
    96,96,96,96,96,96,96,96,0,0,224,32,224,96,96,96,
    96,96,224,32,224,0,0,0,96,96,96,96,224,0,0,0,
    64,64,192,64,192,0,0,0,0,0,0,0,192,64,64,64,
    64,64,64,64,112,0,0,0,64,64,64,64,240,0,0,0,
    0,0,0,0,240,64,64,64,64,64,64,64,112,64,64,64,
    0,0,0,0,240,0,0,0,64,64,64,64,240,64,64,64,
    64,64,112,64,112,64,64,64,96,96,96,96,112,96,96,96,
    96,96,112,64,112,0,0,0,0,0,112,64,112,96,96,96,
    96,96,240,0,240,0,0,0,0,0,240,0,240,96,96,96,
    96,96,112,64,112,96,96,96,0,0,240,0,240,0,0,0,
    96,96,240,0,240,96,96,96,64,64,240,0,240,0,0,0,
    96,96,96,96,240,0,0,0,0,0,240,0,240,64,64,64,
    0,0,0,0,240,96,96,96,96,96,96,96,112,0,0,0,
    64,64,112,64,112,0,0,0,0,0,112,64,112,64,64,64,
    0,0,0,0,112,96,96,96,96,96,96,96,240,96,96,96,
    64,64,240,64,240,64,64,64,64,64,64,64,192,0,0,0,
    0,0,0,0,112,64,64,64,240,240,240,240,240,240,240,240,
    0,0,0,0,240,240,240,240,192,192,192,192,192,192,192,192,
    48,48,48,48,48,48,48,48,240,240,240,240,0,0,0,0,
    0,0,96,160,160,160,96,0,64,160,160,192,160,160,192,0,
    224,160,128,128,128,128,128,0,0,0,224,160,160,160,160,0,
    224,160,128,64,128,160,224,0,0,0,96,160,160,160,64,0,
    0,0,160,160,160,160,192,128,0,80,160,64,64,64,64,0,
    224,64,64,160,160,64,64,224,64,160,160,224,160,160,64,0,
    64,160,160,224,64,64,224,0,32,64,64,96,160,160,64,0,
    0,0,64,224,224,64,0,0,32,64,64,224,224,64,64,128,
    96,128,128,224,128,128,97,0,0,64,160,160,160,160,160,0,
    0,224,0,224,0,224,0,0,64,64,224,64,64,0,224,0,
    128,64,32,64,128,0,224,0,32,64,128,64,32,0,224,0,
    32,64,64,64,64,64,64,64,64,64,64,64,64,64,64,128,
    0,64,0,224,0,64,0,0,0,80,160,0,80,160,0,0,
    64,160,160,64,0,0,0,0,0,0,0,64,64,0,0,0,
    0,0,0,64,0,0,0,0,48,32,32,32,32,160,96,32,
    64,160,160,160,160,0,0,0,64,160,32,64,224,0,0,0,
    0,0,96,96,96,96,0,0,0,0,0,0,0,0,0,0
};

struct fbcon_font_desc font_vtx_4x8 = {
    VTX4x8_IDX,
    "vtx4x8",
    4,
    8,
    fontdata_4x8,
    0
};
