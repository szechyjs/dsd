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

bool Options::GetUseCosineFilter()
{
    return mOptions.use_cosine_filter;
}

void Options::SetUseCosineFilter(bool value)
{
    mOptions.use_cosine_filter = value;
}

bool Options::GetUnmuteEncryptedP25()
{
    return mOptions.unmute_encrypted_p25;
}

void Options::SetUnmuteEncryptedP25(bool value)
{
    mOptions.unmute_encrypted_p25 = value;
}

bool Options::GetSplit()
{
    return mOptions.split;
}

void Options::SetSplit(bool value)
{
    mOptions.split = value;
}

bool Options::GetFrameDstar()
{
    return mOptions.frame_dstar;
}

void Options::SetFrameDstar(bool value)
{
    mOptions.frame_dstar = value;
}

bool Options::GetFrameX2tdma()
{
    return mOptions.frame_x2tdma;
}

void Options::SetFrameX2tdma(bool value)
{
    mOptions.frame_x2tdma = value;
}

bool Options::GetFrameP25p1()
{
    return mOptions.frame_p25p1;
}

void Options::SetFrameP25p1(bool value)
{
    mOptions.frame_p25p1 = value;
}

bool Options::GetFrameNxdn48()
{
    return mOptions.frame_nxdn48;
}

void Options::SetFrameNxdn48(bool value)
{
    mOptions.frame_nxdn48 = value;
}

bool Options::GetFrameNxdn96()
{
    return mOptions.frame_nxdn96;
}

void Options::SetFrameNxdn96(bool value)
{
    mOptions.frame_nxdn96 = value;
}

bool Options::GetFrameDmr()
{
    return mOptions.frame_dmr;
}

void Options::SetFrameDmr(bool value)
{
    mOptions.frame_dmr = value;
}

bool Options::GetFrameProvoice()
{
    return mOptions.frame_provoice;
}

void Options::SetFrameProvoice(bool value)
{
    mOptions.frame_provoice = value;
}

bool Options::GetModC4fm()
{
    return mOptions.mod_c4fm;
}

void Options::SetModC4fm(bool value)
{
    mOptions.mod_c4fm = value;
}

bool Options::GetModQpsk()
{
    return mOptions.mod_qpsk;
}

void Options::SetModQpsk(bool value)
{
    mOptions.mod_qpsk = value;
}

bool Options::GetModGfsk()
{
    return mOptions.mod_gfsk;
}

void Options::SetModGfsk(bool value)
{
    mOptions.mod_gfsk = value;
}


bool Options::GetInvertedX2tdma()
{
    return mOptions.inverted_x2tdma;
}

void Options::SetInvertedX2tdma(bool value)
{
    mOptions.inverted_x2tdma = value;
}

bool Options::GetInvertedDmr()
{
    return mOptions.inverted_dmr;
}

void Options::SetInvertedDmr(bool value)
{
    mOptions.inverted_dmr = value;
}
