
#
# at install time create "zulucrypt-write" and "zulucrypt-read" groups
#

EXEC_PROGRAM( /usr/sbin/groupadd  ARGS -f zulucrypt-read OUTPUT_VARIABLE foo RETURN_VALUE bar )

EXEC_PROGRAM( /usr/sbin/groupadd  ARGS -f zulucrypt-write OUTPUT_VARIABLE foo RETURN_VALUE bar )
