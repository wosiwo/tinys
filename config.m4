 PHP_ARG_ENABLE(tinys, whether to enable tinys support,
 Make sure that the comment is aligned:
 [  --enable-tinys           Enable tinys support])
if test "$PHP_TINYS" != "no"; then
  PHP_SUBST(TINYS_SHARED_LIBADD)
  source_file="tinys.c tinys_client.c src/client.c src/reactor.c src/server.c src/worker.c  "
  PHP_NEW_EXTENSION(tinys,$source_file, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
