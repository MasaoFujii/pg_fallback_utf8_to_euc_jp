MODULE_big	= pg_fallback_utf8_to_euc_jp
OBJS = pg_fallback_utf8_to_euc_jp.o

EXTENSION = pg_fallback_utf8_to_euc_jp
DATA = pg_fallback_utf8_to_euc_jp--1.0.sql

REGRESS = pg_fallback_utf8_to_euc_jp

PGFILEDESC = "pg_fallback_utf8_to_euc_jp - encoding conversion from UTF-8 to EUC_JP"

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/pg_fallback_utf8_to_euc_jp
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
