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
#include <packet/OutputPacketStream.h>
#include <packet/OutputFile.h>
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
	
	if(argc == 4) {
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
		cerr << "1) nevents to write" << endl;
		cerr << "2) compression level (0 = do not compress)" << endl;
		return 0;
	}
	
	string configFileName = ctarta + "/share/packetlibdemo/ctacamera_all.xml";
	char* filename = argv[1];
	int nevents = atoi(argv[2]);
	int compress = atoi(argv[3]);
	
	//Objects that manages a stream of byte
	OutputPacketStream* ops = 0;
	
	try
	{
		//create output packet stream - configFileName is the name of the xml file
		ops = new OutputPacketStream(configFileName.c_str());

		// Create and open an output device: file
		Output* out = (Output*) new OutputFile(ops->isBigEndian());
		vector<string> param;
		param.push_back(filename);
		out->openDevice(param);
		
		// connect the output packet stream with the output
		ops->setOutput(out);
	}
	catch (PacketException* e)
	{
		cout << "Error on stream constructor: ";
		cout << e->geterror() << endl;
	}
	
	unsigned long totbytes = 0;
	
	//encode the data
	try
	{
		//get a packet to encode the data of a camera that manage 40 samples for each pixel
		Packet* p = ops->getPacketType("CTA-CAM");
		
		//get the number of samples managed by the packet
		word numPixelSamples = p->getPacketSourceDataField()->getFieldValue("Number of samples");
		
		//set the number of pixel managed by the packet
		word numberOfCameraPixels = 2048;
		p->getPacketSourceDataField()->setNumberOfBlocks(numberOfCameraPixels);
		
		//time of trigger
		unsigned long timetrigger = 100000;
		
		for(int i=0; i<nevents; i++) {
			
			//store some informations on the headers
			p->getPacketDataFieldHeader()->setFieldValue_32ui("Ttime:secs", timetrigger++);
			p->getPacketDataFieldHeader()->setFieldValue_32ui("Ttime:nsecs", rand());
			p->getPacketSourceDataField()->setFieldValue_32ui("eventNumber", i);
			
			//store the information of the pixels
			
			//use the packetlib to set each single field: very slow
			for(word pixel=0; pixel<numberOfCameraPixels; pixel++)
				for(word sample=0; sample<numPixelSamples; sample++) {
					//word sampleValue = rand() % 100 + 50;
					word sampleValue = sample;
					p->getPacketSourceDataField()->getBlock(pixel)->setFieldValue(sample, sampleValue);
				}
			
			//encode the packet
			p->encode();
			
			//compress the data
			if(compress) p->compressData(LZ4, compress);
			
			//write the encoded packet to output
			ops->writePacket(p);
			
			//get the size of the packet (only for measurement of performances)
			dword packetSize = p->size();
			totbytes += packetSize;
			
		}

	}
	catch (PacketException* e)
	{
		cout << "Error on stream constructor: ";
		cout << e->geterror() << endl;
	}


}
