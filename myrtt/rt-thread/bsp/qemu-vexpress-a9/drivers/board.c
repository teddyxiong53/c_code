

void rt_hw_board_init()
{
	rt_hw_interrupt_init();
	rt_system_heap_init(HEAP_BEGIN, HEAP_END);
}