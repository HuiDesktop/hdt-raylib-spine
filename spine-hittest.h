#include "spine-raylib.h"
#include <stdint.h>

typedef struct HitTestRecorder_t {
	uint64_t count;
	void** list;
	uint64_t capacity;
} HitTestRecorder;
