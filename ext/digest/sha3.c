#include "ruby.h"
#ifdef HAVE_RUBY_DIGEST_H
#include "ruby/digest.h"
#else
#include "digest.h"
#endif
#include "KeccakNISTInterface.h"

#define MAX_DIGEST_SIZE 64
#define DEFAULT_DIGEST_LEN 512

static void sha3_init_func(hashState *ctx);
static void sha3_update_func(hashState *ctx, unsigned char *str, size_t len);

static rb_digest_metadata_t sha3 = {
	RUBY_DIGEST_API_VERSION,
	DEFAULT_DIGEST_LEN,
	KeccakPermutationSize - (2 * DEFAULT_DIGEST_LEN),
	sizeof(hashState),
	(rb_digest_hash_init_func_t)sha3_init_func,
	(rb_digest_hash_update_func_t)sha3_update_func,
	NULL,
};

static void
sha3_init(hashState *ctx, size_t bitlen) {
	switch (Init(ctx, bitlen)) {
	case SUCCESS:
		return;
	case FAIL:
		rb_raise(rb_eRuntimeError, "Unknown error");
	case BAD_HASHLEN:
		rb_raise(rb_eArgError, "Bad hash length (must be 0, 224, 256, 384 or 512)");
	default:
		rb_raise(rb_eRuntimeError, "Unknown error code");
	}
}

static void
sha3_init_func(hashState *ctx) {
	Init(ctx, ctx->capacity / 2);
}

static void
sha3_update_func(hashState *ctx, unsigned char *str, size_t len) {
	Update(ctx, str, len * 8);
}

static VALUE
rb_sha3_alloc(VALUE klass) {
	hashState *ctx;

	ctx = (hashState *) xmalloc(sizeof(hashState));
	sha3_init(ctx, DEFAULT_DIGEST_LEN);
	return Data_Wrap_Struct(klass, 0, xfree, ctx);
}

static VALUE
rb_sha3_initialize(int argc, VALUE *argv, VALUE self) {
	hashState *ctx;
	VALUE hashlen;
	int i_hashlen;

	if (rb_scan_args(argc, argv, "01", &hashlen) == 0) {
		i_hashlen = DEFAULT_DIGEST_LEN;
	} else {
		i_hashlen = NUM2INT(hashlen);
	}
	switch (i_hashlen) {
	case 0:
		rb_raise(rb_eArgError, "Unsupported hash length");
	case DEFAULT_DIGEST_LEN:
		break;
	default:
		Data_Get_Struct(self, hashState, ctx);
		sha3_init(ctx, i_hashlen);
	}

	return self;
}

static VALUE
rb_sha3_finish(VALUE self) {
	hashState *ctx;
	VALUE digest;

	Data_Get_Struct(self, hashState, ctx);

	digest = rb_str_new(0, ctx->capacity / 2 / 8);

	Final(ctx, (unsigned char *)RSTRING_PTR(digest));

	return digest;
}

static VALUE
rb_sha3_digest_length(VALUE self) {
	hashState *ctx;

	Data_Get_Struct(self, hashState, ctx);
	return INT2FIX(ctx->capacity / 2 / 8);
}

static VALUE
rb_sha3_block_length(VALUE self) {
	hashState *ctx;

	Data_Get_Struct(self, hashState, ctx);
	return INT2FIX(ctx->rate / 8);
}

void __attribute__((visibility("default")))
Init_sha3() {
	VALUE mDigest, cDigest_Base, cSHA3;

	rb_require("digest");

	mDigest = rb_path2class("Digest");
	cDigest_Base = rb_path2class("Digest::Base");

	cSHA3 = rb_define_class_under(mDigest, "SHA3", cDigest_Base);

	rb_ivar_set(cSHA3, rb_intern("metadata"), Data_Wrap_Struct(rb_cObject, 0, 0, &sha3));

	rb_define_alloc_func(cSHA3, rb_sha3_alloc);
	rb_define_method(cSHA3, "initialize", rb_sha3_initialize, -1);
	rb_define_private_method(cSHA3, "finish", rb_sha3_finish, 0);
	rb_define_method(cSHA3, "digest_length", rb_sha3_digest_length, 0);
	rb_define_method(cSHA3, "block_length", rb_sha3_block_length, 0);
}
