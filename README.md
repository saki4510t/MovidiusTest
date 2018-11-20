* ncsdkモジュールのsrc/main/jni下にhttps://github.com/movidius/ncsdkをcloneすること
* 今はncsdk1.x対応のみなので1.x系をcheckout(デフォルト)
* 作成時のncsdkのバージョンは1.12.01.01
* src/main/jni/ncsdk/api/src下のget_mvcmd.shを実行してMvNcApi.mvcmdを取得、名前を小文字と_にしてres/raw下へ移動