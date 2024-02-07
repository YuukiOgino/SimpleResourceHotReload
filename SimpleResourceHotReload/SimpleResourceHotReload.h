/**
 * @brief C++とWin32 APIを利用したリソースファイルの中品質ホットリロード機能のサンプルコード
 * @copyright Yuuki Ogino
 */
/*! bulma.io v0.9.4 | MIT License | github.com/jgthms/bulma */

#pragma once

#include "resource.h"
#include <functional>
#include <map>
#include <string>
#include <thread>

//---------------------------------------------------------------------------------------------------------
// 定数定義
//---------------------------------------------------------------------------------------------------------	

/**
 * @def buffer_size
 * @brief ディレクトリ監視に使用するバッファサイズ
 */
constexpr auto buffer_size = 256 * 1;

/*
 * @def wait_millisecond_time
 * @brief 待ち時間（ミリ秒）
 */
constexpr auto wait_millisecond_time = 2000;


/**
 * @typedef hot_reload_success_callback
 * @brief ホットリロード成功時のコールバック
 */
using hot_reload_success_callback = std::function<void(std::string, void*, size_t)>;

/**
 * @typedef hot_reload_error_callback
 * @brief ホットリロードエラー時のコールバック
 */
using hot_reload_error_callback = std::function<void(std::string)>;

/**
 * @struct reload_callback
 * @brief ホットリロードコールバック格納用構造体
 */
struct reload_callback
{
    //! ホットリロード成功時のコールバック
    hot_reload_success_callback reload_cb;
    //! ホットリロードエラー時のコールバック
    hot_reload_error_callback reload_error_cb;
    //! 前回コールバックを呼び出した時間
    std::chrono::system_clock::time_point last_exec_time;

    /**
     * @brief コンストラクタ
     * @param [in] cb			: ホットリロード成功時のコールバック
     * @param [in] error_cb		: エラー時のコールバック
     */
    explicit reload_callback(const hot_reload_success_callback cb, const hot_reload_error_callback error_cb)
        : reload_cb(cb),
        reload_error_cb(error_cb),
        last_exec_time(std::chrono::system_clock::now())
    {
    }

    /**
     * @brief デストラクタ
     */
    ~reload_callback()
    {
        reload_cb = nullptr;
        reload_error_cb = nullptr;
    }
};

class simple_hot_reload final
{


    //---------------------------------------------------------------------------------------------------------
    // 変数
    //---------------------------------------------------------------------------------------------------------

	//! スレッド
	std::thread m_hr_thread_;

	//! マルチスレッド実行フラグ
	std::unique_ptr<bool> m_b_thread_exec_;

	//! コールバックリスト
	std::map<std::string, std::unique_ptr<reload_callback>> callback_list;

	//! ハンドル
	HANDLE h_dir_;

	//! 非同期I/Oで使用される情報
	OVERLAPPED olp_;

	//! イベントオブジェクト
	void* event_;
public:
    simple_hot_reload();
    ~simple_hot_reload();

    /**
     * @brief ホットリロード実行（ファイル監視実行）
     */
    void hot_reload_exec();

    /**
	 * @brief ホットリロード用コールバック関数をセット
	 * @param [in] name				: パス込みのファイル名
	 * @param [in] callback			: コールバック
	 * @param [in] error_callback	: エラー時のコールバック
	 */
    void bind_reload_callback(const std::string& name, hot_reload_success_callback callback, hot_reload_error_callback error_callback);

    /**
     * @brief ホットリロード用コールバック関数を解除
     * @param [in] name : パス込みのファイル名
     */
    void unbind_reload_callback(std::string& name);

    /**
     * @brief ファイル名のバリデートチェック
     * @details ファイル名のバリデートチェック。共通でホットリロード対象外のファイル名、フォルダが送られてきた場合はfalseを返す。
     */
    static bool validate_file_name(const std::string& filepath);
};