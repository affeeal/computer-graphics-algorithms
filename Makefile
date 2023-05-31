.PHONY: clean generate build \
	lab1 lab2 lab3 lab4 lab5

clean:
	rm -rf build

generate:
	cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -S . -B build/

silent-build:
	cmake --build build/

build:
	@make -s silent-build 

lab1:
	./build/lab1/lab1

lab2:
	./build/lab2/lab2

lab3:
	./build/lab3/lab3

lab4:
	./build/lab4/lab4

lab5:
	./build/lab5/lab5
