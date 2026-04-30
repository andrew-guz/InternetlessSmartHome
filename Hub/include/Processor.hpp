#ifndef _PROCESSOR_H_
#define _PROCESSOR_H_

#include <WString.h>
#include <functional>

typedef std::function<void(const String&)> SendReply;

void ProcessCommand(const String& command, SendReply sendFunction);

#endif // _PROCESSOR_H_
