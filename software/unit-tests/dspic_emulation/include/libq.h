#define _Q16 int
#define _Q16div(a, b) ((a/b))
#define _Q16ftoi(x)   ((_Q16)(x*65536))
#define _itofQ16(x)   ((float) x / 65536)


// precomputed value:
#define _Q 16
#define _K   (1 << (_Q - 1))

_Q16 _Q16mpy(_Q16 a, _Q16 b);
