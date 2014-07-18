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

#define COMPRESSIONLEVEL 1


void swap(byte* stream, dword dim) {
	
	for(dword i = 0; i< dim; i+=2)
	{
		/// For odd dimensions
		if((dim - i) != 1)
		{
			byte btemp = stream[i];
			stream[i] = stream[i+1];
			stream[i+1] = btemp;
		}
	}
}

dword getdword(dword value, bool streamisbigendian) {
	if(streamisbigendian)
		return value;
	else {
		
		dword tmp = value << 16;
		//cout << tmp << endl;
		tmp += value >> 16;
		//cout << tmp << endl;
		return tmp;
	}
}

struct CTAPacketHeaders {
	word idAndAPID;
	word ssc;
	dword packetLength;
	word crcTypeAndSubtype;
	word compression;
};

struct CTADataHeaders {
	dword times;
	dword timens;
	word arrayID;
	word runNumber;
	dword eventNumber;
	word telescopID;
	word arrayTriggerData;
	word npixels;
	word nsamples;
	word npixelsID;
};




int main(int argc, char *argv[]) {
	
	unsigned long totbytes = 0;
	unsigned long nops = 0;
	
	struct timespec start;
	
	if(argc < 3) {
		cout << "Parameters: operation file.raw nevents " << endl;
		cout << "0 encoding not zero suppressed camera (not compressed)" << endl;
		cout << "1 encoding not zero suppressed camera (compressed)" << endl;
		//cout << "2 encoding zero suppressed camera (not compressed)" << endl;
		//cout << "3 encoding zero suppressed camera (compressed)" << endl;
		cout << "4 decoding for routing" << endl;
		cout << "5 decoding and get the array of camera data" << endl;
		cout << "6 decoding and get the array of camera data, additional data in the headers + decompression of data (if compressed)";
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
	
	string configFileName = ctarta + "/share/packetlibdemo/ctacamera_all.xml";
	
	int operation = atoi(argv[1]);
	char* filename = argv[2];
	int nevents = atoi(argv[3]);
	
	//Objects that manages a stream of byte
	OutputPacketStream* ops = 0;
	InputPacketStream* ips = 0;
	
	try
	{
		cout << configFileName << endl;
		
		//load the configuration files
		if(operation <= 3 )
		{
			cout << "create output packet stream" << endl;
			ops = new OutputPacketStream();
			ops->setFileNameConfig(configFileName.c_str());
			ops->createStreamStructure();

			// Create and open an output device: file
			Output* out;
			
			out = (Output*) new OutputFile(ops->isBigEndian());
			vector<string> param;
			param.push_back(filename);
			out->openDevice(param);
			
			/*
			out = (Output*) new OutputSocketClient(ops->isBigEndian());
			vector<string> param;
			param.push_back("localhost");
			param.push_back("10001");
			out->openDevice(param);
			*/
			// connect the output packet stream with the output
			ops->setOutput(out);
		}
		if(operation > 3 && operation < 10)
		{
			//cout << "create input packet stream" << endl;
			ips = new InputPacketStream();
			ips->setFileNameConfig(configFileName.c_str());
			ips->createStreamStructure();
			
			// Create and open an input device: file
			Input* in;
			//2) create input
			
			in = (Input*) new InputFile(ips->isBigEndian());
			vector<string> param;
			param.push_back(filename);
			in->openDevice(param); /// open input
			
			
			/*
			in = (Input*) new InputSocketServer(ips->isBigEndian());
			cout << "INPUT: SOCKET SERVER 10001" << endl;
			vector<string> param;
			param.push_back("10001");
			in->openDevice(param); /// open input
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
		totbytes = encodeNotZeroSuppressedPixels(ops, nevents, 0);
		endMiB(true, start, totbytes);
	}
	if(operation == 1) {
		//the second parameter is the number of triggered event to write into files
		totbytes = encodeNotZeroSuppressedPixels(ops, nevents, COMPRESSIONLEVEL);
		endMiB(true, start, totbytes, filename);
	}
	if(operation == 2) {
		//the second parameter is the number of triggered event to write into files
		totbytes = encodeZeroSuppressedPixels(ops, nevents, 0);
		endMiB(true, start, totbytes);
	}
	if(operation == 3) {
		//the second parameter is the number of triggered event to write into files
		totbytes = encodeZeroSuppressedPixels(ops, nevents, COMPRESSIONLEVEL);
		endMiB(true, start, totbytes, filename);
	}
	if(operation >= 4 && operation < 10) {
		try {
			if(operation == 4)
				cout << "4 decoding for routing" << endl;
			if(operation == 5)
				cout << "5 decoding and get the array of camera data" << endl;
			if(operation == 6)
				cout << "6 decoding and get the array of camera data, additional data in the headers + decompression of data (if compressed)" << endl;
			
			//get the packet
			Packet* p = ips->getPacketType("CTA-CAM");
			
			//build the dictionary of packets that should be recognized by the InputPacketStream
			byte ctaCamId = p->getPacketID();
			int indexNPixels = p->getPacketSourceDataField()->getFieldIndex("Number of pixels");
			int indexNSamples = p->getPacketSourceDataField()->getFieldIndex("Number of samples");
			int indexEventNumber = p->getPacketSourceDataField()->getFieldIndex("eventNumber");
			int indexTimes = p->getPacketDataFieldHeader()->getFieldIndex("Ttime:secs");
			int indexTimens = p->getPacketDataFieldHeader()->getFieldIndex("Ttime:nsecs");
			
			//decode for routing
			while(p = ips->readPacket()) {
				nops++;
				if(p->getPacketID() == PACKETNOTRECOGNIZED)
					cout << "Packet not recognized" << endl;
				//search only CTA-CAM packets
				if(p->getPacketID() == ctaCamId) {
					PacketHeader* packet_header = p->getPacketHeader();
					DataFieldHeader* packet_datafieldheader = p->getPacketDataFieldHeader();
					SourceDataField* packet_sdf = (SourceDataField*) p->getPacketSourceDataField();
					
					//packet recognized, do something, e.g.
					
					//get the size of the packet
					dword packetSize = p->size();
					totbytes += packetSize;
					
					if(operation == 5) {
						//e.g. get the camera data to send the packet to a process for data analysis or for storage
						int npixels = p->getPacketSourceDataField()->getFieldValue(indexNPixels);
						ByteStreamPtr cameraData = p->getData();
						
						//do something
					}
					if(operation == 6) {
						//e.g. get the camera data for data analysis or for storage
						
						//get information from the packet: number of pixels and samples, trigger time, event number, packet length
						int npixels =  packet_sdf->getFieldValue(indexNPixels);
						int nsamples =  packet_sdf->getFieldValue(indexNSamples);
						dword times =  packet_datafieldheader->getFieldValue_32i(indexTimes);
						dword timensn = packet_datafieldheader->getFieldValue_32ui(indexTimens);
						dword eventnum = packet_sdf->getFieldValue_32ui(indexEventNumber);
						//cout << npixels << " " << nsamples << " " << times << " " << eventnum << endl;
						//cout << p->getPacketHeader()->getPacketLength() << endl;
						
						//cout << cameraDataBS->size() << " " << p->isCompressed() << " " << p->getCompressionAlgorithm() << " " << p->getCompressionLevel() <<  endl;
						
						word* cameraData;
						
						//get the array of camera data - if packet is compressed, decompress them in a transparent way
						ByteStreamPtr cameraDataDecompressed = p->getData();
						cameraDataDecompressed->swapWordForIntel(); //take into account the endianity
						cameraData = (word*)cameraDataDecompressed->stream;;
						
						
						//do something with camera data, e.g.
						//process the camera data
						for(word pixel=0; pixel<npixels; pixel++) {
							for(word sample=0; sample<nsamples; sample++) {
								//cout << cameraData[pixel*nsamples + sample] << " ";
							}
							//cout << endl;
						}
						
						
					}
				}
			}
			
			endHertz(true, start, totbytes, nops, filename);
		}
		catch (PacketException* e)
		{
			cout << "Error in decoding for routing: ";
			cout << e->geterror() << endl;
		}
	}
	
	
	if(operation == 10) {
		cout << "no packetlib" << endl;
		//open the file with the data
		FILE* fp = fopen(filename, "r");
		//get the size of headers and allocate memory for headers
		word sizePacketHeader = 12;
		word sizeDataHeader = 26;
		byte* packetheader  = (byte*) new word[sizePacketHeader];
		byte* dataheader  = (byte*) new word[sizeDataHeader];
		//cout << sizePacketHeader << " " << sizeDataHeader << endl;
		size_t result;
		//manage the endianity
		bool streamisbigendian = false;
		long offset = 0;
		int pnum = 0;
		//read all packets of the file
		do {
			nops++;
			//--------------------------
			//1) read the packet header
			result = fread(packetheader, 1, sizePacketHeader, fp);
			
			if(result != sizePacketHeader)
				break;
			
			//move to the next block of data
			offset += sizePacketHeader;
			fseek(fp, offset, SEEK_SET);
			//swap data if streaming is bigendian
			if(streamisbigendian)
				swap(packetheader, result);
			
			//2) get the packet lenght
			CTAPacketHeaders* packetheaderstruct = (CTAPacketHeaders*) packetheader;
			dword packetLength = getdword(packetheaderstruct->packetLength, streamisbigendian) + 1;
			
			//--------------------------
			//3) read the data header
			result = fread(dataheader, 1, sizeDataHeader, fp);
			
			if(result != sizeDataHeader)
				break;
			
			offset += sizeDataHeader;
			fseek(fp, offset, SEEK_SET);
			//swap data if streaming is bigendian
			if(streamisbigendian)
				swap(dataheader, result);
			CTADataHeaders* dataheaderstruct = (CTADataHeaders*) dataheader;
			
			//--------------------------
			//4) calculate the dimension of the "data" section and allocate the memory
			dword sizeData = packetLength-sizeDataHeader;
			byte* data  = (byte*) new byte[sizeData];
			//read the data
			result = fread(data, 1, sizeData, fp);
			
			if(result != sizeData)
				break;
			
			//move to the next block of data
			offset += sizeData;
			fseek(fp, offset, SEEK_SET);
			//swap data if streaming is bigendian
			if(streamisbigendian)
				swap(data, result);
			
			//5) use the data of the camera
			word* cameraData = (word*) data;
			
			byte compression = packetheaderstruct->compression >> 8;
			
			//cout << pnum++ << " " << packetLength << " " << (int) compression << " "  << dataheaderstruct->npixels << " " << dataheaderstruct->nsamples << endl;
			if(compression) {
				//packet compressed - decompress the packet first
				/*
				for(dword i=0; i< sizeData/2; i++)
					cout << cameraData[i] << " ";
				cout << endl;
				*/
			} else {
				//packet not compressed
				
				for(word pixel=0; pixel<dataheaderstruct->npixels; pixel++) {
					for(word sample=0; sample<dataheaderstruct->nsamples; sample++) {
						//cout << cameraData[pixel*dataheaderstruct->nsamples + sample] << " ";
					}
					//cout << endl;
				}
				
			}
			
			delete[] data;
			
		} while (true);
		fclose(fp);
		endHertz(true, start, offset, nops);
	}
	
	//delete ips;
	//delete ops;

}
