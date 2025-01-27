use std::{
    alloc::{alloc, dealloc, Layout},
    ops::{Index, IndexMut},
    ptr,
};

#[derive(Debug)]
pub struct Vector<T> {
    pointer: *mut T,
    capacity: usize,
    size: usize,
}

impl<T> Default for Vector<T> {
    fn default() -> Self {
        Vector {
            pointer: core::ptr::null_mut(),
            capacity: 0,
            size: 0,
        }
    }
}

impl<T> Vector<T> {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn len(&self) -> usize {
        self.size
    }

    pub fn capacity(&self) -> usize {
        self.capacity
    }

    pub fn is_empty(&self) -> bool {
        self.size == 0
    }

    fn resize(&mut self) {
        let new_capacity = if self.capacity == 0 {
            16
        } else {
            self.capacity * 2
        };

        let new_layout = Layout::array::<T>(new_capacity).unwrap();
        unsafe {
            let new_ptr = alloc(new_layout) as *mut T;
            if !self.pointer.is_null() {
                ptr::copy_nonoverlapping(self.pointer, new_ptr, self.size);
                let old_layout = Layout::array::<T>(self.capacity).unwrap();
                dealloc(self.pointer as *mut u8, old_layout);
            }

            self.pointer = new_ptr;
            self.capacity = new_capacity;
        }
    }

    pub fn push(&mut self, value: T) {
        if self.capacity == self.size {
            self.resize();
        }

        unsafe {
            ptr::write(self.pointer.add(self.size), value);
        }

        self.size += 1;
    }

    pub fn pop(&mut self) -> Option<T> {
        if self.size == 0 {
            return None;
        }
        self.size -= 1;
        unsafe { Some(ptr::read(self.pointer.add(self.size))) }
    }

    pub fn iter(&self) -> std::slice::Iter<'_, T> {
        unsafe { std::slice::from_raw_parts(self.pointer, self.size).iter() }
    }

    pub fn iter_mut(&mut self) -> std::slice::IterMut<'_, T> {
        unsafe { std::slice::from_raw_parts_mut(self.pointer, self.size).iter_mut() }
    }
}

impl<T> Drop for Vector<T> {
    fn drop(&mut self) {
        if !self.pointer.is_null() {
            unsafe {
                for i in 0..self.size {
                    ptr::drop_in_place(self.pointer.add(i));
                }

                if let Ok(layout) = Layout::array::<T>(self.capacity) {
                    dealloc(self.pointer as *mut u8, layout);
                }
            }
        }
    }
}

impl<T> Index<usize> for Vector<T> {
    type Output = T;

    fn index(&self, index: usize) -> &Self::Output {
        if index > self.size {
            panic!("Index out of bounds");
        }

        unsafe { &*self.pointer.add(index) }
    }
}

impl<T> IndexMut<usize> for Vector<T> {
    fn index_mut(&mut self, index: usize) -> &mut Self::Output {
        if index > self.size {
            panic!("Index out of bounds");
        }

        unsafe { &mut *self.pointer.add(index) }
    }
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn it_creates_an_empty_vector() {
        let v: Vector<u8> = Vector::new();
        assert!(v.pointer.is_null());
        assert_eq!(v.capacity(), 0);
        assert_eq!(v.len(), 0);
        assert!(v.is_empty());
    }

    #[test]
    fn it_can_access_an_element_at_position() {
        let mut v: Vector<u8> = Vector::new();
        v.push(16);
        assert_eq!(v.len(), 1);
        assert_eq!(v[0], 16);
    }

    #[test]
    fn it_can_update_an_element_at_position() {
        let mut v: Vector<u8> = Vector::new();
        v.push(16);
        v[0] = 8;
        assert_eq!(v.len(), 1);
        assert_eq!(v[0], 8);
    }

    #[test]
    fn it_can_pop_an_element() {
        let mut v: Vector<u8> = Vector::new();
        assert_eq!(v.pop(), None);

        v.push(8);

        assert_eq!(v.len(), 1);
        assert_eq!(v[0], 8);

        assert_eq!(v.pop(), Some(8));
        assert_eq!(v.pop(), None);
    }

    #[test]
    fn it_can_iter_over_elements() {
        let mut v: Vector<u8> = Vector::new();

        v.push(1);
        v.push(2);
        v.push(3);

        assert_eq!(v.len(), 3);

        let mut iterator = v.iter();

        assert_eq!(iterator.next(), Some(&1));
        assert_eq!(iterator.next(), Some(&2));
        assert_eq!(iterator.next(), Some(&3));
        assert_eq!(iterator.next(), None);
    }

    #[test]
    fn it_can_iter_mut_over_elements() {
        let mut v: Vector<u8> = Vector::new();

        v.push(1);
        v.push(2);
        v.push(3);

        assert_eq!(v.len(), 3);

        for i in v.iter_mut() {
            *i *= 2;
        }

        let mut iterator = v.iter();

        assert_eq!(iterator.next(), Some(&2));
        assert_eq!(iterator.next(), Some(&4));
        assert_eq!(iterator.next(), Some(&6));
        assert_eq!(iterator.next(), None);
    }

    #[test]
    fn it_resizes_when_capacity_is_reached() {
        let mut v: Vector<usize> = Vector::new();

        assert_eq!(v.capacity(), 0);

        v.push(0);

        assert_eq!(v.capacity(), 16);

        for i in 0..v.capacity() {
            v.push(i);
        }

        assert_eq!(v.capacity(), 32);
    }
}
