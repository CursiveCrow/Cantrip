// Expect: E9010 (Forbidden effect is redundant outside wildcard/polymorphic cases)
function broken()
    needs alloc.heap, !fs.delete;
{
    Vec.new()
}
