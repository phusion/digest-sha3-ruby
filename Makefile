.phony: all clean test

all: ext/digest/Makefile
	make -C ext/digest

ext/digest/Makefile: ext/digest/extconf.rb
	cd ext/digest && ruby extconf.rb

clean:
	if [[ -f ext/digest/Makefile ]]; then make -C ext/digest clean; fi
	rm -f ext/digest/Makefile
	rm -f test/test_vectors.rb

test: all test/test_vectors.rb
	ruby test/test_all.rb

test/test_vectors.rb: test/generate_tests.rb test/data/*
	ruby test/generate_tests.rb > test/test_vectors.rb
