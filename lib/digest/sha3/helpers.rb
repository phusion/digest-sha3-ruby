Digest::SHA3.class_eval do
  def self.hexdigest(*args)
    digest(*args).unpack("H*").first
  end

  def hexdigest
    digest.unpack("H*").first
  end
end
