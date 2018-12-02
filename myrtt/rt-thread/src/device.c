#include <rtthread.h>

rt_device_t rt_device_find(const char *name)
{
	struct rt_object *object;
	struct rt_list_node *node;
	struct rt_object_information *information;

	information = rt_object_get_information(RT_Object_Class_Device);
	for(node = information->object_list.next;
		node != &(information->object_list);
		node = node->next
	)
	{
		object = rt_list_entry(node, struct rt_object, list);
		if(rt_strncmp(object->name, name, RT_NAME_MAX) == 0) {
			return (rt_device_t)object;
		}
	}
	return RT_NULL;
}


rt_err_t rt_device_close(rt_device_t dev)
{
	rt_err_t ret;
	if(dev->ref_count == 0) {
		return -RT_ERROR;
	}
	dev->ref_count --;
	if(dev->ref_count != 0) {
		return RT_EOK;
	}
	if(dev->close != RT_NULL) {
		ret = dev->close(dev);
	}
	if(ret == RT_EOK || ret == -RT_ENOSYS) {
		dev->open_flag = RT_DEVICE_OFLAG_CLOSE;
	}
	return ret;
}

