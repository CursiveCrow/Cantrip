modal File {
    state Closed { path: String; }
    state Open   { path: String; handle: FileHandle; }

    @Closed >> open() >> @Open needs fs.read { Open { path: self.path, handle: open(self.path)? } }
    @Open   >> close() >> @Closed { Closed { path: self.path } }
}

function demo()
    needs fs.read;
{
    let f = File.new("data.txt");
    let f = f.open()?;
    let f = f.close();
}
