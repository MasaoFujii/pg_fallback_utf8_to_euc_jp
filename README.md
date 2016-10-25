# pg_fallback_utf8_to_euc_jp
This extension provides yet another encoding conversion from UTF-8 to EUC_JP.

このモジュールは、UTF-8からEUC_JPへの特別な文字変換機能を提供します。

pg_fallback_utf8_to_euc_jp is released under [the PostgreSQL License](https://opensource.org/licenses/postgresql), a liberal Open Source license, similar to the BSD or MIT licenses.

pg_fallback_utf8_to_euc_jpのライセンスは[the PostgreSQL License](https://opensource.org/licenses/postgresql)(BSDに似たライセンス)です。

## Test Status
[![Build Status](https://travis-ci.org/MasaoFujii/pg_fallback_utf8_to_euc_jp.svg?branch=master)](https://travis-ci.org/MasaoFujii/pg_fallback_utf8_to_euc_jp)

## Install

Download the source archive of pg_fallback_utf8_to_euc_jp from
[here](https://github.com/MasaoFujii/pg_fallback_utf8_to_euc_jp),
and then build and install it.

[ここ](https://github.com/MasaoFujii/pg_fallback_utf8_to_euc_jp)からpg_fallback_utf8_to_euc_jpのソースをダウンロードして、
以下のとおりコンパイルとインストールを行ってください。

    $ cd pg_fallback_utf8_to_euc_jp
    $ make USE_PGXS=1 PG_CONFIG=/opt/pgsql-X.Y.Z/bin/pg_config
    $ su
    # make USE_PGXS=1 PG_CONFIG=/opt/pgsql-X.Y.Z/bin/pg_config install
    # exit

USE_PGXS=1 must be always specified when building this extension.
The path to [pg_config](http://www.postgresql.org/docs/devel/static/app-pgconfig.html)
(which exists in the bin directory of PostgreSQL installation)
needs be specified in PG_CONFIG.
However, if the PATH environment variable contains the path to pg_config,
PG_CONFIG doesn't need to be specified.

コンパイルおよびインストール時、USE_PGXS=1の指定は必須です。
また、[pg_config](http://www.postgresql.jp/document/current/html/app-pgconfig.html)コマンド
(PostgreSQLインストール先のbinディレクトリに存在)のパスをPG_CONFIGに指定する必要があります。
pg_configにPATHが通っているのであれば、PG_CONFIGの指定は不要です。

## Load

Load pg_fallback_utf8_euc_jp into all databases
where encoding conversion from UTF-8 to EUC_JP can happen.

UTF-8からEUC_JPへの文字変換が発生する可能性のあるすべてのデータベース上で、
pg_fallback_utf8_euc_jpモジュールを登録してください。

    =# CREATE EXTENSION pg_fallback_utf8_to_euc_jp;

Update pg_conversion.condefault to use pg_fallback_utf8_to_euc_jp as a default
conversion from UTF-8 to EUC_JP.

UTF-8からEUC_JPへの文字変換にpg_fallback_utf8_to_euc_jpが使われるように、
以下の手順でpg_conversionカタログのcondefaultカラムを変更してください。

    =# BEGIN;
    =# UPDATE pg_conversion SET condefault = 'f' WHERE conname = 'utf8_to_euc_jp';
    =# UPDATE pg_conversion SET condefault = 't' WHERE conname = 'pg_fallback_utf8_to_euc_jp';
    =# COMMIT;

Logout the session. Since next login, pg_fallback_utf8_to_euc_jp will be used as
a default encoding conversion from UTF-8 to EUC_JP.

現在のセッションからログアウトしてください。
次回のログインから、pg_fallback_utf8_to_euc_jpが、UTF-8からEUC_JPへの
デフォルトのエンコーディング変換として使われます。

## Uninstall

Reset pg_conversion, unload pg_fallback_utf8_to_euc_jp from the database
and then uninstall it.

pg_conversionの変更をリセットし、pg_fallback_utf8_to_euc_jpのデータベースからのアンロードと
アンインストールを行ってください。

    =# BEGIN;
    =# UPDATE pg_conversion SET condefault = 'f' WHERE conname = 'pg_fallback_utf8_to_euc_jp';
    =# UPDATE pg_conversion SET condefault = 't' WHERE conname = 'utf8_to_euc_jp';
    =# COMMIT;
    
    =# DROP EXTENSION pg_fallback_utf8_to_euc_jp;
    =# \q
    
    # cd <pg_fallback_utf8_to_euc_jp source directory>
    # make USE_PGXS=1 PG_CONFIG=/opt/pgsql-X.Y.Z/bin/pg_config uninstall
    # exit
