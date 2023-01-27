use kernel::prelude::*;

module! {
	type: RustHello,
	name: b"rust_hello",
	author: b"Dheeraj Playground",
	description: b"Scratchpad",
	license: b"GPL",
}

struct RustHello {
	numbers: Vec<i32>,
}

impl kernel::Module for RustHello {
	fn init (_module: &'static ThisModule) -> Result<Self> {
		pr_info!("Rust Hello init\n");
		
		let mut numbers = Vec::new();
		numbers.try_push(10)?;
		numbers.try_push(20)?;
		numbers.try_push(30)?;

		Ok(RustHello {numbers})
	}
}


impl Drop for RustHello {
	fn drop(&mut self) {
		pr_info!("Numbers are {:?}\n", self.numbers);
		pr_info!("Rust Hello cleanup\n");
	}
}
