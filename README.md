# Airport Code Lookup Service

This project is a C++ program that stores airport information in a binary search tree (BST). Each airport is organized by its three-letter IATA code. The program can search for airports, add or remove airports, list them in alphabetical order, and find airports within a range of codes.

The airport data comes from OpenFlights. Records without an IATA code are skipped when the file is loaded.

## Build and run

Compile the program with:

```bash
g++ -std=c++17 -O2 src/*.cpp -o airports
```

Run it with:

```bash
./airports
```

## Sample session

```text
> load data/airports.dat
Loaded 6072 airports (1626 lines skipped: no IATA code). Duplicates: 0
Nodes: 6072  Height: 177  Average depth: 18.323
Validate: OK

> find BOS
Path: GKA -> AEY -> EGS -> BUJ -> BJA -> BSK -> BOY -> BNI -> BOH -> BOO -> BOU -> BOS  (comparisons: 12)
BOS  General Edward Lawrence Logan International Airport, Boston, United States

> range BOS BWI
113 airports, 127 nodes visited

> remove BOS
Deleted BOS (case D2: only a left child, BOR)  validate: OK

> rebalance
Nodes: 6071  Height: 12  Average depth: 10.653
Validate: OK
```

## Tree-shape experiment

We built the tree using the airport codes in different orders and timed 10,000 searches.

| Build order | Height | Average depth | Search time |
|---|---:|---:|---:|
| File order | 177 | 18.323 | 7.592 ms |
| Sorted order | 6,071 | 3,035.500 | 324.207 ms |
| Balanced | 12 | 10.653 | 3.324 ms |

The search times can be slightly different on another computer.

The sorted tree is the slowest because inserting sorted codes creates a long chain. Its height is 6,071, so a search may need to visit thousands of nodes. The balanced tree has a height of only 12 because the middle code is selected at each step. This makes searching much faster. The file-order tree is also unbalanced because the airport file is grouped instead of being in random IATA-code order.

BST operations take `O(h)` time, where `h` is the height of the tree. A smaller height usually means faster searching, insertion, and deletion.

## Demo videos

- [Demo Part 1](https://youtu.be/7mMD7evsa1c)
- [Demo Part 2: add, find, and remove QQQ](https://youtu.be/LFwiUs4Al4c?si=Xs-r1mi-l-U9g9WG)

## Data credit

Airport data is provided by [OpenFlights.org](https://openflights.org/data.php) under the Open Database License.
