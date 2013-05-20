require 'mkmf'

have_header('ruby/digest.h')
have_func('rb_str_set_len')

$CFLAGS << " -fvisibility=hidden"
create_makefile('digest/sha3')
