require 'mkmf'
$CFLAGS << " -fvisibility=hidden"
create_makefile('digest/sha3')
