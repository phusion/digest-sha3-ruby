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
    assert_equal 'df70adc49b2e76eee3a6931b93fa41841c3af2cdf5b32a18b5478c39',
      Digest::SHA3.hexdigest("\xcc", 224)
  end

  def test_update
    a = init(224)
    a.update("\xcc")
    assert_equal 'df70adc49b2e76eee3a6931b93fa41841c3af2cdf5b32a18b5478c39', a.hexdigest
  end

  def test_updates
    a = init(224)
    a.update("\x21")
    a.update("\xf1\x34")
    a.update("\xac\x57")
    assert_equal '10e580a32199596169331ad43cfcf10264f81565037040028a06b458', a.hexdigest
  end

  def test_empty_224
    a = init(224)
    assert_equal '6b4e03423667dbb73b6e15454f0eb1abd4597f9a1b078e3f5b5a6bc7', a.hexdigest
  end

  def test_empty_256
    a = init(256)
    assert_equal 'a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a', a.hexdigest
  end

  def test_empty_384
    a = init(384)
    assert_equal '0c63a75b845e4f7d01107d852e4c2485c51a50aaaa94fc61995e71bbee983a2ac3713831264adb47fb6bd1e058d5f004', a.hexdigest()
  end

  def test_empty_512
    a = init(512)
    assert_equal 'a69f73cca23a9ac5c8b567dc185a756e97c982164fe25859e0d1dcc1475c80a615b2123af1f5f94c11e3e9402c3ac558f500199d95b6d3e301758586281dcd26', a.hexdigest
  end
end
