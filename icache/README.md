# Function list

**All functions (except *cache_get*) returns TRUE or FALSE if error occurs**

## Service function

### Get error code

`cach_errno();`

Returns: DB error code

### Get error message

`cach_error();`

Returns: DB error message

### Setup connection: DB directory name

`cach_set_dir('/InterSystems/Cache/mgr');`

### Connect to DB

`cach_connect("login","password");`

### Disconnect from DB

`cach_quit();`

## Data function

*First argument may be array.*
In this case this array contains full path to global node beginning with global name

Example:

`array("^time", "subscript1", ..., "subscriptN")`

### Set global nodes examles

`cach_set('^time',1); // Set ^time=1`

`cach_set('^time','tree',1,1,'value'); // Set ^time("tree", 1, 1) = "value"`

### Get global node value

`cach_get('^time',1); // Set ^time=1`

Returns: Value of global node, NULL - if node undefined, FALSE - by error.

### Traverse global on same level

`cach_order('^ccc','new2','res2'); // $Order(^ccc("new2", "res2"))`

### Traverse global in depth

`cach_query('^ccc','new2','res2'); // $Query(^ccc("new2", "res2"))`

Returns: always array with full path to global node beginning with global name

### Reverse traverse global on same level

`cach_order_rev('^ccc','new2','res2'); // $Order(^ccc("new2", "res2"), -1)`

### Killing one node

`cach_zkill('^forKill','global');  // ZKill ^forKill("global")`

### Killing entire tree

`cach_kill('^forKill','global');  // Kill ^forKill("global")`

### Execute DB command

`cach_exec("kill ^global(6)");`
