// Expect: E3003 (procedure not available in current state)
modal F { state A{} state B{} @A >> go() >> @B { B{} } }
function demo() {
    let x = F.new();
    // x.go(); // OK after state A
    let y = x.go();
    // Now in state B; calling go again should be rejected at compile time:
    let z = y.go(); // E3003
}
