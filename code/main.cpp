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




#include "packetlibop.h"





unsigned long encodeNotZeroSuppressedPixels(OutputPacketStream* ops, int neventstowrite) {
	
	unsigned long totbytes = 0;
	
	try {
		//get a packet to encode the data of a camera that manage 30 samples for each pixel
		Packet* p = ops->getPacketType("CHEC-CAM");
		
		//get the sections of a packet
		PacketHeader* packet_header = p->getPacketHeader();
		DataFieldHeader* packet_datafieldheader = p->getPacketDataFieldHeader();
		SourceDataField* packet_sdf = (SourceDataField*) p->getPacketSourceDataField();
		
		//get the number of samples managed by the packet
		word numPixelSamples = packet_sdf->getFieldValue("Number of samples");
		
		word numberOfCameraPixels = 2048;
		packet_sdf->setNumberOfBlocks(numberOfCameraPixels);
		
		//time of trigger
		unsigned long timetrigger = 100000;
		
		//an array of data of pixels
		word cameraData[numberOfCameraPixels][numPixelSamples];
		dword cameraDataSize = numberOfCameraPixels*numPixelSamples*sizeof(word);
		
		for(int i=0; i<neventstowrite; i++) {
			
			//store some informations on the headers
			packet_datafieldheader->setFieldValue_32ui("Ttime:secs", timetrigger++);
			packet_datafieldheader->setFieldValue_32ui("Ttime:nsecs", rand());
			packet_sdf->setFieldValue_32ui("eventNumber", i);
			
			
			//use the packetlib to set an array of pixels into the stream
			
			//1) set the array
			for(word pixel=0; pixel<numberOfCameraPixels; pixel++)
				for(word sample=0; sample<numPixelSamples; sample++) {
					word sampleValue = sample;
					cameraData[pixel][sample] = sampleValue;
				}
			
			//2) create a ByteStream that manage the array. Cast the camera data to an array of bytes
			ByteStreamPtr sourcedata = ByteStreamPtr(new ByteStream((byte*) cameraData, cameraDataSize, ops->isBigEndian()));
			//3) encode the packet with the ByteStream of camera data
			p->encodeAndSetData(sourcedata);
			
			p->compressData(LZH, 4);
		
			//write the encoded packet to output
			ops->writePacket(p);
			
			//get the size of the packet (only for measurement of performances)
			dword packetSize = p->size();
			totbytes += packetSize;
		
		}

	}
	catch (PacketException* e)
	{
		cout << "Error oduring encoding: ";
		cout << e->geterror() << endl;
	}
	return totbytes;
}

int main(int argc, char *argv[]) {
	
	unsigned long totbytes = 0;
	unsigned long nops = 0;
	
	struct timespec start;
	
	if(argc < 3) {
		cout << "Parameters: encode(0)/decode(1) file.raw " << endl;
		cout << "0 encoding" << endl;
		exit(0);
	}
	
	
	string ctarta;
	
	
	
	if(argc > 1) {
		const char* home = getenv("CTARTA");
		if (!home)
		{
			std::cerr << "ERROR: CTARTA environment variable is not defined." << std::endl;
			return 0;
		}
		
		ctarta = home;
		
	} else {
		cerr << "ERROR: Please, provide the .raw" << endl;
		return 0;
	}
	
	string configFileName = ctarta + "/share/packetlibdemo/ctacamera_all.stream";
	
	int operation = atoi(argv[1]);
	char* filename = argv[2];
	
	//Objects that manages a stream of byte
	OutputPacketStream* ops = 0;
	InputPacketStream* ips = 0;
	
	try
	{
		cout << configFileName << endl;
		
		//load the configuration files
		if(operation == 0)
		{
			// create output packet stream
			ops = new OutputPacketStream();
			ops->setFileNameConfig(configFileName.c_str());
			ops->createStreamStructure();

			// Create and open an output device: file
			Output* out;
			
			out = (Output*) new OutputFile(ops->isBigEndian());
			char** param = (char**) new char*[2];
			param[0] = (char*) filename; // file name
			param[1] = 0;
			out->open(param);
			
			/*
			out = (Output*) new OutputSocketClient(ops->isBigEndian());
			cout << "OUTPUT: SOCKET CLIENT 10001" << endl;
			char** param = (char**) new char* [3];
			param[0] = "localhost"; //host
			param[1] = "10001"; //port
			param[2] = 0;
			*/
			// connect the output packet stream with the output
			ops->setOutput(out);
		}
		if(operation >= 1)
		{
			// create input packet stream
			ips = new InputPacketStream();
			ips->setFileNameConfig(configFileName.c_str());
			ips->createStreamStructure();
			
			// Create and open an input device: file
			Input* in;
			//2) create input
			
			in = (Input*) new InputFile(ips->isBigEndian());
			char** param = (char**) new char*[2];
			param[0] = (char*) filename; // file name
			param[1] = 0;
			in->open(param); /// open input
			
			
			/*
			in = (Input*) new InputSocketServer(ips->isBigEndian());
			cout << "INPUT: SOCKET SERVER 10001" << endl;
			char** param = (char**) new char* [2];
			param[0] = "10001"; //port
			param[1] = 0;
			*/
			
			// connect the input packet stream with the input device
			ips->setInput(in);
			
		}
	}
	catch (PacketException* e)
	{
		cout << "Error on stream constructor: ";
		cout << e->geterror() << endl;
	}

	//encoding
	clock_gettime( CLOCK_MONOTONIC, &start);
	
	srand(1);
	if(operation == 0) {
		//the second parameter is the number of triggered event to write into files
		totbytes = encodeNotZeroSuppressedPixels(ops, 1000);
		endMiB(true, start, totbytes);
	}
	
	
	if(operation >= 1) {
		try {
			if(operation == 1)
				cout << "Test for routing" << endl;
			if(operation == 2)
				cout << "Get the array of camera data" << endl;
			if(operation == 3)
				cout << "Get the array of camera data and additional data" << endl;
			//get the packet
			Packet* p = ips->getPacketType("CHEC-CAM");
			
			//build the dictionary of packets that should be recognized by the InputPacketStream
			byte checCamId = p->getPacketID();
			int indexNPixels = p->getPacketSourceDataField()->getFieldIndex("Number of pixels");
			int indexNSamples = p->getPacketSourceDataField()->getFieldIndex("Number of samples");
			int indexEventNumber = p->getPacketSourceDataField()->getFieldIndex("eventNumber");
			int indexTimes = p->getPacketDataFieldHeader()->getFieldIndex("Ttime:secs");
			int indexTimens = p->getPacketDataFieldHeader()->getFieldIndex("Ttime:nsecs");
			
			//decode for routing
			while(ips->readPacket()) {
				nops++;
				if(p->getPacketID() == PACKETNOTRECOGNIZED)
					cout << "Packet not recognized" << endl;
				//search only CHEC-CAM packets
				if(p->getPacketID() == checCamId) {
					PacketHeader* packet_header = p->getPacketHeader();
					DataFieldHeader* packet_datafieldheader = p->getPacketDataFieldHeader();
					SourceDataField* packet_sdf = (SourceDataField*) p->getPacketSourceDataField();
					
					//packet recognized, do something, e.g.
					
					//get the size of the packet
					dword packetSize = p->size();
					totbytes += packetSize;
					
					if(operation == 2) {
						//e.g. get the camera data to send the packet to a process for data analysis or for storage
						int npixels = p->getPacketSourceDataField()->getFieldValue(indexNPixels);
						ByteStreamPtr cameraData = p->getData();
						
						//do something
					}
					if(operation == 3) {
						//e.g. get the camera data to send the packet to a process for data analysis or for storage
						int npixels =  packet_sdf->getFieldValue(indexNPixels);
						int nsamples =  packet_sdf->getFieldValue(indexNSamples);
						dword times =  packet_datafieldheader->getFieldValue_32i(indexTimes);
						dword timensn = packet_datafieldheader->getFieldValue_32ui(indexTimens);
						dword eventnum = packet_sdf->getFieldValue_32ui(indexEventNumber);
						
						//get the array of camera data
						ByteStreamPtr cameraDataBS = p->getData();
						
						cameraDataBS->swapWordForIntel(); //take into account the endianity
						
						cout << cameraDataBS->size() << " " << p->isCompressed() << " " << p->getCompressionAlgorithm() << " " << p->getCompressionLevel() <<  endl;
						
						
						ByteStreamPtr cameraDataDecompressed = cameraDataBS;
						
						//if(p->isCompressed())
						//	cameraDataDecompressed = p->decompressData();
						
						//do something with camera data
						word* cameraData = (word*)cameraDataDecompressed->stream;
						
						/*
						for(word pixel=0; pixel<npixels; pixel++) {
							for(word sample=0; sample<nsamples; sample++) {
								cout << cameraData[pixel*nsamples + sample] << " ";
							}
							cout << endl;
						}
						*/
					}
				}
			}

			endHertz(true, start, totbytes, nops);
		}
		catch (PacketException* e)
		{
			cout << "Error in decoding for routing: ";
			cout << e->geterror() << endl;
		}
	}
	
	
	//delete ips;
	//delete ops;

}
