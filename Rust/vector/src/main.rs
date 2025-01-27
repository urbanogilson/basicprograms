use vector::Vector;

fn main() {
    let mut v: Vector<u32> = Vector::new();

    v.push(1);
    v.push(2);
    v.push(3);
    v.push(4);
    
    v.pop();

    v[0] = 4;
    
    println!("Len: {}", v.len());
    println!("Capacity: {}", v.capacity());
    println!("Empty: {}", v.is_empty());
    
    for i in v.iter_mut() {
        *i *=2;
    }
    
    
    print!("Elements: ");
    for i in v.iter() {
        print!("{i} ");
    }
    println!()
}
