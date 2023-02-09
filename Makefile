.PHONY: clean generate build lab1

clean:
	rm -rf build

generate:
	cmake -S . -B build/

silent-build:
	cmake --build build/

build:
	@make -s silent-build 

lab1:
	./build/lab1/lab1
