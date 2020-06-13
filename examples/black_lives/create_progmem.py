#!/usr/bin/python
import binascii
import sys
import glob, os
import pdb
file_no=0;
file_names=[];
RGB565=1;
out_string="";

def printrgb565(red, green, blue):
	x1 = (red & 0xF8) | (green >> 5);
	x2 = ((green & 0x1C) << 3) | (blue  >> 3);
	#pdb.set_trace()
	this_string="0x" + str(binascii.hexlify(chr(x2))) + ",";
	this_string+="0x" + str(binascii.hexlify(chr(x1))) + ",";
	return this_string;

def printrgb888(red, green, blue):
	this_string="0x" + str(binascii.hexlify(red)) + ",";
	this_string+="0x" + str(binascii.hexlify(green)) + ",";
	this_string+="0x" + str(binascii.hexlify(blue)) + ",";
	return this_string;

out_string="uint8_t animation_lengths[]={";
for file in glob.glob("*.rgb"):
	file_no=file_no+1;
	file_names.append(str(file))
	size = os.path.getsize(str(file))/64/32/3
	out_string+=str(size)+ ",";
out_string=out_string[:-1];
out_string+="};\nconst uint8_t animations[] PROGMEM = {";
print (out_string)
byte_count=0;
for file_name in file_names:
	size = os.path.getsize(str(file_name))
        print(str(file_name)+ "- source_size: " + str(size));

	with open(file_name, 'rb') as f:
		byte0 = f.read(1)
		while byte0 != "":
			byte1 = f.read(1)
			byte2 = f.read(1)
			# Do stuff with byte.
                        if (RGB565):
				out_string+=printrgb565(ord(byte0), ord(byte1), ord(byte2))
				byte_count=byte_count+2;
			else:
				out_string+=printrgb888(byte0, byte1, byte2,out_string)
				byte_count=byte_count+3;
			if ((byte_count%10)==0):
				out_string+="\n";
                        byte0 = f.read(1)
        #print(str(file_name)+ "- out_size: " + str(byte_count));

                        
out_string+="0x00};";

out_file = open("anim_data.h", "w");

out_file.write(out_string);
out_file.close();
