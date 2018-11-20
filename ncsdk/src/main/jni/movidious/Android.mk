LOCAL_PATH := $(call my-dir)

######################################################################
# libmovidius_static.a
######################################################################
include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/.. \
	$(LOCAL_PATH)/../externalLibs/include \
	$(LOCAL_PATH)/../externalLibs/include/aandusb \
	$(LOCAL_PATH)/../externalLibs/include/common \
	$(LOCAL_PATH)/../externalLibs/include/rapidjson/include \

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)

#マクロ定義
LOCAL_CFLAGS += -DANDROID_NDK
LOCAL_CFLAGS += -DNDEBUG							# LOG_ALLを無効にする・assertを無効にする場合
LOCAL_CFLAGS += -DLOG_NDEBUG						# デバッグメッセージを出さないようにする時
#LOCAL_CFLAGS += -DUSE_LOGALL						# define USE_LOGALL macro to enable all debug string
#LOCAL_CFLAGS += -DDISABLE_IMPORTGL					# when static link OpenGL|ES library
#
#LOCAL_CPPFLAGS += -fexceptions						# 例外を有効にする
#LOCAL_CPP_FEATURES += exceptions 
#LOCAL_CPPFLAGS += -frtti							# RTTI(実行時型情報)を有効にする
#LOCAL_CFLAGS += -Wno-multichar

#public関数のみエクスポートする
LOCAL_CFLAGS += -Wl,--version-script,movidius.map

#最適化設定
#LOCAL_CFLAGS += -DAVOID_TABLES
#LOCAL_CFLAGS += -O3 -fstrict-aliasing
#LOCAL_CFLAGS += -fprefetch-loop-arrays

#デバッグ用に警告を出す設定
#LOCAL_CFLAGS += -Wall

#アセンブラのソース(リスティングファイル)を出力させる(遅くなるけど)
#LOCAL_CFLAGS += -fverbose-asm
#LOCAL_CFLAGS +=-save-temps					# このオプションをつけると中間ファイルを削除しない(プロジェクトルートに残る)
#LOCAL_FILTER_ASM := python -c 'import sys; import shutil; src = open(sys.argv[1], "rb"); dst = open(sys.argv[2], "wb"); shutil.copyfileobj(src,dst);'

LOCAL_SRC_FILES := \
	_onload.cpp \
	serenegiant_ncs_movidius.cpp \

LOCAL_EXPORT_LDLIBS += -L$(SYSROOT)/usr/lib -ldl	# to avoid NDK issue(no need for static library)
LOCAL_EXPORT_LDLIBS += -llog

#静的リンクする外部モジュール

#動的リンクする外部モジュール
LOCAL_SHARED_LIBRARIES += common aandusb

LOCAL_ARM_MODE := arm
LOCAL_MODULE    := libmovidius_static

#スタティックライブラリとしてリンク
include $(BUILD_STATIC_LIBRARY)


######################################################################
# libmovidius.so
######################################################################
include $(CLEAR_VARS)
LOCAL_EXPORT_LDLIBS += -llog
LOCAL_EXPORT_C_INCLUDES := \
	$(LOCAL_PATH)/ \

LOCAL_WHOLE_STATIC_LIBRARIES = libmovidius_static
LOCAL_MODULE := libmovidius

include $(BUILD_SHARED_LIBRARY)
