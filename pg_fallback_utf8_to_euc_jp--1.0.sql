-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pg_fallback_utf8_to_euc_jp" to load this file. \quit

CREATE FUNCTION pg_fallback_utf8_to_euc_jp(integer, integer, cstring, internal, integer)
RETURNS void
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT VOLATILE;

CREATE CONVERSION pg_fallback_utf8_to_euc_jp
    FOR 'UTF8' TO 'EUC_JP' FROM pg_fallback_utf8_to_euc_jp;
