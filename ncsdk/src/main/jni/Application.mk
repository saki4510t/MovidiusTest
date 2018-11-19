#NDK_TOOLCHAIN_VERSIONの同じ行内にコメントをくっつけたらダメ.値の後ろにスペースとかもダメ
NDK_TOOLCHAIN_VERSION := clang
#APP_LDFLAGS += -fuse-ld=mcld

APP_CPPFLAGS += -std=c++14
APP_CPPFLAGS += -fexceptions			# 例外を有効にする
APP_CPPFLAGS += -Wreturn-type
APP_CPPFLAGS += -Wno-multichar
APP_CPPFLAGS += -Wno-incompatible-pointer-types
APP_CPPFLAGS += -Wno-address-of-packed-member
APP_CPPFLAGS += -frtti					# RTTI(実行時型情報)を有効にする

APP_CFLAGS += -DHAVE_PTHREADS
APP_CFLAGS += -fPIC

#ログ出力設定
APP_CFLAGS += -DNDEBUG					# LOG_ALLを無効にする・assertを無効にする場合
APP_CFLAGS += -DLOG_NDEBUG				# デバッグメッセージを出さないようにする時
#最適化設定
APP_CFLAGS += -DAVOID_TABLES
APP_CFLAGS += -O3 -fstrict-aliasing
APP_CFLAGS += -fprefetch-loop-arrays
#APP_CFLAGS += -ffunction-sections -funwind-tables -fstack-protector -fno-short-enums
#APP_CFLAGS += -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp -march=armv7-a
#APP_CFLAGS += -fno-exceptions -fno-rtti

#警告を消す設定
APP_CFLAGS += -Wno-parentheses
APP_CFLAGS += -Wno-switch
APP_CFLAGS += -Wno-extern-c-compat
APP_CFLAGS += -Wno-empty-body
APP_CFLAGS += -Wno-deprecated-register

#デバッグ用に警告を出す設定
#APP_CFLAGS += -Wall
#APP_CFLAGS += -Wreorder

#APP_PLATFORM := android-22
APP_PLATFORM := android-16

#出力アーキテクチャ
APP_ABI := armeabi-v7a arm64-v8a x86 x86_64

#STLライブラリ
APP_STL := c++_shared

#出力オプション
APP_OPTIM := release
#APP_OPTIM := debug