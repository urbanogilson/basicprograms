dependencies:
	rustup component add llvm-tools-preview
	cargo install bootimage

boot:
	qemu-system-x86_64 -drive format=raw,file=target/x86_64-os/debug/bootimage-os.bin

clean:
	rm -r target

.PHONY: dependencies boot clean