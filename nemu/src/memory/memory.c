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
  if(cpu.PG == 1){
    uint32_t base1 = (uint32_t)cpu.cr3;
    uint32_t pde = (uint32_t)paddr_read((uint32_t)((get_pdx(addr)) + base1), 4);
    if (!(pde & 0x1)) {
			Log("addr = 0x%x, iswrite = %d", addr, iswrite);
			Log("pde = 0x%x, pde_base = 0x%x, index = 0x%x", pde, (uint32_t)base1, (uint32_t)(get_pdx(addr) + base1));
			assert(0);
		}

    uint32_t base2 = (uint32_t)get_pte_addr(pde);
    uint32_t pte = (uint32_t)paddr_read((uint32_t)(get_ptx(addr) + base2), 4);
    assert(pte & 0x1);

    paddr_t page_address = get_pte_addr(pte) | get_off(addr);

    pde |= 0x20;
    pte |= 0x20;

    if (iswrite) {
      pde |= 0x40;
			pte |= 0x40;
		}
    paddr_write((uint32_t)(get_pdx(addr) + base1), 4, pde);
		paddr_write((uint32_t)(get_ptx(addr) + base2), 4, pte);

    return page_address;
  }
  else{
    return addr;
  }
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  if(get_pte_addr(addr) != get_pte_addr(addr + len -1)){
    Log("in different pages\n");
    assert(0);
  }
  else{
    paddr_t paddr = page_translate(addr, false);
		return paddr_read(paddr, len);
  }
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  if(get_pte_addr(addr) != get_pte_addr(addr + len -1)){
    Log("in different pages\n");
    assert(0);
  }
  else{
    paddr_t paddr = page_translate(addr, true);
		paddr_write(paddr, len, data);
  }
}
