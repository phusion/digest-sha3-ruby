require 'test/unit'

class SHA3UsageTest < Test::Unit::TestCase
  def init(hashsize = 512)
    @digest = Digest::SHA3.new(hashsize)
  end

  def test_copy
    a = init(224)
    a.update('foo')
    b = a.dup
    assert_equal b.digest, a.digest
    b.update('bar')
    assert_not_equal b.digest, a.digest
  end

  def test_class_methods
    assert_equal 'a9cab59eb40a10b246290f2d6086e32e3689faf1d26b470c899f2802',
      Digest::SHA3.hexdigest("\xcc", 224)
  end

  def test_update
    a = init(224)
    a.update("\xcc")
    assert_equal 'a9cab59eb40a10b246290f2d6086e32e3689faf1d26b470c899f2802', a.hexdigest
  end

  def test_updates
    a = init(224)
    a.update("\x21")
    a.update("\xf1\x34")
    a.update("\xac\x57")
    assert_equal '5573da2b02216a860389a581f6e9fb8d805e9e02f6fa911701eee298', a.hexdigest
  end

  def test_empty_224
    a = init(224)
    assert_equal 'f71837502ba8e10837bdd8d365adb85591895602fc552b48b7390abd', a.hexdigest
  end

  def test_empty_256
    a = init(256)
    assert_equal 'c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470', a.hexdigest
  end

  def test_empty_384
    a = init(384)
    assert_equal '2c23146a63a29acf99e73b88f8c24eaa7dc60aa771780ccc006afbfa8fe2479b2dd2b21362337441ac12b515911957ff', a.hexdigest()
  end

  def test_empty_512
    a = init(512)
    assert_equal '0eab42de4c3ceb9235fc91acffe746b29c29a8c366b7c60e4e67c466f36a4304c00fa9caf9d87976ba469bcbe06713b435f091ef2769fb160cdab33d3670680e', a.hexdigest
  end
end
