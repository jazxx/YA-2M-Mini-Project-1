#!/usr/bin/env bash
set -euo pipefail

project_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
binary="$project_dir/airports"
data_file="$project_dir/data/airports.dat"

if [[ ! -x "$binary" ]]; then
    g++ -std=c++17 -O2 "$project_dir"/src/*.cpp -o "$binary"
fi

core_output="$("$binary" <<EOF
load $data_file
find BOS
find GKA
find AAC
find YZR
find XYZ
find 000
find ZZZ
min
max
range BOS BWI
validate
rebalance
quit
EOF
)"

grep -Fq "Loaded 6072 airports (1626 lines skipped: no IATA code). Duplicates: 0" <<<"$core_output"
grep -Fq "Height: 177  Average depth: 18.323" <<<"$core_output"
grep -Fq "BOS  General Edward Lawrence Logan International Airport" <<<"$core_output"
grep -Fq "Path: GKA  (comparisons: 1)" <<<"$core_output"
grep -Fq "AAC  El Arish International Airport" <<<"$core_output"
grep -Fq "YZR  Chris Hadfield Airport" <<<"$core_output"
grep -Fq "XYZ not found" <<<"$core_output"
grep -Fq "000 not found" <<<"$core_output"
grep -Fq "ZZZ not found" <<<"$core_output"
grep -Fq "minimum airport is AAA" <<<"$core_output"
grep -Fq "maximum airport is ZZV" <<<"$core_output"
grep -Fq "113 airports, 127 nodes visited" <<<"$core_output"
grep -Fq "Height: 12  Average depth: 10.653" <<<"$core_output"

check_delete() {
    local code="$1"
    local expected_case="$2"
    local output
    output="$("$binary" <<EOF
load $data_file
remove $code
quit
EOF
)"
    grep -Fq "Deleted $code (case $expected_case)  validate: OK" <<<"$output"
}

check_delete AAC "D1: leaf"
check_delete GJT "D2: only a left child, GJR"
check_delete AAA "D2: only a right child, AAC"
check_delete AEY "D3: two children, successor AFA"
check_delete GKA "D3: two children, successor GKE"

preorder_file="$(mktemp "${TMPDIR:-/tmp}/airport-bst-preorder.XXXXXX")"
trap 'rm -f "$preorder_file"' EXIT

reload_output="$("$binary" <<EOF
load $data_file
save $preorder_file
load $preorder_file
quit
EOF
)"
grep -Fq "Reloaded 6072 airports from preorder. Duplicates: 0" <<<"$reload_output"
grep -Fq "Height: 177  Average depth: 18.323" <<<"$reload_output"
grep -Fq "Level 0: GKA" <<<"$reload_output"
grep -Fq "Level 1: AEY MAG" <<<"$reload_output"
grep -Fq "Level 2: AAE EGS HGU POM" <<<"$reload_output"

qqq_output="$("$binary" <<EOF
load $data_file
add QQQ "Yasmein Asfour Airport" "Boston" "United States" KQQQ 42.3601 -71.0589
find QQQ
remove QQQ
find QQQ
quit
EOF
)"
grep -Fq "Added QQQ  validate: OK" <<<"$qqq_output"
grep -Fq "QQQ  Yasmein Asfour Airport" <<<"$qqq_output"
grep -Fq "Deleted QQQ (case D1: leaf)  validate: OK" <<<"$qqq_output"
grep -Fq "QQQ not found" <<<"$qqq_output"

echo "All Airport BST tests passed."
