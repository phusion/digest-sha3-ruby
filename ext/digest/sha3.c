#include "ruby.h"

static VALUE mDigest, mSHA3;

void
Init_sha3() {
	mDigest = rb_define_module("Digest");
	mSHA3 = rb_define_module_under(mDigest, "SHA3");
}
