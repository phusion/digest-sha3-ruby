#include "ruby.h"
#include "KeccakNISTInterface.h"

#define MAX_DIGEST_SIZE 64

static VALUE mDigest, cSHA3;

typedef struct {
	hashState state;
	int bitlen;
} RbSHA3;

static VALUE
rb_sha3_new(VALUE klass, VALUE bitlen) {
	RbSHA3 *ctx;
	VALUE obj;

	ctx = (RbSHA3 *) xmalloc(sizeof(RbSHA3));
	obj = Data_Wrap_Struct(klass, 0, xfree, ctx);
	ctx->bitlen = NUM2INT(bitlen);

	if (ctx->bitlen == 0) {
		rb_raise(rb_eRuntimeError, "Unsupported hash length");
	}

	switch (Init(&ctx->state, ctx->bitlen)) {
	case SUCCESS:
		return obj;
	case FAIL:
		rb_raise(rb_eRuntimeError, "Unknown error");
		return Qnil;
	case BAD_HASHLEN:
		rb_raise(rb_eRuntimeError, "Bad hash length (must be 0, 224, 256, 384 or 512)");
		return Qnil;
	default:
		rb_raise(rb_eRuntimeError, "Unknown error code");
		return Qnil;
	}
}

static VALUE
rb_sha3_copy(VALUE copy, VALUE obj) {
	RbSHA3 *ctx_copy, *ctx_obj;

	Data_Get_Struct(copy, RbSHA3, ctx_copy);
	Data_Get_Struct(obj, RbSHA3, ctx_obj);
	if (copy == obj) {
		return copy;
	}
	rb_check_frozen(copy);

	memcpy(&ctx_copy->state, &ctx_obj->state, sizeof(hashState));
	ctx_copy->bitlen = ctx_obj->bitlen;
	return copy;
}

static VALUE
rb_sha3_reset(VALUE self) {
	RbSHA3 *ctx;

	Data_Get_Struct(self, RbSHA3, ctx);
	Init(&ctx->state, ctx->bitlen);
	return self;
}

static VALUE
rb_sha3_update(VALUE self, VALUE str) {
	RbSHA3 *ctx;

	Data_Get_Struct(self, RbSHA3, ctx);
	Update(&ctx->state, RSTRING_PTR(str), RSTRING_LEN(str) * 8);
	return self;
}

static VALUE
rb_sha3_digest(VALUE self, VALUE str) {
	RbSHA3 *ctx;
	hashState state;
	unsigned char digest[MAX_DIGEST_SIZE];

	Data_Get_Struct(self, RbSHA3, ctx);
	memcpy(&state, &ctx->state, sizeof(hashState));
	Final(&state, digest);
	return rb_str_new((const char *) digest, ctx->bitlen / 8);
}

static VALUE
rb_sha3_singleton_digest(int argc, VALUE *argv, VALUE klass) {
	VALUE data, hashlen;
	int i_hashlen;
	unsigned char digest[MAX_DIGEST_SIZE];

	if (rb_scan_args(argc, argv, "11", &data, &hashlen) == 1) {
		i_hashlen = 512;
	} else {
		i_hashlen = NUM2INT(hashlen);
	}

	switch (Hash(i_hashlen, RSTRING_PTR(data), RSTRING_LEN(data) * 8, digest)) {
	case SUCCESS:
		return rb_str_new(digest, i_hashlen / 8);
	case FAIL:
		rb_raise(rb_eRuntimeError, "Unknown error");
		return Qnil;
	case BAD_HASHLEN:
		rb_raise(rb_eRuntimeError, "Bad hash length (must be 0, 224, 256, 384 or 512)");
		return Qnil;
	default:
		rb_raise(rb_eRuntimeError, "Unknown error code");
		return Qnil;
	}
}

void
Init_sha3() {
	mDigest = rb_define_module("Digest");
	cSHA3 = rb_define_class_under(mDigest, "SHA3", rb_cObject);
	rb_define_singleton_method(cSHA3, "new", rb_sha3_new, 1);
	rb_define_method(cSHA3, "initialize_copy",  rb_sha3_copy, 1);
	rb_define_method(cSHA3, "reset",  rb_sha3_reset, 0);
	rb_define_method(cSHA3, "update",  rb_sha3_update, 1);
	rb_define_method(cSHA3, "<<",  rb_sha3_update, 1);
	rb_define_method(cSHA3, "digest",  rb_sha3_digest, 0);
	rb_define_singleton_method(cSHA3, "digest", rb_sha3_singleton_digest, -1);
	rb_require("digest/sha3/helpers");
}
