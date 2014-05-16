#include "options.h"

Options& Options::Instance()
{
    static Options instance;
    return instance;
}

Options::Options()
{
}

int Options::GetVerboseLevel()
{
    return mOptions.verbose;
}

void Options::SetVerboseLevel(int level)
{
    mOptions.verbose = level;
}

bool Options::GetErrorBars()
{
    return mOptions.errorbars;
}

void Options::SetErrorBars(bool value)
{
    mOptions.errorbars = value;
}

bool Options::GetDataScope()
{
    return mOptions.datascope;
}

void Options::SetDataScope(bool value)
{
    mOptions.datascope = value;
}

bool Options::GetPlayFiles()
{
    return mOptions.playfiles;
}

void Options::SetPlayFiles(bool value)
{
    mOptions.playfiles = value;
}

bool Options::GetSymbolTiming()
{
    return mOptions.symboltiming;
}

void Options::SetSymbolTiming(bool value)
{
    mOptions.symboltiming = value;
}
