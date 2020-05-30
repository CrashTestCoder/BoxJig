#pragma once

inline extern int serialOpen(char const *, int) { return 0; }

inline extern char serialGetchar(int) { return 0; }

inline extern void serialFlush(int) {}

inline extern int serialDataAvail(int) { return 0; }