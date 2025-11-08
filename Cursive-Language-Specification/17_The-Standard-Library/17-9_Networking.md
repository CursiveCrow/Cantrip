# Cursive Language Specification

## Clause 17 — The Standard Library

**Part**: XVII — The Standard Library  
**File**: 17-9_Networking.md  
**Section**: §17.9 Networking  
**Stable label**: [stdlib.net]  
**Forward references**: §17.2 [stdlib.core], §17.6 [stdlib.io], Clause 12 [contract]

---

### §17.9 Networking [stdlib.net]

#### §17.9.1 Overview

[1] The `std::net` module provides TCP/UDP networking capabilities. All network operations require `net::*` grants and use union types for error handling.

[2] **Informative Note**: The networking API specified here provides foundation-level functionality. Higher-level protocols (HTTP, WebSockets, etc.) are expected to be provided by third-party libraries built on this foundation.

#### §17.9.2 Socket Address Types

##### §17.9.2.1 SocketAddr

[3] Network address representation:

```cursive
public enum SocketAddr {
    V4 { ip: [u8; 4], port: u16 },
    V6 { ip: [u8; 16], port: u16 },
}

public procedure SocketAddr::from_string(addr: string@View): SocketAddr \/ ParseError
    [[ alloc::heap |- addr.len() > 0 => true ]]
{
    // Parse "127.0.0.1:8080" or "[::1]:8080"
}
```

#### §17.9.3 TCP Socket Modal Type

##### §17.9.3.1 TcpStream States

[4] TCP connection lifecycle:

```cursive
public modal TcpStream {
    @Disconnected { }

    @Disconnected::connect(address: SocketAddr): TcpStream@Connected \/ IoError

    @Connected {
        socket_handle: i32,
        peer_addr: SocketAddr,
    }

    @Connected::read(~%, buffer: [u8]): usize \/ IoError
    @Connected::write(~!, data: [u8]): usize \/ IoError
    @Connected::shutdown(~!): TcpStream@Disconnected
}

public procedure TcpStream::connect(addr: SocketAddr): TcpStream@Connected \/ IoError
    [[ net::connect, alloc::heap |- true => true ]]
{
    // Platform-specific socket connection
}
```

#### §17.9.4 TCP Listener

##### §17.9.4.1 TcpListener Type

[5] Accepting incoming connections:

```cursive
public modal TcpListener {
    @Unbound { }

    @Unbound::bind(address: SocketAddr): TcpListener@Listening \/ IoError

    @Listening {
        socket_handle: i32,
        local_addr: SocketAddr,
    }

    @Listening::accept(~%): TcpStream@Connected \/ IoError
    @Listening::close(~!): TcpListener@Unbound
}

public procedure TcpListener::bind(addr: SocketAddr): TcpListener@Listening \/ IoError
    [[ net::listen, alloc::heap |- true => true ]]
{
    // Platform-specific socket binding and listening
}
```

#### §17.9.5 UDP Socket

##### §17.9.5.1 UdpSocket Type

[6] Connectionless datagram socket:

```cursive
public modal UdpSocket {
    @Unbound { }

    @Unbound::bind(address: SocketAddr): UdpSocket@Bound \/ IoError

    @Bound {
        socket_handle: i32,
        local_addr: SocketAddr,
    }

    @Bound::send_to(~%, data: [u8], addr: SocketAddr): usize \/ IoError
    @Bound::receive_from(~%, buffer: [u8]): (usize, SocketAddr) \/ IoError
    @Bound::close(~!): UdpSocket@Unbound
}
```

#### §17.9.6 Network Error Types

##### §17.9.6.1 NetworkError

[7] Network-specific errors:

```cursive
public enum NetworkError {
    ConnectionRefused,
    ConnectionReset,
    Timeout,
    AddressInUse,
    AddressNotAvailable,
    WouldBlock,
}
```

#### §17.9.7 Conformance Requirements

[8] Implementations shall:

1. Provide SocketAddr with IPv4 and IPv6 support
2. Implement TcpStream modal type with all states
3. Implement TcpListener for accepting connections
4. Implement UdpSocket for datagram communication
5. Use appropriate `net::*` grants
6. Return union types for all fallible operations
7. Support cross-platform via comptime conditionals

---

**Previous**: §17.7 Concurrency Support (§17.7 [stdlib.concurrency]) | **Next**: §17.10 Platform-Specific APIs (§17.10 [stdlib.os])
