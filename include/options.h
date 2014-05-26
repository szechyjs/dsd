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
    bool GetUseCosineFilter();
    bool GetUnmuteEncryptedP25();
    bool GetSplit();
    bool GetFrameDstar();
    bool GetFrameX2tdma();
    bool GetFrameP25p1();
    bool GetFrameNxdn48();
    bool GetFrameNxdn96();
    bool GetFrameDmr();
    bool GetFrameProvoice();
    bool GetModC4fm();
    bool GetModQpsk();
    bool GetModGfsk();
    bool GetInvertedX2tdma();
    bool GetInvertedDmr();

    void SetVerboseLevel(int);
    void SetErrorBars(bool);
    void SetDataScope(bool);
    void SetPlayFiles(bool);
    void SetSymbolTiming(bool);
    void SetUseCosineFilter(bool);
    void SetUnmuteEncryptedP25(bool);
    void SetSplit(bool);
    void SetFrameDstar(bool);
    void SetFrameX2tdma(bool);
    void SetFrameP25p1(bool);
    void SetFrameNxdn48(bool);
    void SetFrameNxdn96(bool);
    void SetFrameDmr(bool);
    void SetFrameProvoice(bool);
    void SetModC4fm(bool);
    void SetModQpsk(bool);
    void SetModGfsk(bool);
    void SetInvertedX2tdma(bool);
    void SetInvertedDmr(bool);

private:
    Options();
    Options(const Options&);
    Options& operator=(const Options&);

    dsd_opts mOptions;
};

#endif
