open Migrate_parsetree.Ast_403;

let module Convert = Migrate_parsetree.Convert(Migrate_parsetree.OCaml_403)(Migrate_parsetree.OCaml_current);

let show_structure structure => {
  Pprintast.structure Format.str_formatter (Convert.copy_structure structure);
  Format.flush_str_formatter();
};

let show_error input result expected => {
  print_endline ">> Input:";
  print_endline (show_structure input);
  print_endline ">> Output:";
  print_endline (show_structure result);
  print_endline ">> Expected:";
  print_endline (show_structure expected);
};

let fixtures = [(
  [%str let doit something => {
    [%guard let Ok value = doAnAction something][@else None];
    /* etc */
    Some 10
  }],
  [%str let doit something => {
    switch (doAnAction something) {
    | Ok value => Some 10
    | _ => None
    }
  }]
), (
  [%str let multiple x y => {
    [%guard let Some x = x][@else None];
    let awesome = 10;
    [%guard let Some y = y][@else None];
    Some (x + y)
  }],
  [%str let multiple x y => {
    switch x {
    | Some x => {
      let awesome = 10;
      switch y {
        | Some y => Some (x + y)
        | _ => None
      }
    }
    | _ => None
    }
  }]
)];

let run () => {
  let (total, failures) =
  List.fold_left (fun (total, failures) (input, expected) => {
    try {
    let result = Lib.mapper.structure Lib.mapper input;
    if (result != expected) {
      show_error input result expected;
      (total + 1, failures + 1)
    } else {
      (total + 1, failures)
    }
    } {
      | Location.Error error => {
        print_endline ">> Input:";
        print_endline (show_structure input);
        print_endline ">> Error:";
        print_endline error.Location.msg;
        (total + 1, failures + 1)
      }
    }
  }) (0, 0) fixtures;

  if (failures !== 0) {
      Printf.printf "Total: %d, Failures: %d" total failures;
    exit 1;
  } else {
    Printf.printf "All %d succeeded!" total;
    exit 0;
  }
};

run ();
