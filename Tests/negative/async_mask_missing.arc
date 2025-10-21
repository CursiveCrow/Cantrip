// Expect: E9201 (awaited net.read(outbound) effect missing from enclosing function)
async function bad(url: String): String
    needs alloc.heap;
{
    let body = await http.get(url)?; // E9201
    body
}
