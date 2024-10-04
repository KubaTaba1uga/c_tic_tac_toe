## Double init issue

Once you include .c file the init registration macro is executed twice,
it can be avoided by creating header file only for tests, sth like:
```
extern struct WhateverPrivateOps;
```

Now we can include this header to test file, which should give us access to private ops.

If this doesn't work i propose to attach somehow private ops to public ops, and create
copy of private ops definition in tests, because copy is 1:1 match obj alignment is the same and 
test can because of that dereference private ops from public ops.
