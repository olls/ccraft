/*  GIMP header image file format (RGB): /home/olls/repos/c/ccraft/img/thing.h  */

static unsigned int width = 8;
static unsigned int height = 8;

/*  Call this macro repeatedly.  After each use, the pixel data can be extracted  */

#define HEADER_PIXEL(data,pixel) {\
pixel[0] = (((data[0] - 33) << 2) | ((data[1] - 33) >> 4)); \
pixel[1] = ((((data[1] - 33) & 0xF) << 4) | ((data[2] - 33) >> 2)); \
pixel[2] = ((((data[2] - 33) & 0x3) << 6) | ((data[3] - 33))); \
data += 4; \
}
static char *dirt =
	"!!!!FZ?8FZ?8;'BIFZ?8FZ?8!!!!FZ?8FZ?8;'BI!!!!U*_@!!!!;'BIFZ?8;'BI"
	"FZ?8!!!!W5R-\\9W.W5R-!!!!FZ?8!!!!;'BIU*_@\\9W.[C=H\\9W.U*_@;'BIFZ?8"
	"FZ?8!!!!W5R-\\9W.W5R-!!!!FZ?8!!!!FZ?8;'BI!!!!U*_@!!!!;'BIFZ?8;'BI"
	"!!!!FZ?8FZ?8;'BIFZ?8FZ?8!!!!FZ?8FZ?8;'BI!!!!FZ?8!!!!;'BIFZ?8;'BI"
	"";
