
while {1} {
    after 100
    thread::send -async $worker "set flipflop 1"
    after 100
    thread::send -async $worker "set flipflop 0"
}
