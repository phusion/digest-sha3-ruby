#include "ruby.h"
#ifdef HAVE_RUBY_DIGEST_H
#include "ruby/digest.h"
#else
#include "digest.h"
#endif
#include "sha3impl.h"

#define MAX_DIGEST_SIZE 64
#define DEFAULT_DIGEST_LEN 512

static int sha3_init_func(sha3_ctx *ctx);
static void sha3_update_func(sha3_ctx *ctx, unsigned char *str, size_t len);
static int sha3_finish_func(sha3_ctx *ctx, unsigned char *digest);

/*
  Metadata definition for the SHA3 algorithm.
  Defines the Version, sizes for block and digest as well as
  the entry points for the algorithms
*/
static rb_digest_metadata_t sha3 = {
	RUBY_DIGEST_API_VERSION,
	DEFAULT_DIGEST_LEN,
	SHA3_STATE_BITS - (2 * DEFAULT_DIGEST_LEN), //size of blocks
	sizeof(sha3_ctx), //size of context for the object we'll be passed in below functions.
	(rb_digest_hash_init_func_t)sha3_init_func,
	(rb_digest_hash_update_func_t)sha3_update_func,
	(rb_digest_hash_finish_func_t)sha3_finish_func,
};

/* Initialization function for the algorithm,
   gets called during allocation of the digest object.
   we override initialize to do custom hash size, so we don't care too much here.
*/
static int
sha3_init_func(sha3_ctx *ctx) {
  // Just return a 1 ' successful' we override the init function
  // so this is not necessary
  // the base class alloc calls this to initialize the algorithm
  return 1;
}

/* Update function, take the current context and add str to it */
static void
sha3_update_func(sha3_ctx *ctx, unsigned char *str, size_t len) {
	FIPS202_SHA3_Update(ctx, str, len);
}

/* Finish the hash calculation and return the finished string */
static int
sha3_finish_func(sha3_ctx *ctx, unsigned char *digest) {
	FIPS202_SHA3_Final(ctx, digest, FIPS202_SHA3_HashSize(ctx));
	return 1;
}

/* Ruby method.  Digest::SHA3#finish()
 * No Arguments
 * @returns [String] Encoded Digest String
 */
static VALUE
rb_sha3_finish(VALUE self) {
  sha3_ctx *ctx;
  VALUE digest;

  ctx = (sha3_ctx *)RTYPEDDATA_DATA(self);
  digest = rb_str_new(0, FIPS202_SHA3_HashSize(ctx));
  sha3_finish_func(ctx, (unsigned char *)RSTRING_PTR(digest));

  return digest;
}

/* :nodoc: private method
 * initialize the ctx with the bitlength
 */
static void
sha3_init(sha3_ctx *ctx, size_t bitlen) {
	switch (FIPS202_SHA3_Init(ctx, bitlen)) {
	case SHA3_OK:
		return;
	case SHA3_ERR:
		rb_raise(rb_eArgError, "Bad hash length (must be 0, 224, 256, 384 or 512)");
	default:
		rb_raise(rb_eRuntimeError, "Unknown error code");
	}
}

/* Ruby method.  Digest::SHA3.new(hashlen)
 * @param hashlen The length of hash, only supports 224, 256, 384 or 512
 * @returns [Digest::SHA3] new object.
 */
static VALUE
rb_sha3_initialize(int argc, VALUE *argv, VALUE self) {
	sha3_ctx *ctx;
	VALUE hashlen;
	int i_hashlen;

	if (rb_scan_args(argc, argv, "01", &hashlen) == 0) {
		i_hashlen = DEFAULT_DIGEST_LEN;
	} else {
		i_hashlen = NUM2INT(hashlen);
	}
	if ( i_hashlen == 0) {
		rb_raise(rb_eArgError, "Unsupported hash length");
  }

  ctx = (sha3_ctx *)RTYPEDDATA_DATA(self);
	sha3_init(ctx, i_hashlen);

	return rb_call_super(0, NULL);
}

/* Ruby method.  Digest::SHA3#digest_length
 * @returns [Numeric] Length of the digest.
 */
static VALUE
rb_sha3_digest_length(VALUE self) {
	sha3_ctx *ctx;

	ctx = (sha3_ctx *)RTYPEDDATA_DATA(self);
	return INT2FIX(FIPS202_SHA3_HashSize(ctx));
}

/* Ruby method.  Digest::SHA3#block_length
 * @returns [Numeric] Length of blocks in this digest.
 */
static VALUE
rb_sha3_block_length(VALUE self) {
	sha3_ctx *ctx;

	ctx = (sha3_ctx *)RTYPEDDATA_DATA(self);
	return INT2FIX(FIPS202_SHA3_BlockSize(ctx));
}

void __attribute__((visibility("default")))
Init_sha3() {
	VALUE mDigest, cDigest_Base, cSHA3;

	rb_require("digest");

	mDigest = rb_path2class("Digest");
	cDigest_Base = rb_path2class("Digest::Base");

	cSHA3 = rb_define_class_under(mDigest, "SHA3", cDigest_Base);

	rb_iv_set(cSHA3, "metadata", Data_Wrap_Struct(0, 0, 0, (void *)&sha3));

  rb_define_method(cSHA3, "initialize", rb_sha3_initialize, -1);
  rb_define_method(cSHA3, "digest_length", rb_sha3_digest_length, 0);
  rb_define_method(cSHA3, "block_length", rb_sha3_block_length, 0);
  rb_define_method(cSHA3, "finish", rb_sha3_finish, 0);
}
