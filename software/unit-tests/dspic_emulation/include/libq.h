#define _Q16 int
#define _Q16div(a, b) ((a/b))
#define _Q16mpy(a, b) ((a*b))
#define _Q16ftoi(x)   ((_Q16)(x*65536))
#define _itofQ16(x)   ((float) x / 65536)
