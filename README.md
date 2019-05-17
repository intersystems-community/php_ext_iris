# Function list

**All functions (except *iris_get*) returns TRUE or FALSE if error occurs**

## Service function

### Get error code

`iris_errno();`

Returns: DB error code

### Get error message

`iris_error();`

Returns: DB error message

### Setup connection: DB directory name

`iris_set_dir('/usr/irissys/mgr');`

### Connect to DB

`iris_connect("login","password");`

### Disconnect from DB

`iris_quit();`

## Data function

*First argument may be array.*
In this case this array contains full path to global node beginning with global name

Example:

`array("^time", "subscript1", ..., "subscriptN")`

### Set global nodes examles

`iris_set('^time',1); // Set ^time=1`

`iris_set('^time','tree',1,1,'value'); // Set ^time("tree", 1, 1) = "value"`

### Get global node value

`iris_get('^time',1); // Set ^time=1`

Returns: Value of global node, NULL - if node undefined, FALSE - by error.

### Traverse global on same level

`iris_order('^ccc','new2','res2'); // $Order(^ccc("new2", "res2"))`

### Traverse global in depth

`iris_query('^ccc','new2','res2'); // $Query(^ccc("new2", "res2"))`

Returns: always array with full path to global node beginning with global name

### Reverse traverse global on same level

`iris_order_rev('^ccc','new2','res2'); // $Order(^ccc("new2", "res2"), -1)`

### Killing one node

`iris_zkill('^forKill','global');  // ZKill ^forKill("global")`

### Killing entire tree

`iris_kill('^forKill','global');  // Kill ^forKill("global")`

### Execute DB command

`iris_exec("kill ^global(6)");`
