#include "nemu.h"
#include "device/mmio.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t get_pte_addr(uint32_t pde){
  return (pde & 0xfffff000);
}

uint32_t get_pdx(uint32_t va){
  return ((va >> 22) & 0x000003ff) << 2;
}

uint32_t get_ptx(uint32_t va){
  return ((va & 0x003ff000) >> 10);
}

uint32_t get_off(uint32_t va){
  return (va & 0x00000fff);
}

uint32_t paddr_read(paddr_t addr, int len) {
  int num = is_mmio(addr);
  if(num == -1)
    return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
  else
    return mmio_read(addr, len, num); 
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  int num = is_mmio(addr);
  if(num == -1)
    memcpy(guest_to_host(addr), &data, len);
  else 
    mmio_write(addr, len, data, num);
}

uint32_t page_translate(vaddr_t addr, bool iswrite){
  // if(cpu.PG == 1){
  //   uint32_t base1 = (uint32_t)cpu.cr3;
  //   uint32_t pde = (uint32_t)paddr_read((uint32_t)((get_pdx(addr)) + base1), 4);
  //   if (!(pde & 0x1)) {
	// 		Log("addr = 0x%x, iswrite = %d", addr, iswrite);
	// 		Log("pde = 0x%x, pde_base = 0x%x, index = 0x%x", pde, (uint32_t)base1, (uint32_t)(get_pdx(addr) + base1));
	// 		assert(0);
	// 	}

  //   uint32_t base2 = (uint32_t)get_pte_addr(pde);
  //   uint32_t pte = (uint32_t)paddr_read((uint32_t)(get_ptx(addr) + base2), 4);
  //   assert(pte & 0x1);

  //   paddr_t page_address = get_pte_addr(pte) | get_off(addr);

  //   pde |= 0x20;
  //   pte |= 0x20;

  //   if (iswrite) {
  //     pde |= 0x40;
	// 		pte |= 0x40;
	// 	}
  //   paddr_write((uint32_t)(get_pdx(addr) + base1), 4, pde);
	// 	paddr_write((uint32_t)(get_ptx(addr) + base2), 4, pte);

  //   return page_address;
  // }
  // else{
  //   return addr;
  // }

  if (cpu.PG == 1) {
		paddr_t pde_base = cpu.cr3;
		paddr_t pde_address = pde_base + ((addr >> 22) << 2);
		paddr_t pde = paddr_read(pde_address, 4);
		if (!(pde & 0x1)) {
			Log("addr = 0x%x, iswrite = %d", addr, iswrite);
			Log("pde = 0x%x, pde_base = 0x%x, pde_address = 0x%x", pde, pde_base, pde_address);
			assert(0);
		}

		paddr_t pte_base = pde & 0xfffff000;
		//paddr_t pte_address = pte_base + (((addr & 0x003ff000) >> 12) << 2);
		paddr_t pte_address = pte_base + ((addr & 0x003ff000) >> 10);
		paddr_t pte = paddr_read(pte_address, 4);
		if (!(pte & 0x1)) {
			Log("addr = 0x%x, iswrite = %d", addr, iswrite);
			Log("pte = 0x%x", pte);
			assert(0);
		}
		paddr_t page_address = (pte & 0xfffff000) + (addr & 0xfff);
		
		// set the access and dirty
		pde = pde | 0x20;
		pte = pte | 0x20;
		if (iswrite) {
			pde = pde | 0x40;
			pte = pte | 0x40;
		}
		paddr_write(pde_address, 4, pde);
		paddr_write(pte_address, 4, pte);
	
		return page_address;
	}
	else {
		return addr;
	}
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  // if(get_pte_addr(addr) != get_pte_addr(addr + len -1)){
  //   // Log("in different pages\n");
  //   // assert(0);
	// 	int boundary = (int)(get_off(addr) + len - 0x1000);

	// 	uint32_t low_paddr = page_translate(addr, false);
	// 	uint32_t low = paddr_read(low_paddr, len - boundary);

	// 	uint32_t high_paddr = page_translate(addr + len - boundary, false);
	// 	uint32_t high = paddr_read(high_paddr, boundary);

	// 	uint32_t paddr = (high << ((len - boundary) << 3)) + low;
	// 	return paddr;
  // }
  // else{
  //   paddr_t paddr = page_translate(addr, false);
	// 	return paddr_read(paddr, len);
  // }

  if (((addr & 0xfff) + len) > 0x1000) {
		//Log("in the read!!!!!!!!!!!!!!!!!!!!!!!!");
		/* this is a special case, you can handle it later. */
		int point;
		paddr_t paddr, low, high;
		// calculate the split point
		point = (int)((addr & 0xfff) + len - 0x1000);
		// get the low address
		paddr = page_translate(addr, false);
		low = paddr_read(paddr, len - point);
		// get the low address
		paddr = page_translate(addr + len - point, false);
		high = paddr_read(paddr, point);
		paddr = (high << ((len - point) << 3)) + low;
		return paddr;
	}
	else {
		paddr_t paddr = page_translate(addr, false);
		return paddr_read(paddr, len);
	}
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  // if(get_pte_addr(addr) != get_pte_addr(addr + len -1)){
  //   // Log("in different pages\n");
  //   // assert(0);
	// 	int boundary = (int)(get_off(addr) + len - 0x1000);

	// 	uint32_t low = (data << (boundary << 3)) >> (boundary << 3);
	// 	uint32_t high = data >> ((len - boundary) << 3);
	
	// 	uint32_t low_paddr = page_translate(addr, true);
	// 	paddr_write(low_paddr, len - boundary, low);

	// 	uint32_t high_paddr = page_translate(addr + len - boundary, true);
	// 	paddr_write(high_paddr, boundary, high);
  // }
  // else{
  //   paddr_t paddr = page_translate(addr, true);
	// 	paddr_write(paddr, len, data);
  // }

  if (((addr & 0xfff) + len) > 0x1000) {
		int point;
		uint32_t low, high;
		paddr_t paddr;
		// calculate the split point 
		point = (int)((addr & 0xfff) + len - 0x1000);
		// split the date into the high and low
		low = (data << (point << 3)) >> (point << 3);
		high = data >> ((len - point) << 3);
	
		//Log("addr = %x, high = %x, low = %x, point = %d", addr, high, low, point);
		// store the low data
		paddr = page_translate(addr, true);
		paddr_write(paddr, len - point, low);
		// store the high data
		paddr = page_translate(addr + len - point, true);
		paddr_write(paddr, point, high);
	}
	else {
		//Log("i am here~");
		paddr_t paddr = page_translate(addr, true);
		paddr_write(paddr, len, data);
	}
}
