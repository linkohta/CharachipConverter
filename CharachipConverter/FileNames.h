#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>

/**
* @brief フォルダ以下のファイル一覧を取得する関数
* @param[in]    folderPath  フォルダパス
* @param[out]   file_names  ファイル名一覧
* return        true:成功, false:失敗
*/
bool getFileNames(std::string folderPath, std::vector<std::string>& file_names);