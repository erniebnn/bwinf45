Properties of a Node:

ID       : u32
Name     : str?
Birthdate: unix timestamp
Mother   : Node
Father   : Node

Every query is an operation that takes a set of nodes, and returns a set of nodes.

Selectors:
    `father`
    `mother`
    `children`
    `partners`

Filters:
    `oldest`
    `youngest`
    `where(prop op val)`
    prop: name, bdate, 
    op: ==, !=, >, <, >=, <=

Query Grammar:

```ebnf

```