# The SHA-3 (Keccak) extension for Ruby

This Ruby extension implements the SHA-3 ([Keccak](http://keccak.noekeon.org/)) cryptographic hashing algorithm. It is based on the reference C implementation, version 3.2. The exposed interface is almost identical to that of the `digest` standard library.

## Installation

    gem install digest-sha3

## Usage

Keccak supports 5 hash lengths: 224-bit, 256-bit, 384-bit, 512-bit and variable length. Variable length is not supported by this Ruby extension. Unless the user specifies otherwise, this Ruby extension assumes 512-bit.

    require 'digest/sha3'

    # Generate 512-bit digest.
    Digest::SHA3.digest("foo")       # => "\025\227\204*..."
    Digest::SHA3.hexdigest("foo")    # => "1597842a..."

    # Generate 224-bit digest.
    Digest::SHA3.digest("foo", 224)       # => "\332\251M\247..."
    Digest::SHA3.hexdigest("foo", 224)    # => "daa94da7..."

    # Use this interface to feed data in chunks. 512-bit by default.
    digest = Digest::SHA3.new
    digest.update("f")
    digest.update("o")
    digest.update("o")
    digest.digest       # => "\025\227\204*..."
    digest.hexdigest    # => "1597842a..."

    # You can pass a hash length to the constructor.
    digest = Digest::SHA3.new(224)

## Running the test suite

Run the test suite as follows:

    make test

A part of the test suite is automatically generated from Keccak's reference test suite.

## Warning

Do not use SHA-3 for hashing passwords. Do not even use SHA-3 + salt for hashing passowords. Use a [slow hash](http://codahale.com/how-to-safely-store-a-password/) instead.
