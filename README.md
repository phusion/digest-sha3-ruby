# The SHA-3 (FIPS 202) extension for Ruby

This Ruby extension implements the SHA-3 Standard [FIPS 202 (pdf)](http://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.202.pdf) cryptographic hashing algorithm. It is based on the reference C implementation, version 3.2. The exposed interface is almost identical to that of the `digest` standard library.

[<img src="http://www.phusion.nl/assets/logo.png">](http://www.phusion.nl/)

## Installation

    gem install digest-sha3

## Bundler Installation

    # Add this to your Gemfile
    gem 'digest-sha3', require: 'digest'

## Notes

- As of version 2.0.0, digest-sha3 generates FIPS 202 standard hashes. The last version that generated Keccak hashes was 1.1.0.
- As of version 1.1.0, digest-sha3 requires Ruby 2.2. The last version that worked on older versions was 1.0.2.

## Usage

SHA-3 supports 5 hash lengths: 224-bit, 256-bit, 384-bit, 512-bit and variable length. Variable length is not supported by this Ruby extension. Unless the user specifies otherwise, this Ruby extension assumes 512-bit.

```ruby
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
```

## Running the test suite

Run the test suite as follows:

    make test

## Warning

Do not use SHA-3 for hashing passwords. Do not even use SHA-3 + salt for hashing passowords. Use a [slow hash](http://codahale.com/how-to-safely-store-a-password/) instead.

## See also

- [node-sha3](https://github.com/phusion/node-sha3)
- [sha3](https://github.com/johanns/sha3)
- [sha3-pure-ruby](https://github.com/havenwood/sha3-pure-ruby)
- [sha3 calculators](http://emn178.github.io/online-tools/sha3_256.html)
