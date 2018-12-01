#ifndef __RT_DEF_H__
#define __RT_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif

#define RT_VERION 3L
#define RT_SUBVERSION 0L
#define RT_REVISION 2L

#define RTTHREAD_VERSION ((RT_REVISION*10000) + (RT_SUBVERSION*100) + (RT_REVISION))

typedef unsigned char rt_int8_t;
typedef signed char rt_uint8_t;

typedef unsigned short rt_uint16_t;
typedef signed short rt_int16_t;

typedef unsigned int rt_uint32_t;
typedef signed int rt_int32_t;

typedef int rt_bool_t;

typedef long rt_base_t;

typedef unsigned long rt_ubase_t;

typedef rt_base_t rt_err_t;

typedef rt_uint32_t rt_time_t;

typedef rt_uint32_t rt_tick_t;

typedef rt_base_t rt_flag_t;

typedef rt_ubase_t  rt_size_t;

typedef rt_ubase_t rt_dev_t;

typedef rt_base_t rt_off_t;

#define RT_TRUE 	1
#define RT_FALSE 	0

#define RT_UINT8_MAX 0xff

#define RT_UINT16_MAX 0xffff

#define RT_UINT32_MAX 0xffffffff

#define RT_TICK_MAX RT_UINT32_MAX

#include <stdarg.h>

#define SECTION(x) __attribute__((section(x)))
#define RT_UNUSED 		__attribute__((unused))
#define RT_USED 		__attribute__((used))
#define ALIGN(n)		__attribute__((aligned(n)))

#define RT_WEAK			__attribute__((weak))

#define rt_inline 		static __inline

#ifdef RT_USING_COMPONENTS_INIT
typedef int (*init_fn_t)(void);

#define INIT_EXPORT(fn, level) \
	const init_fn_t __rt_init_##fn SECTION(".rti_fn."level) = fn
	
	
#endif

#define INIT_BOARD_EXPORT(fn) 	INIT_EXPORT(fn, "1")

#define INIT_PREV_EXPORT(fn)	INIT_EXPORT(fn, "2")

#define INIT_DEVICE_EXPORT(fn)	INIT_EXPORT(fn, "3")

#define INIT_COMPONENT_EXPORT(fn)	INIT_EXPORT(fn, "4")

#define INIT_ENV_EXPORT(fn)		INIT_EXPORT(fn, "5")

#define INIT_APP_EXPORT(fn)		INIT_EXPORT(fn, "6")


#define RT_EVENT_LENGTH		32

#define RT_MM_PAGE_SIZE		4096

#define RT_MM_PAGE_MASK		(RT_MM_PAGE_SIZE-1)

#define RT_MM_PAGE_BITS		12

#define RT_KERNEL_MALLOC(sz)		rt_malloc(sz)

#define RT_KERNEL_FREE(ptr)			rt_free(ptr)

#define RT_KERNEL_REALLOC(ptr, size)	rt_realloc(ptr, size)


//error code
#define RT_OK	0
#define RT_ERROR	1
#define RT_ETIMEOUT		2
#define	 RT_EFULL 	3
#define RT_EEMPTY	4
#define RT_ENOMEM	5
#define RT_ENOSYS	6
#define RT_EBUSY	7
#define RT_EIO		8
#define RT_EINTR	9
#define RT_EINVAL	10

#define RT_ALIGN(size, align)	(((size) + (align) - 1) & ~((align) - 1))

#define RT_ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))

#define RT_NULL (0)

struct rt_list_node {
	struct rt_list_node *next;
	struct rt_list_node *prev;
	
};

typedef struct rt_list_node rt_list_t;

struct rt_slist_node {
	struct rt_slist_node *next;
};

typedef struct rt_slist_node rt_slist_t;


#define RT_OBJECT_FLAG_MODULE 0x80

struct rt_object {
	char name[RT_NAME_MAX];
	rt_uint8_t type;
	rt_uint8_t flag;
	
	rt_list_t list;
};

typedef struct rt_object * rt_object_t;

enum rt_object_class_type {
	RT_Object_Class_Thread = 0,
	RT_Object_Class_Semaphore,
	RT_Object_Class_Mutex,
	RT_Object_Class_Event,
	RT_Object_Class_MailBox,
	RT_Object_Class_MessageQueue,
	RT_Object_Class_MemHeap,
	RT_Object_Class_MemPool,
	RT_Object_Class_Device,
	RT_Object_Class_Timer,
	RT_Object_Class_Module,
	RT_Object_Class_Unknown,
	RT_Object_Class_Static = 0x80
};

struct rt_object_information {
	enum rt_object_class_type type;
	rt_list_t object_list;
	rt_size_t object_size;
};

//thread

//thread state
#define RT_THREAD_INIT 0x00
#define RT_THREAD_
#define RT_THREAD_READY 0X01
#define RT_THREAD_SUSPEND 0X02
#define RT_THREAD_RUNNING 0X03
#define RT_THREAD_BLOCK RT_THREAD_SUSPEND
#define RT_THREAD_CLOSE		0x04

#define RT_THREAD_STAT_MASK 	0x0f

struct rt_thread {
	char name[RT_NAME_MAX];
	rt_uint8_t type;
	rt_uint8_t flags;
	
	rt_list_t list; //object list
	rt_list_t tlist; //thread list
	
	void *sp;//stack pointer
	
	void *entry;
	
	void *parameter;
	
	void *stack_addr;
	rt_uint32_t stack_size;
	rt_err_t error;
	rt_uint8_t stat;
	
	rt_uint8_t current_priority;
	rt_uint8_t init_priority;
	
	
	
	
};


#ifdef __cplusplus
}
#endif

#endif