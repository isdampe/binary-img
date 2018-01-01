#include <iostream>
#include <fstream>
#include <string>
#include "imgarc.h"

namespace imgarc
{
	bool validate_session(imgdata session)
	{
		std::ifstream infile(session.img_fp);
		std::ifstream binfile(session.binary_file_fp);

		if (! infile.good())
		{
			std::cerr << "The image file \"" << session.img_fp << "\" could not" \
				" be located or accessed." << std::endl;
			return false;
		}

		if (! binfile.good())
		{
			std::cerr << "The binary file \"" << session.img_fp << "\" could not" \
				" be located or accessed." << std::endl;
			return false;
		}

		return true;
	}

	void process(imgdata session)
	{
		char *buffer = new char[1];
		int progress = 0;
		long long eo_jpg = -1;

		char *end_prog = new char[2];
		end_prog[0] = (char)0xFF;
		end_prog[1] = (char)0xD9;

		//Open the image.
		std::ifstream input(session.img_fp, std::ios::binary);
		input.seekg(0, input.end);
		unsigned long long length = input.tellg();
		unsigned long long sk = length - (length / 8);

		while (sk < length)
		{
			input.seekg(sk);
			input.read(buffer, 1);
			if (buffer[0] == end_prog[progress])
			{
				++progress;
				if (progress > 1)
				{
					eo_jpg = sk;
					break;
				}
			}
			else
				if (progress > 0)
					progress = 0;

			++sk;
		}

		if (eo_jpg < 0)
		{
			std::cerr << "Could not find end of JPG signature 0xFF 0xD9." << std::endl;
			return;
		}

		std::cout << "Found end of JPG data at 0x" << std::hex << eo_jpg << std::endl; 

		//Read and write jpg file from 0 to eo_jpg
		std::ofstream output(session.output_fp, std::ios::binary | std::ios::out);
		sk = 0;

		std::cout << "Writing JPG binary data from 0x0 to 0x" << std::hex << eo_jpg <<std::endl;
		while (sk<=eo_jpg)
		{
			input.seekg(sk);
			input.read(buffer, 1);
			output.write(buffer, 1);
			++sk;
		}

		//Then read and write binary
		std::ifstream binput(session.binary_file_fp, std::ios::binary);
		sk = 0;
		binput.seekg(0, binput.end);
		unsigned long long binary_length = binput.tellg();

		std::cout << "Writing binary file data from 0x" << std::hex << (eo_jpg +1) << std::endl;
		while (sk<=binary_length)
		{
			binput.seekg(sk);
			binput.read(buffer, 1);
			output.write(buffer, 1);
			++sk;
		}

		//Write the end signature.
		std::cout << "Writing end of imgarc signature at 0x" << std::hex << (eo_jpg + sk) << std::endl;
		output.write("!EOIMGARC", 9);

		std::cout << "Writing remaining JPG meta data at 0x" << std::hex << (eo_jpg + sk + 9) << std::endl;

		//Then read and write end of jpg file 
		sk = eo_jpg +2;
		while (sk<=length)
		{
			input.seekg(sk);
			input.read(buffer, 1);
			output.write(buffer, 1);
			++sk;
		}
		

		output.close();
		input.close();
		binput.close();

		std::cout << "Process complete. Output written to " << session.output_fp << std::endl;
		std::cout << "Unpack the binary file again with \"imgarc -d " << session.output_fp << "\"" << std::endl;

	}
}
