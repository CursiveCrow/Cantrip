// Expect: E3004 (use of moved value / explicit move required)
record Data { }

function consume(own d: Data) { }

function demo()
    needs alloc.heap;
{
    let d = Data.new();
    consume(d); // E3004: must write consume(move d)
}
