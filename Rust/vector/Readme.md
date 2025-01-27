### Vector Implementation in Rust

This project provides a custom implementation of a dynamic array (vector) in Rust, showcasing low-level memory management using raw pointers, manual allocation, and deallocation. It mimics the behavior of Rust's standard `Vec` while allowing for hands-on learning about Rust's ownership and safety features.

---

### Features

- **Dynamic resizing**: Automatically grows when capacity is exceeded.
- **Push and pop operations**: Add or remove elements from the vector.
- **Indexing support**: Access and modify elements using `Index` and `IndexMut`.
- **Iterators**: Provides both immutable and mutable iterators.
- **Custom drop logic**: Ensures proper cleanup and memory deallocation.

---

### Usage

Here's an example of how to use the `Vector`:

```rust
fn main() {
    let mut v: Vector<u32> = Vector::new();

    // Add elements
    v.push(1);
    v.push(2);
    v.push(3);

    // Access elements
    println!("First element: {}", v[0]);

    // Modify elements
    v[1] = 42;
    println!("Updated second element: {}", v[1]);

    // Remove elements
    let popped = v.pop();
    println!("Popped element: {:?}", popped);

    // Check length and capacity
    println!("Length: {}, Capacity: {}", v.len(), v.capacity());

    // Iterate over elements
    for val in v.iter() {
        println!("{}", val);
    }
}
```

---

### Tests

Run the included tests to verify functionality:

```bash
cargo test
```


### Caveats

- This implementation is not as optimized or feature-complete as Rust's standard `Vec`. It is intended for educational purposes only.
- It does not handle out-of-memory errors beyond panicking.