CREATE EXTENSION pg_fallback_utf8_to_euc_jp;

\pset null '(null)'

CREATE OR REPLACE FUNCTION pg_fallback_all_utf8(OUT code integer, OUT utf8 text)
  RETURNS SETOF record AS $$
DECLARE
  code integer;
BEGIN
  FOR code IN 0 .. 1114111 LOOP
    BEGIN
      RETURN QUERY SELECT code, chr(code);
    EXCEPTION WHEN others THEN
    END;
  END LOOP;
END;
$$ LANGUAGE plpgsql;

CREATE TABLE fallback_all_utf8 (code integer, utf8 text);
INSERT INTO fallback_all_utf8 SELECT * FROM pg_fallback_all_utf8();

SET client_encoding TO utf8;
SELECT * FROM fallback_all_utf8 WHERE code = 65533;

SET client_encoding TO eucjp;
SELECT * FROM fallback_all_utf8 WHERE code = 65533;

UPDATE pg_conversion SET condefault = 'f' WHERE conname = 'utf8_to_euc_jp';
UPDATE pg_conversion SET condefault = 't' WHERE conname = 'pg_fallback_utf8_to_euc_jp';

\c contrib_regression
SET client_encoding TO eucjp;
SELECT * FROM fallback_all_utf8 ORDER BY code;
SELECT string_agg(utf8, '') FROM (SELECT utf8 FROM fallback_all_utf8 ORDER BY code) ss;

DROP EXTENSION pg_fallback_utf8_to_euc_jp;
