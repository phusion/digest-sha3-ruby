$LOAD_PATH.unshift(File.expand_path("lib"))
$LOAD_PATH.unshift(File.expand_path("ext"))
require 'digest/sha3'
require File.expand_path('test/test_usage')
require File.expand_path('test/test_vectors')
