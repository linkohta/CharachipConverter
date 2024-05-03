#pragma once
#include <iostream>
#include "Converters.h"
#include "FileNames.h"

using namespace std;

int main(int argc, char* argv[]) {

	const vector<String> MODES = { "i", "b", "f"};

	vector<String> inputs, outputs;
	if (getFileNames("./input", inputs)) {
		if (inputs.size() == 0) {
			cout << "./inputに変換元ファイルがありません。続行するにはキーを押してください...";
			int i = getchar();
			return 0;
		}
		String mode;
		cout << "変換モードを選択('i':異世界の創造者, 'b':RPG Developer Bakin, 'f':異世界の創造者 フェイス)";
		cin >> mode;
		if (find(MODES.begin(), MODES.end(), mode) == MODES.end()) {
			cout << "入力したモードは存在しません。続行するにはキーを押してください...";
			int i = getchar();
			return 0;
		}
		for (String filename : inputs) {
			if (mode == "i") {
				convert_isekai(filename, 3, 4);
			}
			else if (mode == "b") {
				convert_bakin(filename, 1, 4);
			}
			else if (mode == "f") {
				convert_isekai_face(filename, 4, 2);
			}
		}
	}
	else {
		cout << "./inputに変換元ファイルがありません。続行するにはキーを押してください...";
		int i = getchar();
	}

	return 0;
}
