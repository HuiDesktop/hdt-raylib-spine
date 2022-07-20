#include "spine-raylib.h"

typedef struct eventRecorderAtom_t {
	spEventType type;
	spTrackEntry* entry;
	spEvent event;
	struct eventRecorderAtom_t* next;
} eventRecorderAtom;
