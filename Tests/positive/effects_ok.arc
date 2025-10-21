// Should compile: uses heap with declared effect
function make_vec(): Vec<i32>
    needs alloc.heap;
{
    Vec.new()
}
