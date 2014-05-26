#include "packetlibop.h"



void endMiB(bool exitprg, struct timespec start, unsigned long totbytes) {
	struct timespec stop;
	clock_gettime( CLOCK_MONOTONIC, &stop);
	double time = timediff(start, stop);
	//std::cout << "Read " << ncycread << " ByteSeq: MB/s " << (float) (ncycread * Demo::ByteSeqSize / time) / 1048576 << std::endl;
	cout << "Result: it took  " << time << " s" << endl;
	cout << "Result: rate: " << setprecision(10) << totbytes / 1000000 / time << " MiB/s" << endl;
	cout << totbytes << endl;
	if(exitprg) exit(1);
}

void endHertz(bool exitprg, struct timespec start, unsigned long totbytes, unsigned long nops) {
	struct timespec stop;
	clock_gettime( CLOCK_MONOTONIC, &stop);
	double time = timediff(start, stop);
	//std::cout << "Read " << ncycread << " ByteSeq: MB/s " << (float) (ncycread * Demo::ByteSeqSize / time) / 1048576 << std::endl;
	cout << "Result: it took  " << time << " s" << endl;
	cout << "Result: rate: " << setprecision(10) << nops / time << " Hertz" << endl;
	cout << "Result: rate: " << setprecision(10) << totbytes / 1000000 / time << " MiB/s" << endl;
	cout << totbytes << endl;
	if(exitprg) exit(1);
}




unsigned long encodeNotZeroSuppressedPixelsSlow(OutputPacketStream* ops, int neventstowrite) {
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
				
		for(int i=0; i<neventstowrite; i++) {
			
			//store some informations on the headers
			packet_datafieldheader->setFieldValue_32ui("Ttime:secs", timetrigger++);
			packet_datafieldheader->setFieldValue_32ui("Ttime:nsecs", rand());
			packet_sdf->setFieldValue_32ui("eventNumber", i);
			
			//store the information of the pixels
			
			
			//use the packetlib to set each single field: very slow
			for(word pixel=0; pixel<numberOfCameraPixels; pixel++)
				for(word sample=0; sample<numPixelSamples; sample++) {
					word sampleValue = sample;
					packet_sdf->getBlock(pixel)->setFieldValue(sample, sampleValue);
				}
			
			//encode the packet
			p->encode();
				
						
			//write the encoded packet to output
			ops->writePacket(p);
			
			//get the size of the packet (only for measurement of performances)
			dword packetSize = p->size();
			totbytes += packetSize;
			
			//NEXT STEP COMPRESSION
		}
		
	}
	catch (PacketException* e)
	{
		cout << "Error during encoding: ";
		cout << e->geterror() << endl;
	}
	return totbytes;
}

unsigned long encodeNotZeroSuppressedPixels(OutputPacketStream* ops, int neventstowrite, bool compress) {
	
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
			p->getPacketHeader()->setFieldValue("Packet Type", 2);
			p->getPacketHeader()->setFieldValue("Packet Subtype", 40);
			packet_datafieldheader->setFieldValue_32ui("Ttime:secs", timetrigger++);
			packet_datafieldheader->setFieldValue_32ui("Ttime:nsecs", rand());
			packet_sdf->setFieldValue_32ui("eventNumber", i);
			
			
			//use the packetlib to set an array of pixels into the stream
			
			//1) set the array
			for(word pixel=0; pixel<numberOfCameraPixels; pixel++)
				for(word sample=0; sample<numPixelSamples; sample++) {
					word sampleValue = rand() % 100 + 50;
					cameraData[pixel][sample] = sampleValue;
				}
			
			//2) create a ByteStream that manage the array. Cast the camera data to an array of bytes
			ByteStreamPtr sourcedata = ByteStreamPtr(new ByteStream((byte*) cameraData, cameraDataSize, ops->isBigEndian()));
			//3) encode the packet with the ByteStream of camera data
			p->encodeAndSetData(sourcedata);
			
			//4) compress the data
			if(compress)
				p->compressData(LZ4, 4);
			
			//write the encoded packet to output
			ops->writePacket(p);
			
			//get the size of the packet (only for measurement of performances)
			dword packetSize = p->size();
			totbytes += packetSize;
			
		}
		
	}
	catch (PacketException* e)
	{
		cout << "Error during encoding: ";
		cout << e->geterror() << endl;
	}
	return totbytes;
}



unsigned long encodeZeroSuppressedPixels(OutputPacketStream* ops, int neventstowrite, bool compress) {
	
	unsigned long totbytes = 0;
	
	try {
		//get a packet to encode the data of a camera that manage 30 samples for each pixel
		Packet* p = ops->getPacketType("CHEC-CAM");
		
		//get the sections of a packet
		PacketHeader* packet_header = p->getPacketHeader();
		DataFieldHeader* packet_datafieldheader = p->getPacketDataFieldHeader();
		SourceDataField* packet_sdf = (SourceDataField*) p->getPacketSourceDataField();
		
		//build the dictionary
		word numPixelSamples = packet_sdf->getFieldValue("Number of samples");
		word indexBlockPixels = 0;
		word indexBlockPixelsId = 1;
		
		
		//time of trigger
		unsigned long timetrigger = 100000;
		
		//an array of data of pixels
		
		
		for(int i=0; i<neventstowrite; i++) {
			
			word numberOfCameraPixels = rand()%2048;
			//set the number of pixel stored in the first block section that contains the samples
			packet_sdf->setNumberOfBlocks(numberOfCameraPixels, indexBlockPixels);
			//set the number of pixel stored in the second block section that contains the id of the pixel
			packet_sdf->setNumberOfBlocks(numberOfCameraPixels, indexBlockPixelsId);
			
			//store some informations on the headers
			packet_datafieldheader->setFieldValue_32ui("Ttime:secs", timetrigger++);
			packet_datafieldheader->setFieldValue_32ui("Ttime:nsecs", rand());
			packet_sdf->setFieldValue_32ui("eventNumber", i);
			
			
			//use the packetlib to set an array of pixels into the stream
			word cameraData[numberOfCameraPixels][numPixelSamples];
			dword cameraDataSize = numberOfCameraPixels*numPixelSamples*sizeof(word);
			
			//1) set the array of samples
			for(word pixel=0; pixel<numberOfCameraPixels; pixel++) {
				for(word sample=0; sample<numPixelSamples; sample++) {
					word sampleValue = sample;
					cameraData[pixel][sample] = sampleValue;
				}
				//2) set the pixel ID
				word pixelID = rand()%2048;
				packet_sdf->getBlock(pixel, indexBlockPixelsId)->setFieldValue(0, pixelID);
			}

			//2) create a ByteStream that manage the array. Cast the camera data to an array of bytes
			ByteStreamPtr sourcedata = ByteStreamPtr(new ByteStream((byte*) cameraData, cameraDataSize, ops->isBigEndian()));
			//3) encode the packet with the ByteStream of camera data
			p->encodeAndSetData(sourcedata);
			
			//4) compress the data
			if(compress)
				p->compressData(LZ4, 4);
			
			//write the encoded packet to output
			ops->writePacket(p);
			
			//get the size of the packet (only for measurement of performances)
			dword packetSize = p->size();
			totbytes += packetSize;
			
		}
		
	}
	catch (PacketException* e)
	{
		cout << "Error during encoding: ";
		cout << e->geterror() << endl;
	}
	return totbytes;
}

