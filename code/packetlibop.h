#ifndef _PACKETDEMO_H
#define _PACKETDEMO_H


#include <packet/Packet.h>
#include <packet/InputPacketStream.h>
#include <packet/OutputPacketStream.h>
#include <packet/InputFile.h>
#include <packet/OutputFile.h>
#include <packet/InputSocketServer.h>
#include <packet/OutputSocketClient.h>
#include <iostream>
#include "mac_clock_gettime.h"
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <limits>

using namespace std;
using namespace PacketLib;






void endMiB(bool exitprg, struct timespec start, unsigned long totbytes );
void endHertz(bool exitprg, struct timespec start, unsigned long totbytes, unsigned long nops);

unsigned long encodeNotZeroSuppressedPixelsSlow(OutputPacketStream* ops, int neventstowrite);


#endif