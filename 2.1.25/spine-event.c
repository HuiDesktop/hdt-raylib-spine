#include "spine-event.h"

HDTLAPI void eventListenerFunc(spAnimationState* state, int trackIndex, spEventType type, spEvent* event, int loopCount) {
	eventRecorderAtom *a = CALLOC(eventRecorderAtom, 1);
	a->type = type;
	a->entry = state->tracks[trackIndex];
	a->next = state->userData;
	state->userData = a;
	if (event) {
		memcpy(&a->event, event, sizeof(a->event));
	}
}

HDTLAPI void releaseAllEvents(spAnimationState* state) {
	eventRecorderAtom* a = state->userData;
	while (a != NULL)
	{
		eventRecorderAtom* aa = a->next;
		FREE(a);
		a = aa;
	}
	state->userData = NULL;
}
