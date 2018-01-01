#include <iostream>
#include <stdlib.h>

#include "imgarc.h"
#include "archiver.h"

namespace imgarc
{
	void usage()
	{
		std::cout << "Usage: imgarc original_image.jpg file.txt [another_file.txt]" << std::endl;
		exit(1);
	}
}

int main(const int argc, const char *argv[])
{
	if (argc < 3)
		imgarc::usage();

	imgarc::imgdata session;
	session.img_fp = argv[1];
	session.binary_file_fp = argv[2];
	session.output_fp = session.img_fp + ".imgarc.jpg";

	if (! imgarc::validate_session(session))
		return 1;

	imgarc::process(session);

	return 0;
}
