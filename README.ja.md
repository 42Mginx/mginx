以下のマークダウンコンテンツを日本語に翻訳してください：

# mginx

<div align="center">
<img src="https://user-images.githubusercontent.com/54441505/171986791-972d5370-8d19-4abc-b98c-4d90a0c3320e.png" width="400" />
</div>

## ドキュメンテーション
* 英語と日本語の両方でドキュメンテーションを提供しています。
* [translateme](https://github.com/marketplace/actions/readme-ai-translator)を使用して、README.mdの変更を検出し、自動的に翻訳します。

|言語|パス|
|---|---|
| 英語  | [README.md](https://github.com/42Mginx/mginx/blob/main/README.md) |
| 日本語 | [README.ja.md](https://github.com/42Mginx/mginx/blob/main/README.ja.md) |


## ディレクトリ構造
|ディレクトリ|説明|
|---|---|
| **webserver** | ソケット通信部分 |
| **config**    | 設定ファイルの解析   |
| **request**   | リクエストの解析   |
| **response**  | レスポンスの生成  | 
| **cgi**       | CGIプログラムの実行   |
| **autoindex** | autoindex機能の実装   |
| **utils**     | ユーティリティ関数、構造体   |
| **client**    | テスト用クライアントプログラム   |

## MGINXの実行方法
0. このリポジトリをクローンし、ターミナルを開きます
```shell
> git clone https://github.com/42Mginx/mginx.git
> cd mginx
```
1. Makefileで実行します
```shell
> make
> ./webserv ./test/conf/mac_test.conf
```
- または、このように入力します
```shell
> make run
```
- 例 <成功したバインドのポート情報を表示>
```shell
0: 8000
Bind success [8000/3]
0: 8001
Bind success [8001/4]
0: 8002
Bind success [8002/5]
```
- ケース1: 全て成功

```shell
0: 8000
エラーナンバー48: Address already in use
Could not bind [8000]
0: 8001
Bind success [8001/4]
0: 8002
Bind success [8002/5]
```
- ケース2: 1つ失敗[8000]/ 2つ成功[8001, 8002]

2. ブラウザでテスト
- 成功したポートを使用してテストできます。
- 入力: `localhost:${port}`
```shell
localhost:8000
```
 - このようなページが表示されます：
<img width="1006" alt="Screen Shot 2022-06-24 at 6 35 39 PM" src="https://user-images.githubusercontent.com/54441505/175508270-7e1f4677-16d2-4e07-ba2c-cd65893d04ee.png">

## 実行コマンド
### make run
- webserverプログラムを作成するためにコンパイル
- プログラムを実行

### make test
- webserverプログラムとクライアントプログラムをコンパイルして実行
- 私たちが作ったクライアントプログラムを使用してテストできます：
<img width="682" alt="Screen Shot 2022-06-24 at 7 06 13 PM" src="https://user-images.githubusercontent.com/54441505/175513539-4e9a0ef8-a4a2-4760-9d8d-4ef4db3cd8d0.png">
   
   - オプションを選んでリクエストを作成できます
   - メソッド(GET/POST/DELETE)
   - オプション(シンプル/不正なリクエスト/CGI)
   - ポート(8000/8001)
   
### make tester
- webserverプログラムを実行
- 新しいターミナルウィンドウを開いてテスターを実行
- このコマンドをVSC環境で実行するには、以下の手順を守る必要があります：   
   - 開く
      * システム環境設定       
      * セキュリティとプライバシー   
      * プライバシー   
      * アクセシビリティ   
   - `Code`をチェック
   ![스크린샷 2022-06-04 오후 3 19 58](https://user-images.githubusercontent.com/54441505/171987290-dd6803c0-cef6-4188-b7ba-b597aab34adf.png)
- この設定により、このコマンドの操作が可能になります：`@osascript -e 'tell application "System Events" to keystroke "./tester http://localhost:8000" '`
- 設定を変更できない場合は、以下の手順に従います：
   - ターミナルを開き（場所：このリポジトリのルートディレクトリ）、入力します：
   ```shell
   > make run
   ```
   - テストしたいポート（8000、8001、8002）が正常にバインドされていることを確認します
   - 別のターミナル（場所：このリポジトリのルートディレクトリ）を開き、入力します：
   ```shell
   > cd test/42_tester
   > ./tester http://localhost:8000
   ```
---

## 基本的なMakefileコマンド
### make
- サーバープログラムとクライアントプログラムをコンパイル

### make re
- `fclean` && 再度`make`

### make clean
- オブジェクトファイルを削除

### make fclean
- オブジェクトファイル(`make clean`)、ログファイル、実行ファイルを削除
 
---
## 全てのポートがバインドに失敗した場合、ポートをkillする
* PIDを確認
```shell
> lsof -i :8000
```
* PIDを使用してポートをkill
```shell
> kill -9 28423
```

[ポートをkillする](https://codinhood.com/nano/macos/find-kill-proccess-port-macos)

