#ifndef DSD_DISPLAY_H
#define DSD_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

void displayState(dsd_opts * opts, dsd_state * state);
void displayMBEErrors (dsd_opts * opts, dsd_state * state);
void displayFrame(dsd_opts * opts, dsd_state * state, dsd_frame * frame);
void printFrameSync (dsd_opts * opts, dsd_state * state);
void displayActivity(dsd_opts * opts, dsd_frame * frame, dsd_display *display);

#ifdef __cplusplus
}
#endif
#endif
