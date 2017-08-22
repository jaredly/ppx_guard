# ppx_guard

`guard let` for reason/ocaml!

## What?
A shortcut for early returns when you know it's going south :P

```reason
let doit something => {
  [%guard let Ok value = doAnAction something][@else None];
  /* etc */
  Some 10
}
/* becomes */
let doit something => {
  switch (doAnAction something) {
    | Ok value => Some 10
    | _ => None
  }
}


let multiple x y => {
  [%guard let Some x = x][@else None];
  let awesome = 10;
  [%guard let Some y = y][@else None];
  Some (x + y)
}
/* becomes */
let multiple x y => {
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
}
```


## Related work / inspiration

Inspired by swift's [`guard let`](https://thatthinginswift.com/guard-statement-swift/), [`if let`, `if case`, `guard case`](http://alisoftware.github.io/swift/pattern-matching/2016/05/16/pattern-matching-4/), etc. It currently acts similar to swift's `guard case`.

It's also inspired by rust's [`try!` macro](https://doc.rust-lang.org/std/macro.try.html) (which is now deprecated in favor of a bit of syntax sugar, the `?` postfix operator, which does the same thing).

It might be interesting to add a `[%try` function, which would have the same
effect as rust's try macro. If we start to use `result` a lot, it could be
worth it.

## FAQ

#### Why not have it act only on `option` (as swift does with `guard let`)?
I considered that, but I think it might
not be as common as in swift. An alternative would be to have a `guard_case`
that does what this currently does, and then `guard` would be for unwrapping
optionals.
