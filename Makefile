UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
  FORMAT=aout
  ARGS=
else
ifeq ($(UNAME), Darwin)
  FORMAT=macho
  ARGS=-Wl,-no_pie
endif
endif

PKGS=oUnit,extlib,unix,sexplib,str

BUILD=corebuild -r -use-ocamlfind -pkg $(PKGS) $(OCAMLOPT)

test: runner.ml test.ml
	mkdir -p output
	$(BUILD) test.native
	mv test.native test

output/%.run: output/%.o main.c gc.c
	clang -g -m32 $(ARGS) -mstackrealign -o $@ gc.c main.c $<

output/%.o: output/%.s
	nasm -f $(FORMAT) -o $@ $<

output/%.s: input/%.garter compilers/main
	mkdir -p output
	./compilers/main $< > $@

clean:
	rm -rf output/*.o output/*.s output/*.dSYM output/*.run *.log
	rm -rf _build/
