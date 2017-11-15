
let fail loc txt => raise (Location.Error (Location.error ::loc txt));

let process_let contents loc => {
    open Parsetree;
    let bindings = switch contents {
    | PStr [{pstr_desc: Pstr_value Nonrecursive bindings, pstr_loc}] => bindings
    | _ => fail loc "guard must contain a nonrecursive let binding"
    };

    let binding = switch bindings {
    | [binding] => binding
    | _ => fail loc "only one binding supported atm"
  };
  (binding.pvb_pat, binding.pvb_expr)
};

let getExpr contents loc => {
  open Parsetree;
  switch contents {
  | PStr [{pstr_desc: Pstr_eval expr _}] => expr
  | _ => fail loc "@else must contain an expression"
  }
};

let mapper _args => Parsetree.{
  ...Ast_mapper.default_mapper,

  expr: fun mapper expr => {
    switch expr.pexp_desc {
    | Pexp_sequence {pexp_desc: Pexp_extension ({txt: "guard"}, contents), pexp_loc, pexp_attributes} next => {
        let (pat, expr) = process_let contents pexp_loc;
        switch pexp_attributes {
        | [({txt: "else"}, else_contents)] => {
          [%expr
            switch [%e expr] {
            | [%p pat] => [%e mapper.expr mapper next]
            | _ => [%e mapper.expr mapper (getExpr else_contents pexp_loc)]
            }
          ]
        }
        | _ => fail pexp_loc "No @else attribute given"
        }
    }

    | _ => Ast_mapper.default_mapper.expr mapper expr
    }
  }
};
