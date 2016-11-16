/*Graph*/
#include "graph.h"

void drawSystem (TFT_HX8357 &tft, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t colorb, uint16_t colorf){
     tft.drawRect(x,y,w,h,colorb);
     tft.fillRect(x+3,y+3,w-3,h-3,colorf);
}
