#include "button.hpp"

#include <sys/alt_irq.h>
#include <altera_avalon_pio_regs.h>

buttons_c::buttons_c(const uint32_t base_address, const uint32_t interrupt_controller, const uint32_t irq_id):
	base_address(base_address),
	interrupt_controller(interrupt_controller),
	irq_id(irq_id)
{}

void buttons_c::isr(void* isr_context){
	buttons_c* buttons = (buttons_c*)isr_context;
	buttons->edge_capture = IORD_ALTERA_AVALON_PIO_EDGE_CAP(buttons->base_address);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(buttons->base_address, 0x0);
	buttons->m_callback(buttons->edge_capture);
}

void buttons_c::init(void (*callback)(uint8_t pin)){
	m_callback = callback;
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(base_address, 0xF);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(base_address, 0x0);
	alt_ic_isr_register(interrupt_controller, irq_id, isr, this, 0x0);
}

bool buttons_c::pressed(const uint8_t pin){
	return (IORD_ALTERA_AVALON_PIO_DATA(base_address) & pin) == 0;
}
