# encoding: ascii
Digest::SHA3.class_eval do
  def self.hexdigest(*args)
    force_ascii(digest(*args).unpack("H*").first)
  end

  def hexdigest
    Digest::SHA3.force_ascii(digest.unpack("H*").first)
  end

  if ''.respond_to?(:force_encoding)
    def self.force_ascii(str)
      str.force_encoding('ascii')
    end
  else
    def self.force_ascii(str)
      str
    end
  end
end
