record Account { balance: f64; }

impl Account {
    function withdraw(self: mut Account, amount: f64)
        requires amount > 0.0;
        requires self.balance >= amount;
        ensures  self.balance == @old(self.balance) - amount;
    { self.balance -= amount; }
}
