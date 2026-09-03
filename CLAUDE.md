# CharachipConverter

## 概要

RPGツクールVX/VXAceのキャラチップ素材を、RPG Developer Bakinまたは異世界の創造者向けのキャラチップ形式に変換するWindows向けGUIツール。C++とOpenCVで実装されたネイティブWin32アプリケーション。

## 技術構成

- 言語: C++17
- GUI: Win32 API（ネイティブウィンドウ、MFCやWPF等のフレームワークは未使用）
- 画像処理: OpenCV（`opencv_world460`）
- ビルド: Visual Studio ソリューション（`CharachipConverter.sln`）、プラットフォームツールセット v143、x64/Win32構成あり
- OpenCVは `C:\opencv\build` にインストールされている前提でインクルードパス・ライブラリパスが設定されている（`CharachipConverter.vcxproj` 参照）

## 開発ルール

- 機能改修等を行うときは、必ずブランチを切って作業すること。

## ファイル構成

- `CharachipConverter/main.cpp` — Win32 GUIのエントリポイント（`wWinMain`）。ウィンドウ作成、入出力フォルダの選択（`SHBrowseForFolderW`）、変換モードのラジオボタン、変換実行、ログ表示（リストボックス）を担当
- `CharachipConverter/Converters.h` / `Converters.cpp` — 実際の画像分割・変換ロジック（`convert_bakin`, `convert_isekai`, `convert_isekai_face`, `PinP_tr`）。ログ出力は `LogFunc`（`std::function<void(const std::string&)>`）をコールバックとして受け取り、GUIのログ欄とCLIの標準出力の両方に対応できる設計
- `CharachipConverter/FileNames.h` / `FileNames.cpp` — 指定フォルダ直下のファイル一覧を取得する `getFileNames`
- `bin/` — コンパイル済み実行ファイルと `input`/`output` フォルダを配置する場所

## 変換モード

- `i`（異世界の創造者）: `convert_isekai`, 3列×4行分割
- `b`（RPG Developer Bakin）: `convert_bakin`, 1列×4行分割
- `f`（異世界の創造者 フェイス）: `convert_isekai_face`, 4列×2行分割から1マス切り出し

各変換関数は出力先フォルダをパラメータとして受け取る（ハードコードされた `./output` 固定ではない）。

## ビルド・検証について

この開発環境にはOpenCVがインストールされていないため、Bashエージェントからのフルビルド検証はできない。コード変更後にビルド確認が必要な場合は、OpenCVとVisual Studio（v143ツールセット）がセットアップされた環境でMSBuildを実行するか、Visual Studio上でビルドすること。

## GUI化の経緯

元は `std::cin`/`std::cout` を使うコンソールアプリだった。現在はWin32ネイティブウィンドウのGUIアプリ（`SubSystem=Windows`）に変更されている。CLIの対話フローは廃止し、フォルダ選択・モード選択・実行をすべてGUI操作で行う。
