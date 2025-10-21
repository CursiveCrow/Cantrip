async function fetch(url: String): String
    needs net.read(outbound), alloc.heap;
{
    let body = await http.get(url)?;
    body
}
