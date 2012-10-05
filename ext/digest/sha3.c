#include "ruby.h"
#include "KeccakNISTInterface.h"

#define MAX_DIGEST_SIZE 64

static VALUE mDigest, cSHA3;

typedef struct {
	hashState state;
	int bitlen;
} RbSHA3;

static VALUE
rb_sha3_alloc(VALUE klass) {
	RbSHA3 *ctx;
	
	ctx = (RbSHA3 *) xmalloc(sizeof(RbSHA3));
	ctx->bitlen = -1;
	return Data_Wrap_Struct(klass, 0, xfree, ctx);
}

static VALUE
rb_sha3_initialize(int argc, VALUE *argv, VALUE self) {
	RbSHA3 *ctx;
	VALUE hashlen;
	int i_hashlen;

	if (rb_scan_args(argc, argv, "01", &hashlen) == 0) {
		i_hashlen = 512;
	} else {
		i_hashlen = NUM2INT(hashlen);
	}
	if (i_hashlen == 0) {
		rb_raise(rb_eRuntimeError, "Unsupported hash length");
	}

	Data_Get_Struct(self, RbSHA3, ctx);
	ctx->bitlen = i_hashlen;

	switch (Init(&ctx->state, i_hashlen)) {
	case SUCCESS:
		return self;
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
rb_sha3_initialize_copy(VALUE self, VALUE other) {
	RbSHA3 *ctx_self, *ctx_other;

	rb_check_frozen(self);
	Data_Get_Struct(self, RbSHA3, ctx_self);
	Data_Get_Struct(other, RbSHA3, ctx_other);
	memcpy(&ctx_self->state, &ctx_other->state, sizeof(hashState));
	ctx_self->bitlen = ctx_other->bitlen;
	return self;
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

void __attribute__((visibility("default")))
Init_sha3() {
	mDigest = rb_define_module("Digest");
	cSHA3 = rb_define_class_under(mDigest, "SHA3", rb_cObject);
	rb_define_alloc_func(cSHA3, rb_sha3_alloc);
	rb_define_method(cSHA3, "initialize", rb_sha3_initialize, -1);
	rb_define_method(cSHA3, "initialize_copy",  rb_sha3_initialize_copy, 1);
	rb_define_method(cSHA3, "reset",  rb_sha3_reset, 0);
	rb_define_method(cSHA3, "update",  rb_sha3_update, 1);
	rb_define_method(cSHA3, "<<",  rb_sha3_update, 1);
	rb_define_method(cSHA3, "digest",  rb_sha3_digest, 0);
	rb_define_singleton_method(cSHA3, "digest", rb_sha3_singleton_digest, -1);
	rb_require("digest/sha3/helpers");
}
