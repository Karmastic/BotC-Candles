#pragma once
#include <WString.h>

#include "IDebugStream.h"

class GithubHelper
{
public:
    static bool getLatestReleaseTag(IDebugStream *debugOutput, const char *owner, const char *repo, String& tag);
};
