UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
  FORMAT=elf64
  NOPIE=-nopie
else
ifeq ($(UNAME), Darwin)
  FORMAT=macho6
  NOPIE=-Wl,-no_pie
endif
endif

PKGS=oUnit,extlib,unix,sexplib,str

BUILD=ocamlbuild -r -use-ocamlfind -pkg $(PKGS) $(OCAMLOPT)

test: runner.ml test.ml
	mkdir -p output
	$(BUILD) test.native
	mv test.native test

output/%.run: output/%.o main.c gc.c print.c
	clang $(NOPIE) -fsanitize=address -g -mstackrealign -o $@ main.c gc.c print.c $<

output/%.o: output/%.s
	nasm -f $(FORMAT) -o $@ $<

output/%.s: input/%.boa compilers/main
	mkdir -p output
	./main $< > $@

clean:
	rm -rf output/*.o output/*.s output/*.dSYM output/*.run *.log
	rm -rf _build/
