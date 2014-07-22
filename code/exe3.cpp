/***************************************************************************
	main.cpp
	-------------------
 copyright            : (C) 2014 Andrea Bulgarelli, Andrea Zoli, Valentina Fioretti
 email                : bulgarelli@iasfbo.inaf.it
						zoli@iasfbo.inaf.it
						fioretti@iasfbo.inaf.it
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <packet/Packet.h>
#include <packet/InputPacketStream.h>
#include <packet/InputFile.h>
#include <iostream>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>


using namespace std;
using namespace PacketLib;

int main(int argc, char *argv[]) {

	string ctarta;
	
	if(argc == 2) {
		const char* home = getenv("CTARTA");
		if (!home)
		{
			std::cerr << "ERROR: CTARTA environment variable is not defined." << std::endl;
			return 0;
		}
		ctarta = home;
		
	} else {
		cerr << "Parameters: " << endl;
		cerr << "0) raw file name" << endl;
		return 0;
	}
	
	string configFileName = ctarta + "/share/packetlibdemo/ctacamera_all.xml";
	char* filename = argv[1];
	
	//Objects that manages a stream of byte
	InputPacketStream* ips = 0;
	
	try
	{
		//create output packet stream - configFileName is the name of the xml file
		ips = new InputPacketStream(configFileName.c_str());

		// Create and open an output device: file
		Input* in = (Input*) new InputFile(ips->isBigEndian());
		vector<string> param;
		param.push_back(filename);
		in->openDevice(param); /// open input
		
		// connect the input packet stream with the input device
		ips->setInput(in);
	}
	catch (PacketException* e)
	{
		cout << "Error on stream constructor: ";
		cout << e->geterror() << endl;
	}
	
	unsigned long totbytes = 0;
	
	//decode the data
	long nops = 0;
	try
	{
		//decode for routing
		Packet* p = ips->getPacketType("CTA-CAM");
		byte ctaCamId = p->getPacketID();
		while(p = ips->readPacket()) {
			nops++;
			if(p->getPacketID() == PACKETNOTRECOGNIZED)
				cout << "Packet not recognized" << endl;
			//search only CTA-CAM packets
			if(p->getPacketID() == ctaCamId) {
				//packet recognized, do something, e.g.
				
				//get the size of the packet
				dword packetSize = p->size();
				totbytes += packetSize;
				
				//e.g. get the camera data for data analysis or for storage
				
				//get information from the packet: number of pixels and samples, trigger time, event number, packet length
				int npixels =   p->getPacketSourceDataField()->getFieldValue("Number of pixels");
				int nsamples =  p->getPacketSourceDataField()->getFieldValue("Number of samples");
				dword times =   p->getPacketDataFieldHeader()->getFieldValue_32i("Ttime:secs");
				dword timensn = p->getPacketDataFieldHeader()->getFieldValue_32ui("Ttime:nsecs");
				dword evetnum = p->getPacketSourceDataField()->getFieldValue_32ui("eventNumber");
				cout << npixels << " " << nsamples << " " << times << " " << evetnum << endl;
				cout << p->getPacketHeader()->getPacketLength() << endl;
				
				//version 1 - fast
				//get the array of camera data - if packet is compressed, decompress them in a transparent way
				
				ByteStreamPtr cameraDataStream = p->getData();
				cameraDataStream->swapWordForIntel(); //take into account the endianity
				word* cameraData = (word*)cameraDataStream->stream;
				
				//process the camera data
				for(word pixel=0; pixel<npixels; pixel++) {
					for(word sample=0; sample<nsamples; sample++) {
						cout << cameraData[pixel*nsamples + sample] << " ";
					}
					cout << endl;
				}
				
				//version 2 - slow
				/*
				if(p->isCompressed()) p->decompress();
				for(word pixel=0; pixel<npixels; pixel++) {
					for(word sample=0; sample<nsamples; sample++) {
						cout << p->getPacketSourceDataField()->getBlock(pixel)->getFieldValue(sample) << " ";
					}
					cout << endl;
				}
				*/
				
			}
		}

	}
	catch (PacketException* e)
	{
		cout << "Error on stream constructor: ";
		cout << e->geterror() << endl;
	}


}
