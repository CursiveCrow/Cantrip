# Product Type Examples

**Related Specification**: [02_ProductTypes.md](../PART_B_Composite/02_ProductTypes.md)

This file contains practical examples for Cantrip's product types: tuples, records, and tuple structs.

---

## Part 1: Tuple Examples

### 1.1 Basic Usage

**Multiple return values:**

```cantrip
function divide_with_remainder(x: i32, y: i32): (i32, i32)
    must y != 0
{
    (x / y, x % y)
}

let (quotient, remainder) = divide_with_remainder(17, 5)
// quotient = 3, remainder = 2
```

**Coordinate systems:**

```cantrip
// 2D point
let point: (f64, f64) = (3.0, 4.0)
let distance = ((point.0 * point.0) + (point.1 * point.1)).sqrt()

// RGB color
let color: (u8, u8, u8) = (255, 128, 0)
let (r, g, b) = color
```

### 1.2 Multiple Return Values Pattern

**Error handling (before Result enum):**

```cantrip
function parse_number(s: str): (Option<i32>, str) {
    if let Some(n) = try_parse(s) {
        (Some(n), "")
    } else {
        (None, "invalid number")
    }
}

let (result, error) = parse_number("123")
match result {
    Some(n) => println("Parsed: {n}"),
    None => println("Error: {error}"),
}
```

**Splitting operations:**

```cantrip
function split_at_first_space(s: str): (str, str) {
    if let Some(index) = s.find(' ') {
        (s[..index], s[index + 1..])
    } else {
        (s, "")
    }
}

let (first_word, rest) = split_at_first_space("hello world")
```

### 1.3 Temporary Grouping Pattern

**Function arguments:**

```cantrip
function distance(p1: (f64, f64), p2: (f64, f64)): f64 {
    let dx = p2.0 - p1.0
    let dy = p2.1 - p1.1
    (dx * dx + dy * dy).sqrt()
}

let d = distance((0.0, 0.0), (3.0, 4.0))  // 5.0
```

**Swapping values:**

```cantrip
function swap<T>(a: T, b: T): (T, T) {
    (b, a)
}

let (x, y) = swap(10, 20)  // x = 20, y = 10
```

### 1.4 Nested Tuples

**Complex structures:**

```cantrip
// Before parsing into proper records
let person: (str, i32, (str, str)) =
    ("Alice", 30, ("alice@example.com", "+1-555-0123"))

let (name, age, (email, phone)) = person
```

**Matrix operations:**

```cantrip
function matrix_multiply(
    a: ((f64, f64), (f64, f64)),
    b: ((f64, f64), (f64, f64))
): ((f64, f64), (f64, f64)) {
    // 2x2 matrix multiplication
    let ((a11, a12), (a21, a22)) = a
    let ((b11, b12), (b21, b22)) = b
    (
        (a11 * b11 + a12 * b21, a11 * b12 + a12 * b22),
        (a21 * b11 + a22 * b21, a21 * b12 + a22 * b22),
    )
}
```

### 1.5 Pattern Matching with Tuples

**Destructuring in match:**

```cantrip
function classify_point(p: (i32, i32)): str {
    match p {
        (0, 0) => "origin",
        (0, _) => "on y-axis",
        (_, 0) => "on x-axis",
        (x, y) if x == y => "on diagonal",
        (x, y) if x > 0 && y > 0 => "quadrant I",
        _ => "other",
    }
}
```

**Nested destructuring:**

```cantrip
let data: ((i32, i32), (i32, i32)) = ((1, 2), (3, 4))

match data {
    ((0, 0), _) => println("First point is origin"),
    (_, (0, 0)) => println("Second point is origin"),
    ((x1, y1), (x2, y2)) => {
        println("Points: ({x1}, {y1}) and ({x2}, {y2})")
    }
}
```

### 1.6 Common Pitfalls: When to Prefer Records

❌ **BAD: Tuple with unclear meaning**

```cantrip
function get_user_info(): (str, i32, str, bool) {
    ("Alice", 30, "alice@example.com", true)
}

let info = get_user_info()
let name = info.0     // What is .0?
let age = info.1      // What is .1?
let email = info.2    // What is .2?
let active = info.3   // What is .3?
```

✅ **GOOD: Use a record instead**

```cantrip
record UserInfo {
    name: str
    age: i32
    email: str
    active: bool
}

function get_user_info(): UserInfo {
    UserInfo {
        name: "Alice",
        age: 30,
        email: "alice@example.com",
        active: true,
    }
}

let info = get_user_info()
let name = info.name    // Clear semantic meaning
```

**Rule of thumb:** If a tuple has more than 3 elements or will be used in multiple places, use a record instead.

---

## Part 2: Record Examples

### 2.1 Basic Usage

**Simple record:**

```cantrip
record Point {
    x: f64
    y: f64
}

let p = Point { x: 3.0, y: 4.0 }
let distance = ((p.x * p.x) + (p.y * p.y)).sqrt()
```

**Field shorthand:**

```cantrip
let x = 3.0
let y = 4.0
let point = Point { x, y }  // Equivalent to Point { x: x, y: y }
```

**Generic record:**

```cantrip
record Pair<T, U> {
    first: T
    second: U
}

let pair = Pair { first: 42, second: "hello" }
```

### 2.2 Basic Data Modeling

[Examples to be extracted from lines 1074-1103]

### 2.3 Builder Pattern

[Examples to be extracted from lines 1104-1151]

### 2.4 Newtype Pattern

[Examples to be extracted from lines 1152-1178]

### 2.5 Composition Over Inheritance

[Examples to be extracted from lines 1179-1212]

---

## Part 3: Tuple Struct Examples

### 3.1 Basic Usage

[Examples to be extracted from lines 1579-1713]

### 3.2 Comparison: When to Use What

[Comparison examples to be extracted from lines 1560-1578]

---

**Note**: This file is being populated during Phase 2 of specification cleanup. Some sections are placeholders pending full extraction.

**Back to specifications**: [02_ProductTypes.md](../PART_B_Composite/02_ProductTypes.md)
