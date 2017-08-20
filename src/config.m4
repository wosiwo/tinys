dnl $Id$
dnl config.m4 for extension tinys

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(tinys, for tinys support,
dnl Make sure that the comment is aligned:
dnl [  --with-tinys             Include tinys support])

dnl Otherwise use enable:

 PHP_ARG_ENABLE(tinys, whether to enable tinys support,
 Make sure that the comment is aligned:
 [  --enable-tinys           Enable tinys support])

if test "$PHP_TINYS" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-tinys -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/tinys.h"  # you most likely want to change this
  dnl if test -r $PHP_TINYS/$SEARCH_FOR; then # path given as parameter
  dnl   TINYS_DIR=$PHP_TINYS
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for tinys files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       TINYS_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$TINYS_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the tinys distribution])
  dnl fi

  dnl # --with-tinys -> add include path
  dnl PHP_ADD_INCLUDE($TINYS_DIR/include)

  dnl # --with-tinys -> check for lib and symbol presence
  dnl LIBNAME=tinys # you may want to change this
  dnl LIBSYMBOL=tinys # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $TINYS_DIR/$PHP_LIBDIR, TINYS_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_TINYSLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong tinys lib version or lib not found])
  dnl ],[
  dnl   -L$TINYS_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  PHP_SUBST(TINYS_SHARED_LIBADD)

  PHP_NEW_EXTENSION(tinys, tinys.c server.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
