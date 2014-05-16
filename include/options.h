#ifndef OPTIONS_H
#define OPTIONS_H

#include "dsd.h"

class Options
{
public:
    static Options& Instance();

    int GetVerboseLevel();
    bool GetErrorBars();
    bool GetDataScope();
    bool GetPlayFiles();
    bool GetSymbolTiming();

    void SetVerboseLevel(int);
    void SetErrorBars(bool);
    void SetDataScope(bool);
    void SetPlayFiles(bool);
    void SetSymbolTiming(bool);

private:
    Options();
    Options(const Options&);
    Options& operator=(const Options&);

    dsd_opts mOptions;
};

#endif
