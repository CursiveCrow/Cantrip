# Cursive Language Specification

## Clause 13 — Witness System

**Clause**: 13 — Witness System
**File**: 13-8_Diagnostics.md
**Section**: §13.8 Diagnostics
**Stable label**: [witness.diagnostics]  
**Forward references**: §13.1-13.7 (all witness subclauses), Annex E §E.5 [implementation.diagnostics]

---

### §13.8 Diagnostics [witness.diagnostics]

#### §13.8.1 Overview

[1] This subclause consolidates all diagnostics for the witness system, provides conformance requirements, and specifies integration with other language subsystems.

[2] Witness system diagnostics cover formation errors, dispatch failures, permission violations, region escapes, and grant violations. All diagnostics follow the canonical format `E13-[NNN]` as specified in §1.6.6 [intro.document.diagnostics].

#### §13.8.2 Complete Diagnostic Catalog [witness.diagnostics.catalog]

[3] Clause 13 defines the following diagnostic codes:

##### §13.8.2.1 Formation and Construction Diagnostics (E13-001 through E13-012)

| Code    | Description                                                  | Section    |
| ------- | ------------------------------------------------------------ | ---------- |
| E13-001 | Type does not implement behavior/satisfy contract/have state | §13.3.6[1] |
| E13-010 | Missing `move` for responsible witness (@Heap/@Region)       | §13.3.6[2] |
| E13-011 | Region witness escapes region                                | §13.3.6[5] |
| E13-012 | Grant missing for witness construction (alloc::heap/region)  | §13.3.6[4] |

##### §13.8.2.2 Dispatch Diagnostics (E13-020 through E13-022)

| Code    | Description                                 | Section     |
| ------- | ------------------------------------------- | ----------- |
| E13-020 | Method not found in behavior/contract       | §13.5.8[7]  |
| E13-021 | Method not available in modal state         | §13.5.5[10] |
| E13-022 | Insufficient grants for witness method call | §13.5.7[14] |

##### §13.8.2.3 Memory Integration Diagnostics (E13-030 through E13-032)

| Code    | Description                                 | Section     |
| ------- | ------------------------------------------- | ----------- |
| E13-030 | Permission upgrade on witness               | §13.6.4[6]  |
| E13-031 | Witness transition requires grant           | §13.6.7[20] |
| E13-032 | Invalid witness transition (no such method) | §13.6.7[19] |

#### §13.8.3 Diagnostic Payloads [witness.diagnostics.payloads]

[4] Witness diagnostics shall follow Annex E §E.5 structured payload format. Required fields for each diagnostic family:

**Formation diagnostics** (E13-001, E13-010, E13-011, E13-012):

- Source value type
- Target witness type
- Missing implementation/state (for E13-001)
- Source location
- Target allocation state
- Required grants (for E13-012)

**Dispatch diagnostics** (E13-020, E13-021, E13-022):

- Witness type
- Method name
- Available methods list (for E13-020)
- Modal state (for E13-021)
- Required grants vs available grants (for E13-022)

**Memory integration diagnostics** (E13-030, E13-031, E13-032):

- Witness type
- Source permission / target permission (for E13-030)
- Transition method name
- Required grant (for E13-031)

#### §13.8.4 Consolidated Conformance Requirements [witness.diagnostics.conformance]

[5] A conforming implementation of the witness system shall:

**Type System** (§13.1-§13.2):

1. Provide `witness<Property>@State` as built-in modal type
2. Support behavior, contract, and modal state witnesses
3. Default witness types to @Stack allocation state
4. Unify all witness kinds with same dense pointer representation

**Formation** (§13.3): 5. Support automatic witness coercion from implementing types 6. Verify type implements/satisfies witness property 7. Create non-responsible @Stack witnesses without `move` 8. Require explicit `move` for @Heap/@Region witnesses 9. Enforce grant requirements for witness construction 10. Apply region escape analysis to region witnesses

**Representation** (§13.4): 11. Implement witnesses as dense pointers (16 bytes on 64-bit) 12. Structure witness tables with type ID, size, align, state tag, drop, vtable 13. Share witness tables across instances 14. Support type erasure hiding concrete types

**Dispatch** (§13.5): 15. Support method calls using `::` operator 16. Implement vtable-based dynamic dispatch 17. Type-check method existence and availability 18. Propagate grants from method sequents 19. Optimize to static dispatch when concrete type known (optional)

**Memory Integration** (§13.6): 20. Map allocation states to cleanup responsibility 21. Integrate with RAII for destructor calls 22. Enforce region escape prevention 23. Inherit permissions from source values 24. Support witness transitions between allocation states 25. Enforce grant requirements for transitions

**Grammar** (§13.7): 26. Parse witness types with angle bracket syntax 27. Support allocation state annotations 28. Integrate with Annex A type and expression grammars

**Diagnostics**: 29. Emit all diagnostics E13-001 through E13-032 30. Provide structured diagnostic payloads per Annex E §E.5 31. Maintain witness metadata for reflection and tooling

#### §13.8.5 Integration Summary [witness.diagnostics.integration]

[6] The witness system integrates with:

**Behaviors (Clause 10)**:

- Behavior declarations compile to witness tables
- Behavior implementations provide vtable entries
- Generic bounds (`<T: B>`) vs witness types (`witness<B>`) trade static/dynamic

**Contracts (Clause 12)**:

- Contract declarations compile to witness tables
- Contract implementations provide vtable entries
- Dynamic verification hooks generate witness evidence

**Modal Types (§7.6)**:

- Modal states trackable dynamically via witnesses
- State-specific procedures in modal witness vtables
- Witness state obligations in transition sequents

**Memory Model (Clause 11)**:

- Witness allocation states map to cleanup responsibility
- Region witnesses subject to escape analysis
- Permissions inherited from values
- Move semantics apply to witness construction

**Type System (Clause 7)**:

- Witnesses are modal types with allocation states
- Dense pointer representation defined
- Method dispatch typed through behavior/contract signatures

#### §13.8.6 Complete Example [witness.diagnostics.example]

**Example 13.8.6.1 (Comprehensive witness usage)**:

```cursive
behavior Drawable {
    procedure draw(~)
        [[ io::write |- true => true ]]
    { }

    procedure bounds(~): Rectangle
    { result Rectangle { x: 0.0, y: 0.0, width: 0.0, height: 0.0 } }
}

record Point with Drawable {
    x: f64,
    y: f64,

    procedure draw(~)
        [[ io::write |- true => true ]]
    {
        println("Point({}, {})", self.x, self.y)
    }

    procedure bounds(~): Rectangle
    {
        result Rectangle { x: self.x, y: self.y, width: 0.0, height: 0.0 }
    }
}

record Circle with Drawable {
    center: Point,
    radius: f64,

    procedure draw(~)
        [[ io::write |- true => true ]]
    {
        println("Circle(center: {}, radius: {})", self.center, self.radius)
    }

    procedure bounds(~): Rectangle
    {
        result Rectangle {
            x: self.center.x - self.radius,
            y: self.center.y - self.radius,
            width: self.radius * 2.0,
            height: self.radius * 2.0
        }
    }
}

// Witness construction and usage
procedure render_shapes(shapes: [witness<Drawable>])
    [[ io::write |- shapes.len() > 0 => true ]]
{
    loop shape: witness<Drawable> in shapes {
        shape::draw()                          // Dynamic dispatch
        let bounds = shape::bounds()          // Dynamic dispatch
        println("Bounds: {}x{}", bounds.width, bounds.height)
    }
}

// Usage
procedure main(): i32
    [[ alloc::heap, io::write |- true => true ]]
{
    let point = Point { x: 1.0, y: 2.0 }
    let circle = Circle {
        center: Point { x: 5.0, y: 5.0 },
        radius: 3.0
    }

    // Create heap witnesses (heterogeneous collection)
    let shapes: [witness<Drawable>@Heap] = [
        move point,   // Coerced to witness<Drawable>@Heap
        move circle   // Coerced to witness<Drawable>@Heap
    ]

    render_shapes(shapes)

    result 0
}
```

#### §13.8.7 Conformance Requirements [witness.grammar.requirements]

[7] Implementations shall:

1. Parse witness types per §13.7.2 grammar
2. Support witness method calls per §13.7.4 grammar
3. Integrate witness grammar with Annex A
4. Emit syntax diagnostics for malformed witness types or calls
5. Maintain AST representation of witnesses for type checking

---

**Previous**: §13.6 Regions, Permissions, and Grants Interplay (§13.6 [witness.memory]) | **Next**: Clause 14 — Concurrency and Memory Ordering (§14 [concurrency])
