#pragma once
#include "Converters.h"
#include "FileNames.h"

int main(int argc, char* argv[]) {

	std::filesystem::create_directory("./output");
	std::vector<String> inputs;
	if (getFileNames("./input", inputs)) {
		for (String filename : inputs) {
			convert_isekai(filename, 3, 4);
		}
	}

	return 0;
}
