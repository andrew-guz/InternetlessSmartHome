#ifndef _PROCESSOR_H_
#define _PROCESSOR_H_

#include <WString.h>

void ProcessCommand(const String& command);

// SET_NAME;00:00:00:00:00:00;newName$
// SET_NAME;old_name;newName$
void Rename(const String& command);

// SET_BRIGHTNESS;00:00:00:00:00:00;value$
// SET_BRIGHTNESS;name;value$
// SET_BRIGHTNESS;ALL;value$
void SetBrightness(const String& command);

// SET_STATE;00:00:00:00:00:00;value$
// SET_STATE;name;value$
// SET_STATE;ALL;value$
void SetState(const String& command);

#endif // _PROCESSOR_H_
