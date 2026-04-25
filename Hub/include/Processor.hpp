#ifndef _PROCESSOR_H_
#define _PROCESSOR_H_

#include <WString.h>

void ProcessCommand(const String& command);

// NAME;00:00:00:00:00:00;newName$
// NAME;old_name;newName$
void Rename(const String& command);

// BRIGHTNESS;00:00:00:00:00:00;value$
// BRIGHTNESS;name;value$
void SetBrightness(const String& command);

// STATE;00:00:00:00:00:00;value$
// STATE;name;value$
void SetState(const String& command);

#endif // _PROCESSOR_H_
