open Runner
open Printf
open OUnit2

let default_heap_size = "100000"

let is_osx = Conf.make_bool "osx" false "Set this flag to run on osx"
let wrap_prog program = "((define our_main (input) " ^ program ^ "))"

let tgc_main name heap_size program expected =
  name>::test_run (wrap_prog program) name expected [string_of_int heap_size]
let tgc_i_main name heap_size program expected args =
  name>::test_run (wrap_prog program) name expected ([string_of_int heap_size]@args)
let tgc_prog name heap_size program expected =
  name>::test_run program name expected [string_of_int heap_size]

let tgcerr_main name heap_size program expected =
  name>::test_err (wrap_prog program) name expected [string_of_int heap_size]


let t_i_prog name program expected args =
  name>::test_run program name expected (default_heap_size::args)
let t_prog name program expected =
  name>::test_run program name expected [default_heap_size]
let tvg_i_prog name program expected args =
  name>::test_vg_run program name expected (default_heap_size::args)
let tvg_prog name program expected =
  name>::test_vg_run program name expected [default_heap_size]
let t_i_main name program expected args =
  name>::test_run (wrap_prog program) name expected (default_heap_size::args)
let t_main name program expected =
  name>::test_run (wrap_prog program) name expected [default_heap_size]
let tvg_i_main name program expected args =
  name>::test_vg_run (wrap_prog program) name expected (default_heap_size::args)
let tvg_main name program expected =
  name>::test_vg_run (wrap_prog program) name expected [default_heap_size]
let terr_i_prog name program expected args =
  name>::test_err program name expected (default_heap_size::args)
let terr_prog name program expected =
  name>::test_err program name expected [default_heap_size]
let terr_i_main name program expected args =
  name>::test_err (wrap_prog program) name expected (default_heap_size::args)
let terr_main name program expected =
  name>::test_err (wrap_prog program) name expected [default_heap_size]
let t_f test_type = (fun (name,program,expected) ->
    test_type name program expected)
let f_to_s fname = Runner.string_of_file ("input/" ^ fname ^ ".diamondback")
let lst_main body = (f_to_s "list") ^ body ^ "))"
let bst_main body = (f_to_s "bst") ^ body ^ "))"

(* All of thse are correct tests -- if they fail, something is broken *)
let oom = [
  tgcerr_main "oomgc1" 7 "(tup 1 (tup 3 4))" "Out of memory";
  tgc_main "oomgc2" 8 "(tup 1 (tup 3 4))" "(1,(3,4))";
  tgc_main "oomgc3" 10 "(tup 1 (tup 3 4))" "(1,(3,4))";
  tgc_main "oomgc4" 4 "(tup 3 4)" "(3,4)";
  tgcerr_main "oomgc5" 3 "(tup 3 4)" "Allocation";
]

let gc_tests = [
  tgc_prog "overwrite_var" 8
  "(\
      (define our_main (_)\
        (let ((t (tup 1 2)))\
          (begin\
            (t := (tup 3 4))\
            (t := (tup 5 6))\
            (t := (tup 7 8))\
            t))\
      )\
  )\
  " "(7,8)"
]

(* Add tests here as you write them! *)

let suite =
  "suite">:::
  oom @ gc_tests

let () =
  run_test_tt_main suite
;;

