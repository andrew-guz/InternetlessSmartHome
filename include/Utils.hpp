#ifndef _UTILS_H_
#define _UTILS_H_

#include <WString.h>
#include <vector>

inline std::vector<String> SplitString(const String& str, char delimiter) {
    std::vector<String> result;
    int startIndex = 0;
    int endIndex = str.indexOf(delimiter);

    while (endIndex >= 0) {
        String part = str.substring(startIndex, endIndex);
        if (part.length() > 0) {
            result.push_back(part);
        }
        startIndex = endIndex + 1;
        endIndex = str.indexOf(delimiter, startIndex);
    }

    String lastPart = str.substring(startIndex);
    if (lastPart.length() > 0) {
        result.push_back(lastPart);
    }

    return result;
}

#endif // _UTILS_H_
