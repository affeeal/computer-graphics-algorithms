.PHONY: clean generate build \
	lab1 lab2 lab3 lab4 lab5 lab6

clean:
	rm -rf build

generate:
	cmake -S . -B build/

silent-build:
	cmake --build build/

build:
	@make -s silent-build 
	
