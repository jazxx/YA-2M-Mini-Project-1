# Airport BST Pseudocode

The tree is ordered by the three-character IATA code. Tree height and node depth count edges, so the root is at depth 0 and a one-node tree has height 0.

## Build from the airport file

```
BUILD_FROM_FILE(file)
    create an empty airport array
    create an empty BST
    for every line in file
        parse the line with a quote-aware CSV parser
        if the IATA field is missing (\N)
            increase skipped count
        else
            create an Airport from fields 2 through 8
            append the Airport to the array
            INSERT(Airport)
    report inserted, skipped and duplicate counts
    report HEIGHT(root) and AVERAGE_DEPTH(root)
```

## Insert

```
INSERT(airport)
    if root is NULL
        root = new node containing airport
        return success

    current = root
    while current is not NULL
        if airport.code equals current.code
            report duplicate and return failure
        if airport.code is smaller than current.code
            if current.left is NULL
                current.left = new node containing airport
                return success
            current = current.left
        else
            if current.right is NULL
                current.right = new node containing airport
                return success
            current = current.right
```

## Search

```
SEARCH(code)
    current = root
    comparisons = 0
    path = empty list

    while current is not NULL
        append current.code to path
        comparisons = comparisons + 1
        if code equals current.code
            return current airport, path and comparisons
        if code is smaller than current.code
            current = current.left
        else
            current = current.right

    return not found, path and comparisons
```

## Delete

```
DELETE(code)
    search iteratively for the node and the link from its parent
    if node is not found
        return failure

    if node has no children                         // D1
        replace the parent link with NULL
    else if node has only a left child              // D2
        replace the parent link with node.left
    else if node has only a right child             // D2
        replace the parent link with node.right
    else                                             // D3
        find the smallest node in the right subtree
        copy that airport into the node being deleted
        remove the smallest node from the right subtree

    decrease tree size
    return success and the deletion case
```

## Inorder traversal

```
INORDER(root)
    create an empty stack
    current = root
    while current is not NULL or stack is not empty
        while current is not NULL
            push current
            current = current.left
        current = pop stack
        output current airport
        current = current.right
```

## Preorder save and reload

```
SAVE_PREORDER(root, file)
    if root is NULL
        return
    push root onto a stack
    while stack is not empty
        node = pop stack
        write node.code
        push node.right if it exists
        push node.left if it exists

LOAD_PREORDER(file)
    require the original airport catalog to be loaded
    clear the current tree
    for every code in file
        find the complete Airport record in the catalog
        INSERT(record)
    report the height and the top three levels
```

## Validate with ranges

```
VALIDATE(node, low, high)
    if node is NULL
        return true
    if low exists and node.code is less than or equal to low
        return false
    if high exists and node.code is greater than or equal to high
        return false

    leftIsValid = VALIDATE(node.left, low, node.code)
    rightIsValid = VALIDATE(node.right, node.code, high)
    return leftIsValid AND rightIsValid

To validate the whole tree, call VALIDATE(root, no lower bound, no upper bound).
```

## Inclusive range query

```
RANGE(node, from, to)
    if node is NULL
        return
    increase visited count
    if node.code is greater than from
        RANGE(node.left, from, to)
    if from <= node.code <= to
        output node airport
    if node.code is less than to
        RANGE(node.right, from, to)
```

## Balanced build

```
BUILD_BALANCED(sortedAirports, begin, end)
    if begin >= end
        return NULL
    middle = begin + (end - begin) / 2
    node = new node containing sortedAirports[middle]
    node.left = BUILD_BALANCED(sortedAirports, begin, middle)
    node.right = BUILD_BALANCED(sortedAirports, middle + 1, end)
    return node

REBALANCE()
    sortedAirports = INORDER(root)
    delete the old tree
    root = BUILD_BALANCED(sortedAirports, 0, sortedAirports.size)
```

## Height and average depth

```
TREE_METRICS(root)
    if root is NULL
        return height -1 and average depth 0
    enqueue (root, depth 0)
    while queue is not empty
        remove (node, depth)
        add depth to total depth
        update maximum depth
        enqueue each child with depth + 1
    return maximum depth and total depth / node count
```
