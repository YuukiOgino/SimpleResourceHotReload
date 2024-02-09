// SimpleResourceHotReload.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "SimpleResourceHotReload.h"

#include <filesystem>
#include <fstream>

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。
    const auto hr = std::make_unique<simple_hot_reload>();
	hr->hot_reload_exec();

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SIMPLERESOURCEHOTRELOAD, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SIMPLERESOURCEHOTRELOAD));

    MSG msg;

    // メイン メッセージ ループ:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SIMPLERESOURCEHOTRELOAD));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SIMPLERESOURCEHOTRELOAD);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 選択されたメニューの解析:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: HDC を使用する描画コードをここに追加してください...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// バージョン情報ボックスのメッセージ ハンドラーです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


simple_hot_reload::simple_hot_reload(): h_dir_(nullptr), olp_(), event_(nullptr)
{
}

simple_hot_reload::~simple_hot_reload()
{
	hot_reload_end();
}

//---------------------------------------------------------------------------------------------------------
// ホットリロード
//---------------------------------------------------------------------------------------------------------	

/**
 * @brief ホットリロードを実行 
 */
void simple_hot_reload::hot_reload_exec()
{
	// ホットリロードサンプル
	m_b_thread_exec_ = std::make_unique<bool>(true);

	// 読み込み用スレッド起動
	m_hr_thread_ = std::thread([this]
		{
			const std::wstring p = std::filesystem::current_path().wstring();

			h_dir_ =
				CreateFileW(
					p.c_str(),
					FILE_READ_DATA,
					FILE_SHARE_READ,
					nullptr,
					OPEN_EXISTING,
					FILE_FLAG_BACKUP_SEMANTICS,
					nullptr
				);

			//最終書き込み日時変更のみフィルタリング

			std::vector<wchar_t> buf(buffer_size);
			void* p_buf = &buf[0];
			event_ = CreateEvent(nullptr, true, false, nullptr);

			while (m_b_thread_exec_)
			{
				// イベントの手動リセット
				ResetEvent(event_);
				olp_ = {};
				olp_.hEvent = event_;

				if (constexpr DWORD dw_notify_filter = FILE_NOTIFY_CHANGE_LAST_WRITE;
					!ReadDirectoryChangesW(
						h_dir_,
						p_buf,
						buffer_size,
						true,
						dw_notify_filter,
						nullptr,
						&olp_,
						nullptr))
				{
					return;
				}

				while (m_b_thread_exec_)
				{
					// 変更通知まち
					if (const auto wait_result = WaitForSingleObject(event_, 1000); wait_result != WAIT_TIMEOUT)
					{
						// 変更通知があった場合 (イベントがシグナル状態になった場合)
						break;
					}
				}

				if (!m_b_thread_exec_)
				{
					// 途中終了するなら非同期I/Oも中止し、
					// Overlapped構造体をシステムが使わなくなるまで待機する必要がある.
					if (h_dir_ != nullptr)
					{
						CancelIoEx(h_dir_, &olp_);
						h_dir_ = nullptr;
						event_ = nullptr;
					}
					WaitForSingleObject(event_, INFINITE);
					break;
				}

				// 非同期I/Oの結果を取得する.
				DWORD size = 0;
				if (!GetOverlappedResult(h_dir_, &olp_, &size, false))
				{
					// 結果取得に失敗した場合
					hot_reload_end();
					return;
				}

				auto* lp_information = static_cast<FILE_NOTIFY_INFORMATION*>(p_buf);

				while (true)
				{
					// ReadDirectoryChangesWでは文字列はwstringのため、必要であればstringへ変換をかける
					std::wstring w_file_name = lp_information->FileName;

					std::string filename(w_file_name.begin(), w_file_name.end());
					std::string old = "\\";
					std::string rep = "/";
					// バックスラッシュを変換

					// 変換処理は省略（指定の文字列を全置換処理するには自作する必要があるため）
					// 参考：https://learningprog.com/cpp-replace/

					//　ホットリロード対象のファイルである場合は再読み込みを実行させる
					if (validate_file_name(filename))
					{
						switch (lp_information->Action)
						{
						case FILE_ACTION_MODIFIED:
						{
							// 書き込むタイミングによってはサイズが0の場合があるため、チェックを行う
							std::filesystem::path path = filename;
							if (const auto file_size = std::filesystem::file_size(path); file_size > 0)
							{
								if (callback_list.contains(filename))
								{
									auto end = std::chrono::system_clock::now();
									// 短時間で連続２回以上更新の通知がくる場合があるので、待ち時間を過ぎた場合のみコールバックを実行する
									if (std::chrono::duration_cast<std::chrono::milliseconds>(end - callback_list[filename]->last_exec_time).count() > wait_millisecond_time)
									{
										callback_list[filename]->last_exec_time = end;
										// 何かしらの方法でファイル読み込みを実行(各自で使用しているライブラリに合わせてください。)
										std::ifstream file(filename);
										//読込が終わったらコールバックに渡す(サンプルは雑に渡してます。バイナリデータを渡してください)
										callback_list[filename]->reload_cb(filename, file.rdbuf(), file_size);
										file.close();
									}
								}
							}
							break;
						}
						default:
							break;
						}
					}
					// オフセットが0の場合はbreak
					if (lp_information->NextEntryOffset == 0) break;

					lp_information = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<unsigned char*>(lp_information) + lp_information->NextEntryOffset);
				}

				// バッファを初期化する
				buf.clear();
				buf.resize(buffer_size);
				p_buf = buf.data();
			}
		});
}

/**
 * @brief ホットリロード終了
 */
void simple_hot_reload::hot_reload_end()
{
	if (m_b_thread_exec_ != nullptr && *m_b_thread_exec_)
	{
		if (h_dir_ != nullptr)
		{
			CloseHandle(event_);
			CancelIoEx(h_dir_, &olp_);
			h_dir_ = nullptr;
			event_ = nullptr;
		}
		m_b_thread_exec_.reset(nullptr);
		m_hr_thread_.join();
	}
}

/**
 * @brief ホットリロード コールバック登録
 * @details 指定のアセット更新を検知した時のコールバック関数を登録します。
 */
void simple_hot_reload::bind_reload_callback(const std::string& name, hot_reload_success_callback callback, hot_reload_error_callback error_callback)
{
	auto cb = std::make_unique<reload_callback>(std::move(callback), std::move(error_callback));
	callback_list.emplace(name, std::move(cb));
}

/**
 * @brief ホットリロード コールバック削除
 * @details 指定のアセット更新を検知した時のコールバック関数を削除します。
 */
void simple_hot_reload::unbind_reload_callback(std::string& name)
{
	callback_list.erase(name);
}

//---------------------------------------------------------------------------------------------------------
// バリデートチェック
//---------------------------------------------------------------------------------------------------------	
/**
 * @brief ファイル名のバリデートチェック
 * @details ファイル名のバリデートチェック。共通でホットリロード対象外のファイル名、フォルダが送られてきた場合はfalseを返す。
 */
bool simple_hot_reload::validate_file_name(const std::string& filepath)
{
	// .がない場合はほぼフォルダのみの更新のため、false
	if (filepath.find('.') == std::string::npos) return false;
	// svnフォルダの中身を読み込ませるのは必要ないため、false
	if (filepath.find(".svn") != std::string::npos) return false;
	// exeは読み込ませる必要はないため、false
	if (filepath.find(".exe") != std::string::npos) return false;
	// .gitは読み込ませる必要はないため、false
	if (filepath.find(".git") != std::string::npos) return false;
	// .dllは読み込ませる必要はないため、false
	if (filepath.find(".dll") != std::string::npos) return false;
	return true;
}